// src/core/storm/storm_batch_b66.cpp -- Storm batch b66: transcribed from the cached 1.13c decompiles.
// Stage A lane (implementation + marker only). Retail layouts are 32-bit; here pointer-bearing
// objects are modelled as structs with the retail byte offsets in comments (wasm64 pointers).
// Register inputs (in_EAX/unaff_EBX/EDI/ECX) are explicit leading params, commented per function.
// Skipped: 0x14250 (1.9 KB state machine, 15 callees), 0x52D0 (SEH + eh_vector), 0x6A28/0x6D32/
// 0x7572/0x76FA (MSVC CRT, TLS/locale), 0x1CE50 (direct GDI).
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <Windows.h>
#include "storm_image_leaves.h"

// Capture-probe seams (src/core/fn/storm_28e80.cpp): the cache fill and the sector read, replaced by recorders; null = the empty-state default.
extern "C" {
uint32_t (*g_storm_b66_cachefill)(void* file, uint32_t off) = nullptr;
uint32_t (*g_storm_b66_read)(void* file, uint32_t off, void* dst, uint32_t len) = nullptr;
}

namespace {

// ---- seams for unmapped callees (provider/cache state is empty in ours) ---------------------
uint8_t g_crcTable[256 * 4];                                   // 0x6FC41600 CRC32 table (4 B/entry)
uint32_t SeamCacheFill28DF0(void* f, uint32_t off) { return g_storm_b66_cachefill ? g_storm_b66_cachefill(f, off) : 0; }  // 0x28DF0 (file in ESI, offset in EDX) -> bytes valid
uint32_t SeamRead28450(void* f, uint32_t o, void* d, uint32_t n) { return g_storm_b66_read ? g_storm_b66_read(f, o, d, n) : 0; }  // 0x28450 (obj, off, dst, len) -> bytes read
void SeamFree1B960(void*, const char*, int32_t) {}               // 0xB960 SMemFree
void* SeamAlloc1C580(uint32_t n, const char*, int32_t, uint32_t) { return ::operator new(n); }  // 0xC580 SMemAlloc
void SeamNetFree4D60(void*) {}                                   // 0x4D60 list unlink

struct Cache {                // *(obj+0x110)
    uint8_t pad[0x120];
    uint32_t blockSize;       // +0x120
    uint8_t* data;            // +0x124 (wasm64 offset differs; field order is what matters)
};
struct Info { uint32_t pad[2]; uint32_t size; };   // *(obj+0x114), size at +8

struct File {                 // unaff_EBX of 0x28E80, retail dword indices in comments
    Cache* cache;             // [0x44]
    Info*  info;              // [0x45]
    uint32_t crc;             // [0x51]
    uint32_t crcOn;           // [0x53]
    uint32_t state;           // [0x54] 2 = hashing sequentially, 1 = broken
    uint32_t hashed;          // [0x55] bytes hashed
};

uint32_t Crc(uint32_t c, const uint8_t* p, uint32_t n) {
    for (; n; --n, ++p) {
        uint32_t e; std::memcpy(&e, g_crcTable + (((*p) ^ c) & 0xff) * 4, 4);
        c = (c >> 8) ^ e;
    }
    return c;
}

}  // namespace

// 1.13c Storm+0x28E80  (this = EBX; stack: offset, dst, len) -> bytes read
uint32_t Storm_sub_28E80(File* f, uint32_t off, uint8_t* dst, uint32_t len) {
    if (f->info->size <= off) return 0;
    if (f->info->size - off < len) len = f->info->size - off;
    if (f->crcOn && off == 0) { f->crc = 0xffffffffu; f->hashed = 0; f->state = 2; }
    uint32_t bs = f->cache->blockSize, head = 0, pos = off;
    auto seq = [&](uint32_t at) { if (f->crcOn && f->state != 1 && f->hashed != at) f->state = 1; };
    auto hash = [&](const uint8_t* p, uint32_t n) {
        if (f->crcOn && f->state == 2) { f->crc = Crc(f->crc, p, n); f->hashed += n; }
    };
    if (off & (bs - 1)) {                       // unaligned head, served from the block cache
        seq(off);
        uint32_t valid = SeamCacheFill28DF0(f, off & ~(bs - 1));
        uint32_t inb = off & (bs - 1);
        head = bs - inb < len ? bs - inb : len;
        std::memcpy(dst, f->cache->data + inb, head);
        hash(dst, head);
        dst += head; len -= head;
        if (valid != f->cache->blockSize || len == 0) {
            inb = (f->cache->blockSize - 1) & off;
            if (valid < inb) return 0;
            valid -= inb;
            return head < valid ? head : valid;
        }
        pos = off + head;
    }
    uint32_t mid = 0;
    if (len) {                                  // aligned middle, read directly
        seq(pos);
        uint32_t n = len & ~(bs - 1);
        mid = SeamRead28450(f, pos, dst, n);
        hash(dst, mid);
        dst += n; len -= n; pos += n;
        if (n != mid || len == 0) return head + mid;
    }
    uint32_t tail = 0;
    if (len) {                                  // unaligned tail, via the block cache
        seq(pos);
        uint32_t valid = SeamCacheFill28DF0(f, pos);
        std::memcpy(dst, f->cache->data, len);
        tail = len < valid ? len : valid;
        hash(dst, tail);
    }
    return mid + tail + head;
}

