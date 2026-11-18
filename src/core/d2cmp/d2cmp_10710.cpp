// src/core/d2cmp/d2cmp_10710.cpp -- 1.13c D2CMP.dll sub_10710 transcribed from the disassembly (the decompile reads the
// first stack word as the header and drops FD70's stack arguments).
// Retail convention: fastcall, ECX = tile-library header, EDX = output header, stack: colour callback (stdcall, 3 words), transparent RGB; ret 8.
// Callees 0xFD70 and Fog #10042/#10043 are seams.
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2CMP10710Env {
    uint8_t (*colorMap)(uint32_t r, uint32_t g, uint32_t b);                      // the stack callback; r carries the caller's EAX residue above its low byte
    void* (*alloc)(uint32_t n, uint32_t line);                                    // Fog #10042 (ECX = size, file EDX, line, 0)
    void (*release)(void* p, uint32_t line);                                      // Fog #10043 (ECX = block, file EDX, line, 0)
    void (*fd70)(uint8_t* idx, uint32_t height, uint32_t flags, uint32_t width, uint8_t** data, uint32_t* size);   // 0xFD70 (EAX = width)
};

// 1.13c D2CMP+0x10710 (469 bytes): header byte +4 bit 2 clear is fatal. For each of w(+0x10)*h(+0x14) tiles (pointers from +0x18),
// map its RGB triples (tile+0x20, tile[1]*tile[2] of them) to one index byte each (triples equal to the transparent colour give 0, a
// zero from the callback is fatal), run 0xFD70 over the index run, then build a record (tile dwords 0..4, two zero dwords, FD70's size,
// FD70's output bytes) and store it at out+0x18+4*i. Retail copies FD70's output buffer, then frees it (line 0xfd).
// 1.13c D2CMP+0x10710
extern "C"
void d2cmp_10710(const uint8_t* hdr, uint8_t* out, uint32_t transparentRgb, const D2CMP10710Env* env) {
    auto rd = [](const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; };
    if ((hdr[4] & 4) == 0) _Exit(-1);
    uint32_t resid = 0;   // EAX above the low byte at the callback: the allocation's address on the first pixel, then the pixel counter
    for (uint32_t i = 0; i < rd(hdr, 0x10) * rd(hdr, 0x14); ++i) {
        const uint8_t* t = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd(hdr, 0x18 + 4 * i)));
        const uint32_t n = rd(t, 8) * rd(t, 4);
        uint8_t* idx = static_cast<uint8_t*>(env->alloc(n, 0xd4));
        resid = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(idx));
        const uint8_t* px = t + 0x20;
        uint8_t* cur = idx;
        for (uint32_t cnt = 0; cnt < n; ++cnt, ++cur, px += 3) {
            if (px[0] == uint8_t(transparentRgb) && px[1] == uint8_t(transparentRgb >> 8) && px[2] == uint8_t(transparentRgb >> 16)) {
                *cur = 0;
            } else {
                const uint8_t c = env->colorMap((resid & ~0xffu) | px[0], px[1], px[2]);
                if (c == 0) _Exit(-1);
                *cur = c;
            }
            resid = cnt + 1;
        }
        uint8_t* data = nullptr; uint32_t size = 0;
        env->fd70(idx, rd(t, 8), rd(t, 0), rd(t, 4), &data, &size);
        env->release(idx, 0xf0);
        uint8_t* rec = static_cast<uint8_t*>(env->alloc(size + 0x23, 0xf2));
        std::memcpy(rec, t, 0x14);                 // dwords 0..4
        std::memset(rec + 0x14, 0, 8);
        std::memcpy(rec + 0x1c, &size, 4);
        std::memcpy(rec + 0x20, data, size);
        env->release(data, 0xfd);
        const uint32_t p = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(rec));
        std::memcpy(out + 0x18 + 4 * i, &p, 4);
    }
}
