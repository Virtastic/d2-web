// src/core/d2cmp/d2cmp_108f0.cpp -- 1.13c D2CMP.dll+0x108F0 (sub_108F0), transcribed from the disassembly (the decompile drops FD70's stack
// arguments and reads the pixel pointer as an address).
// Retail convention: fastcall, ECX = tile-cache context, EDX = output header, plain ret; the result is the last row quotient (EAX at exit).
// Tile-cache builder: for each node of the list at ctx+0x34 (next at node+4), RLE-compress its pixel run (0xFD70, seam), copy the node header + compressed
// bytes into a fresh Fog block, free the compressor's buffer, and store the block at out+0x18+4*(tbl[row*64+k] * perRow + i % perRow).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2CMP108F0Env {
    void* (*alloc)(uint32_t n, uint32_t line);                       // Fog #10042 (ECX = size, file EDX, line, 0)
    void (*release)(void* p, uint32_t line);                         // Fog #10043 (ECX = block, file EDX, line, 0)
    // 0xFD70: EAX = node[0xc], stack (pixels = node[0x20], node[0x10], node[8], &data, &size)
    void (*fd70)(uint32_t width, uint32_t pixels, uint32_t node10, uint32_t node8, uint8_t** data, uint32_t* size);
    const uint32_t* tbl;                                             // the module table at RVA 0x1E4D8, rows of 64 dwords
};

// 1.13c D2CMP+0x108F0
extern "C"
uint32_t d2cmp_108f0(uint8_t* ctx, uint8_t* out, const D2CMP108F0Env* env) {
    auto rd = [](const uint8_t* b, int o) { uint32_t v; memcpy(&v, b + o, 4); return v; };
    if (ctx[0x18] & 4) _Exit(-1);
    const uint32_t per = rd(ctx, 0x24) / rd(ctx, 0x28);
    uint32_t i = 0;
    int32_t first = (int32_t)rd(ctx, 0x34);
    uint32_t last = (uint32_t)first;
    uint32_t n = first < 1 ? 0u : (uint32_t)first;
    while ((int32_t)n > 0) {
        uint8_t* nd = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(n));
        uint8_t* data = nullptr; uint32_t size = 0;
        env->fd70(rd(nd, 0xc), rd(nd, 0x20), rd(nd, 0x10), rd(nd, 8), &data, &size);
        uint8_t* b = (uint8_t*)env->alloc(size + 0x23, 0x82);
        const uint32_t h[8] = { rd(nd, 8), rd(nd, 0xc), rd(nd, 0x10), rd(nd, 0x18), rd(nd, 0x1c), 0, 0, size };
        memcpy(b, h, 32);
        memcpy(b + 0x20, data, size);
        env->release(data, 0x8d);
        const uint32_t m = rd(ctx, 0x28);
        const uint32_t k = m < 2 ? 1 : ((i / per) << 6) / m;
        const uint32_t bit = m ? (uint32_t)__builtin_ctz(m) + 1 : 0;
        last = i / per;
        const uint32_t idx = env->tbl[bit * 0x40 + k] * per + i % per;
        const uint32_t p32 = (uint32_t)(uintptr_t)b;
        memcpy(out + 0x18 + idx * 4, &p32, 4);
        i++;
        n = rd(nd, 4);
    }
    return last;
}