// Capture-probe runner: builds the port's File view from the retail-layout object (cache +0x110, info +0x114, crc +0x144, crcOn +0x14C, state +0x150,
// hashed +0x154; cache blockSize +0x120, data +0x124; info size +8), pins the CRC table, runs 0x28E80 and writes crc/state/hashed back.
extern "C" uint32_t storm_28e80_fnrun(uint8_t* rf, uint32_t off, uint8_t* dst, uint32_t len, const uint8_t* crcTable) {
    auto rd = [](const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    auto wr = [](uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); };
    const uint8_t* cx = (const uint8_t*)(uintptr_t)rd(rf + 0x110);
    const uint8_t* inf = (const uint8_t*)(uintptr_t)rd(rf + 0x114);
    Cache c{}; Info in{}; File f{};
    c.blockSize = rd(cx + 0x120); c.data = (uint8_t*)(uintptr_t)rd(cx + 0x124);
    in.size = rd(inf + 8);
    f.cache = &c; f.info = &in; f.crc = rd(rf + 0x144); f.crcOn = rd(rf + 0x14c); f.state = rd(rf + 0x150); f.hashed = rd(rf + 0x154);
    std::memcpy(g_crcTable, crcTable, sizeof g_crcTable);
    uint32_t r = Storm_sub_28E80(&f, off, dst, len);
    wr(rf + 0x144, f.crc); wr(rf + 0x150, f.state); wr(rf + 0x154, f.hashed);
    return r;
}

// Earlier draft (native Node layout, superseded by storm_361c0.cpp): the global hash-bucket table; g_buckets = 0x6FC45370, count 0x6FC45374
namespace { struct Node { void* a; void* b; void* c; Node* items; Node* next; }; }
Node** g_stormBuckets; uint32_t g_stormBucketCount, g_stormBucketUsed, g_stormBucketsInit;
static CRITICAL_SECTION g_sevtLock;                    // 0x6FC458C8 (SEVT table lock; shim no-op)
uint32_t Storm_sub_361C0(void (*releaseItem)(Node*)) {   // releaseItem = 0x35FC0 (SEVT item-group free; item = node->items)
    EnterCriticalSection(&g_sevtLock);
    for (uint32_t i = 0; i < g_stormBucketCount; ++i) {
        while (Node* n = g_stormBuckets[i]) {
            while (Node* it = n->items) { n->items = it->items; releaseItem(it); }  // item link at +0xC
            g_stormBuckets[i] = n->next;
            SeamFree1B960(n, "delete", -1);
        }
    }
    if (g_stormBuckets) SeamFree1B960(g_stormBuckets, "delete", -1);
    g_stormBuckets = nullptr; g_stormBucketCount = 0; g_stormBucketUsed = 0; g_stormBucketsInit = 1;
    LeaveCriticalSection(&g_sevtLock);
    return 1;
}

namespace {
// Win32 WriteFile seam: sequential sink for the BMP/PCX writers.
struct Sink { uint8_t* buf; size_t cap, len; };
void Put(Sink* s, const void* p, size_t n) {
    if (s->len + n <= s->cap) std::memcpy(s->buf + s->len, p, n);
    s->len += n;
}
}  // namespace
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);   // storm_alloc_1b960.cpp
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
extern "C" bool Storm_Sub31FF0(int32_t rows, uint8_t* dst, const uint8_t* src, int32_t dstSize, int32_t rowBytes, int32_t stride, int32_t* lastError);
namespace {
}  // namespace

