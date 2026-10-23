// src/core/d2cmp/d2cmp_sub_12480.cpp -- 1.13c D2CMP.dll 0x12480, from the cached Ghidra decompile.
#include <cstdint>

// Seams: blit = D2CMP+0xFEC0; fatal = Fog 10265(id) + 10024 + _exit(-1).
struct D2CMP12480Env {
    void (*blit)(uint8_t* dst, uint32_t stride, int32_t a, int32_t b, int32_t c, int32_t skip,
                 uint32_t rows, int32_t d, uint8_t e);
    void (*fatal)(uint32_t msgId);
};

// 1.13c D2CMP+0x12480
extern "C"
void d2cmp_sub_12480(const D2CMP12480Env* env, const uint8_t* p1, int32_t p2, int32_t p3, int32_t p4,
                     int32_t p5, uint8_t* p6, int32_t p7, uint32_t p8, int32_t p9, uint8_t p10) {
    auto rd = [](const uint8_t* b, int o) { return *(const int32_t*)(b + o); };
    const uint8_t* s = *(const uint8_t* const*)(p1 + 0x34);
    if (s) {
        if (rd(s, 0) != 6) { env->fatal(0x50d); return; }
        if (s[4] & 4) { env->fatal(0x50e); return; }
    }
    if (!p6) { env->fatal(0x511); return; }
    const uint8_t* q = *(const uint8_t* const*)(p1 + 0x3c);
    if (!q) { env->fatal(0x514); return; }
    int32_t h = rd(q, 0x10) / 2;
    int32_t y = p3 + h;
    uint32_t n = (uint32_t)rd(q, 8) >> 1;
    int32_t skip = 0;
    if (y - rd(q, 8) / 2 < 0) n = (uint32_t)(y + 1);
    if ((int32_t)n > 0) {
        if (p7 <= y) {
            skip = y - p7 + 1;
            n -= (uint32_t)skip;
            if ((int32_t)n < 1) return;
            y = p7 - 1;
        }
        int32_t x = (p2 + rd(q, 0xc) + h) - skip;
        env->blit(p6 + (int32_t)((uint32_t)y * p8) + x, p8, x, p4, p5, skip, n, p9, p10);
    }
}
