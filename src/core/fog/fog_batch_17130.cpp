// src/core/fog/fog_batch_17130.cpp -- five 1.13c Fog.dll IFF header rows (Source/Fog/Src/IFF.cpp),
// from the cached Ghidra decompiles. Header: magic 'BOND' +0x00, count +0x04, then count * 12-byte
// entries from +0x08; capacity 0x400 bytes. Alloc/free are malloc/free seams (Fog+0x1B560/0x1B0C0).
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <emscripten.h>

namespace {
constexpr uint32_t kMagic = 0x444e4f42;
}

// 1.13c Fog+0x17260 -- validate a header at *cursor, advance cursor/remaining past it.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* fog_sub_17260(uint8_t** cursor, uint32_t* remaining) {  // 1.13c Fog+0x17260
    if (*remaining < 8) return nullptr;
    uint32_t* h = reinterpret_cast<uint32_t*>(*cursor);
    uint32_t size = h[1] * 0xc + 8;
    if (h[0] == kMagic && size < 0x401 && size <= *remaining) {
        *cursor = reinterpret_cast<uint8_t*>(h + h[1] * 3 + 2);
        *remaining -= size;
        return h;
    }
    return nullptr;
}

// 1.13c Fog+0x17140 -- allocate a 0x400 header block, stamp the magic.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* fog_sub_17140() {  // 1.13c Fog+0x17140
    uint32_t* h = static_cast<uint32_t*>(std::calloc(1, 0x400));
    *h = kMagic;
    return h;
}

// 1.13c Fog+0x17210 -- serialize header to dst; 0 ok, 0xE when remaining is too small.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_17210(const uint32_t* src, uint32_t* dst, uint32_t* written, int32_t* remaining) {  // 1.13c Fog+0x17210
    *written = 0;
    uint32_t size = src[1] * 0xc + 8;
    if (*remaining < static_cast<int32_t>(size)) return 0xe;
    *written = size;
    *remaining -= static_cast<int32_t>(size);
    std::memcpy(dst, src, size & ~3u);
    return 0;
}

// 1.13c Fog+0x17130 -- free a header block.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_17130(void* block) {  // 1.13c Fog+0x17130
    std::free(block);
}

// 1.13c Fog+0x171C0 -- append a 3-dword entry; 0 ok, 0xE when full.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_171C0(uint32_t a, uint32_t b, uint32_t c, uint32_t* hdr) {  // 1.13c Fog+0x171C0
    uint32_t n = hdr[1];
    if (n * 0xc + 0x14u > 0x400) return 0xe;
    hdr[n * 3 + 2] = a;
    hdr[n * 3 + 3] = b;
    hdr[n * 3 + 4] = c;
    hdr[1] = n + 1;
    return 0;
}
