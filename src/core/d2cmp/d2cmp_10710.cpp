// src/core/d2cmp/d2cmp_10710.cpp -- 1.13c D2CMP.dll sub_10710 transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2cmp/decomp/0x10710.c). Alpha batch lane: no vector yet.
// Prior art: none (D2MOO's D2CMP is stubs). Callees 0xFD70 and Fog #10042/#10043 are seams.
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2CMP10710Env {
    uint8_t (*colorMap)(uint8_t r, uint8_t g, uint8_t b);                        // header +0x00 callback
    void* (*alloc)(uint32_t n, uint32_t line);                                    // Fog #10042
    void (*release)(void* p, uint32_t line);                                      // Fog #10043
    void (*fd70)(uint8_t* idx, uint32_t tileDword2, void* tileDword0);            // 0xFD70
};

// 1.13c D2CMP+0x10710 (469 bytes): flags(+4)&4 clear is fatal. For each of w(+0x10)*h(+0x14) tiles,
// map its RGB triples (tile+0x20, tile[1]*tile[2] of them) to one index byte each (triples equal to the
// transparent colour give 0, a zero from the callback is fatal), run 0xFD70, then build a 0x20-byte
// header + index copy and store it in out[i]. Retail's copy source is the cursor AFTER the index run
// (end of the buffer), reproduced as-is.
// 1.13c D2CMP+0x10710
extern "C"
void d2cmp_10710(uint32_t flags, uint32_t w, uint32_t h, uint8_t** tiles, uint8_t** out,
                 uint32_t transparentRgb, const D2CMP10710Env* env) {
    if ((flags & 4) == 0) _Exit(-1);
    for (uint32_t i = 0; i < w * h; ++i) {
        uint8_t* t = tiles[i];
        uint32_t n; { uint32_t a, b; std::memcpy(&a, t + 4, 4); std::memcpy(&b, t + 8, 4); n = a * b; }
        uint8_t* idx = static_cast<uint8_t*>(env->alloc(0xd4, 0));
        const uint8_t* px = t + 0x20;
        uint8_t* cur = idx;
        uint32_t cnt = 0;
        for (; cnt < n; ++cnt, ++cur, px += 3) {
            if (px[0] == uint8_t(transparentRgb) && px[1] == uint8_t(transparentRgb >> 8) &&
                px[2] == uint8_t(transparentRgb >> 16)) {
                *cur = 0;
            } else {
                uint8_t c = env->colorMap(px[0], px[1], px[2]);
                if (c == 0) _Exit(-1);
                *cur = c;
            }
        }
        uint32_t d2; std::memcpy(&d2, t + 8, 4);
        env->fd70(idx, d2, *reinterpret_cast<void**>(t));
        env->release(nullptr, 0xf0);
        uint8_t* rec = static_cast<uint8_t*>(env->alloc(0xf2, 0));
        std::memcpy(rec, t, 0x14);                 // dwords 0..4
        std::memset(rec + 0x14, 0, 8);
        std::memcpy(rec + 0x1c, &cnt, 4);
        std::memcpy(rec + 0x20, cur, cnt);         // cursor = idx + cnt, as retail
        env->release(nullptr, 0xfd);
        out[i] = rec;
    }
}
