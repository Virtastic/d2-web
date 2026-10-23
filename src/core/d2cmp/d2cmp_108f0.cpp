// src/core/d2cmp/d2cmp_108f0.cpp -- 1.13c D2CMP.dll+0x108F0 (sub_108F0), transcribed from the cached
// decompile and checked against the disassembly (the decompile drops FD70's stack args).
// Tile-cache builder: for each node of the list at ctx+0x34 (next at +4), RLE-compress its pixels
// (sub_FD70, seam), copy node header + compressed bytes into a fresh Fog block, free the scratch,
// and store the block in out->slot[tbl[bit(ctx[0x28])*64 + idx] * perRow + i % perRow].
#include <cstdint>
#include <cstring>
#include <cstdlib>

// Seams: Fog #10042 alloc(line,zero; size in ECX, file in EDX), #10043 free, #10265+#10024 fatal.
extern "C" __attribute__((weak)) void* d2cmp_fog_alloc(uint32_t size, const char* file, uint32_t line, uint32_t zero);
extern "C" __attribute__((weak)) void d2cmp_fog_free(void* p, const char* file, uint32_t line, uint32_t zero);
extern "C" __attribute__((weak)) void d2cmp_fog_fatal(uint32_t msg);   // 10265(msg) + 10024, then _exit(-1)
// sub_FD70: (eax=node[0xc], stack: pixels=node[0x20], stride=node[0x10], rows=node[8], &data, &size).
extern "C" __attribute__((weak)) void d2cmp_fd70(uint32_t eax, uint32_t pixels, uint32_t stride,
                                                 uint32_t rows, uint8_t** data, uint32_t* size);
extern "C" __attribute__((weak)) const uint32_t d2cmp_tbl_6fe2e4d8[];   // 0x6FE2E4D8, rows of 64 dwords
extern "C" const char d2cmp_src_6fe2fe88[];

// 1.13c D2CMP+0x108F0
extern "C"
uint32_t d2cmp_108f0(uint8_t* ctx, uint8_t* out) {
    auto rd = [](const uint8_t* b, int o) { uint32_t v; memcpy(&v, b + o, 4); return v; };
    if (ctx[0x18] & 4) { d2cmp_fog_fatal(0x72); exit(-1); }
    uint32_t per = rd(ctx, 0x24) / rd(ctx, 0x28);
    uint32_t i = 0, last = 0;
    int32_t first = (int32_t)rd(ctx, 0x34);
    last = (uint32_t)first;
    uint32_t n = (first < 1 ? 0u : (uint32_t)first);   // ((int)x<1)-1 & x
    while ((int32_t)n > 0) {
        uint8_t* nd = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(n));
        uint8_t* data; uint32_t size;
        d2cmp_fd70(rd(nd, 0xc), rd(nd, 0x20), rd(nd, 0x10), rd(nd, 8), &data, &size);
        uint8_t* b = (uint8_t*)d2cmp_fog_alloc(size + 0x23, d2cmp_src_6fe2fe88, 0x82, 0);
        uint32_t h[8] = { rd(nd, 8), rd(nd, 0xc), rd(nd, 0x10), rd(nd, 0x18), rd(nd, 0x1c), 0, 0, size };
        memcpy(b, h, 32);
        memcpy(b + 0x20, data, size);
        d2cmp_fog_free(data, d2cmp_src_6fe2fe88, 0x8d, 0);
        uint32_t m = rd(ctx, 0x28);
        uint32_t k = m < 2 ? 1 : ((i / per) << 6) / m;
        uint32_t bit = m ? (uint32_t)__builtin_ctz(m) + 1 : 0;
        last = i / per;
        uint32_t idx = d2cmp_tbl_6fe2e4d8[bit * 0x40 + k] * per + i % per;
        uint32_t p32 = (uint32_t)(uintptr_t)b;
        memcpy(out + 0x18 + idx * 4, &p32, 4);
        i++;
        n = rd(nd, 4);
    }
    return last;
}