// 1.13c Storm+0x32C00  PCX writer. in_EAX = height; stack: file, palette (256 x RGBX), pixels, width
uint32_t Storm_sub_32C00(int32_t height, Sink* file, const uint8_t* pal, const uint8_t* pixels, int32_t width) {
    uint8_t hdr[0x80] = {};
    uint16_t xmax = (uint16_t)(width - 1), ymax = (uint16_t)(height - 1), dpi = 0x48;
    hdr[0] = 0x0a; hdr[1] = 5; hdr[2] = 1; hdr[3] = 8;      // 0x50a, version 5, RLE, 8 bpp (dword0 = 0x0108050a)
    std::memcpy(hdr + 8, &xmax, 2); std::memcpy(hdr + 10, &ymax, 2);
    std::memcpy(hdr + 12, &dpi, 2); std::memcpy(hdr + 14, &dpi, 2);
    hdr[0x41] = 1;                                          // planes
    std::memcpy(hdr + 0x42, &width, 2);                     // bytes per line
    Put(file, hdr, sizeof hdr);
    auto* buf = (uint8_t*)Storm_Sub1C580((uint32_t)(width * height * 2), "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x4eb, 0);
    uint8_t* out = buf;
    const uint8_t* src = pixels;
    for (int32_t y = 0; y < height; ++y) Storm_Sub31F60((uint32_t)width, &src, &out);  // 1.13c 0x31F60 row RLE
    Put(file, buf, (size_t)(out - buf));
    Storm_Sub1B960(buf, "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x4f2);
    uint8_t tail[0x301]; tail[0] = 0x0c;
    for (int i = 0; i < 256; ++i) { tail[1 + i*3] = pal[i*4]; tail[2 + i*3] = pal[i*4+1]; tail[3 + i*3] = pal[i*4+2]; }
    Put(file, tail, sizeof tail);
    return 1;
}

// 1.13c Storm+0x32EC0  BMP writer. in_EAX = palette (256 x RGBX); EDI = width; stack: file, pixels, height
uint32_t Storm_sub_32EC0(const uint8_t* pal, int32_t width, Sink* file, const uint8_t* pixels, int32_t height) {
    uint8_t fh[14] = {'B', 'M'};
    uint32_t size = (uint32_t)width * (uint32_t)height + 0x436, off = 0x436;
    std::memcpy(fh + 2, &size, 4); std::memcpy(fh + 10, &off, 4);
    Put(file, fh, sizeof fh);
    uint32_t ih[10] = {0x28};                    // BITMAPINFOHEADER body is zeroed by retail's stack frame as written
    Put(file, ih, 0x28);
    uint8_t quad[0x400];
    for (int i = 0; i < 256; ++i) { quad[i*4+2] = pal[i*4]; quad[i*4+1] = pal[i*4+1]; quad[i*4] = pal[i*4+2]; quad[i*4+3] = 0; }
    Put(file, quad, sizeof quad);
    uint32_t pitch = (uint32_t)width; if (pitch & 3) pitch += 4 - (pitch & 3);
    uint32_t n = pitch * (uint32_t)height;
    auto* buf = (uint8_t*)Storm_Sub1C580(n, "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x294, 0);
    int32_t lastErr = 0;
    Storm_Sub31FF0(height, buf, pixels, (int32_t)n, (int32_t)pitch, width, &lastErr);   // 0x31FF0 bottom-up row copy
    Put(file, buf, n);
    Storm_Sub1B960(buf, "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x29c);
    return 1;
}

// FUNCTION-mode entries: the WriteFile sink is a caller buffer (payloads are not observable in the capture).
extern "C" uint32_t Storm_Sub32C00Probe(int32_t height, const uint8_t* pal, const uint8_t* pixels, int32_t width, uint8_t* sink, size_t cap) {
    Sink sk{sink, cap, 0};
    return Storm_sub_32C00(height, &sk, pal, pixels, width);
}
extern "C" uint32_t Storm_Sub32EC0Probe(const uint8_t* pal, int32_t width, const uint8_t* pixels, int32_t height, uint8_t* sink, size_t cap) {
    Sink sk{sink, cap, 0};
    return Storm_sub_32EC0(pal, width, &sk, pixels, height);
}

// 1.13c Storm+0x115B0 moved to storm_115b0.cpp (retail 4-byte node layout).
