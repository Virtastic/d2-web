// src/core/storm/storm_batch_b66.cpp -- Storm batch b66: transcribed from the cached 1.13c decompiles.
// Stage A lane (implementation + marker only). Retail layouts are 32-bit; here pointer-bearing
// objects are modelled as structs with the retail byte offsets in comments (wasm64 pointers).
// Register inputs (in_EAX/unaff_EBX/EDI/ECX) are explicit leading params, commented per function.
// Skipped: 0x14250 (1.9 KB state machine, 15 callees), 0x52D0 (SEH + eh_vector), 0x6A28/0x6D32/
// 0x7572/0x76FA (MSVC CRT, TLS/locale), 0x1CE50 (direct GDI).
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace {

// ---- seams for unmapped callees (provider/cache state is empty in ours) ---------------------
uint8_t g_crcTable[256 * 4];                                   // 0x6FC41600 CRC32 table (4 B/entry)
uint32_t SeamCacheFill28DF0(void*, uint32_t) { return 0; }       // 0x28DF0 (cache, aligned offset) -> bytes valid
uint32_t SeamRead28450(void*, uint32_t, void*, uint32_t) { return 0; }  // 0x28450 (obj, off, dst, len) -> bytes read
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
        uint32_t valid = SeamCacheFill28DF0(&f->cache->blockSize, off & ~(bs - 1));
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
        uint32_t valid = SeamCacheFill28DF0(&f->cache->blockSize, pos);
        std::memcpy(dst, f->cache->data, len);
        tail = len < valid ? len : valid;
        hash(dst, tail);
    }
    return mid + tail + head;
}

// 1.13c Storm+0x361C0  frees the global hash-bucket table; g_buckets = 0x6FC45370, count 0x6FC45374
namespace { struct Node { void* a; void* b; void* c; Node* items; Node* next; }; }
Node** g_stormBuckets; uint32_t g_stormBucketCount, g_stormBucketUsed, g_stormBucketsInit;
uint32_t Storm_sub_361C0(void (*releaseItem)(Node*)) {   // releaseItem = 0x25FC0 (item = node->items)
    for (uint32_t i = 0; i < g_stormBucketCount; ++i) {
        while (Node* n = g_stormBuckets[i]) {
            while (Node* it = n->items) { n->items = it->items; releaseItem(it); }  // item link at +0xC
            g_stormBuckets[i] = n->next;
            SeamFree1B960(n, "delete", -1);
        }
    }
    if (g_stormBuckets) SeamFree1B960(g_stormBuckets, "delete", -1);
    g_stormBuckets = nullptr; g_stormBucketCount = 0; g_stormBucketUsed = 0; g_stormBucketsInit = 1;
    return 1;
}

namespace {
// Win32 WriteFile seam: sequential sink for the BMP/PCX writers.
struct Sink { uint8_t* buf; size_t cap, len; };
void Put(Sink* s, const void* p, size_t n) {
    if (s->len + n <= s->cap) std::memcpy(s->buf + s->len, p, n);
    s->len += n;
}
void Rows21F60(uint8_t*&, const uint8_t*, int32_t) {}    // 0x21F60 PCX row RLE (unmapped)
void Rows21FF0(uint8_t* dst, const uint8_t* src, uint32_t, uint32_t pitch, uint32_t w) {  // 0x21FF0 row copy
    (void)dst; (void)src; (void)pitch; (void)w;
}
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
    auto* buf = (uint8_t*)SeamAlloc1C580((uint32_t)(width * height * 2), "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x4eb, 0);
    uint8_t* out = buf;
    for (int32_t y = 0; y < height; ++y) Rows21F60(out, pixels, width);
    Put(file, buf, (size_t)(out - buf));
    SeamFree1B960(buf, "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x4f2);
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
    auto* buf = (uint8_t*)SeamAlloc1C580(n, "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x294, 0);
    Rows21FF0(buf, pixels, n, pitch, (uint32_t)width);
    Put(file, buf, n);
    SeamFree1B960(buf, "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP", 0x29c);
    return 1;
}

namespace { struct Msg { void* link; int32_t pad; void* data; void* aux; int32_t type; int32_t owned; }; }
// 1.13c Storm+0x115B0  drains a queue. unaff_EBX = queue (head at +8); provider callback table = 0x6FC45834
void Storm_sub_115B0(Msg** queueHead, void (*providerFree)(void*, void*, int32_t), void (*unlink)(Msg*)) {
    while (Msg* m = *queueHead) {
        if (!m->owned) { if (providerFree) providerFree(m->data, m->aux, m->type); }
        else {
            if (m->data) SeamFree1B960(m->data, "delete", -1);
            if (m->aux) SeamFree1B960(m->aux, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x3b6);
        }
        unlink(m); SeamNetFree4D60(m);
        SeamFree1B960(m, "AUMESSAGE", -2);
    }
}
