// 1.13c D2Client+0x18370: unit = stack arg (ret 4). Walks the four (row, chance, multiplier) shorts of the unit's descriptor at +0xCC; each entry that passes its percent roll (unit seed at +0x6C) asks 0x181A0 to place a random count of that row
#include "cl17.h"
static uint32_t rnd18370(uint32_t sp, uint32_t n, const cl::Env& e) {
    uint64_t p = (uint64_t)e.R(sp) * 0x6ac690c5u + e.R(sp + 4);
    uint32_t lo = (uint32_t)p, hi = (uint32_t)(p >> 32);
    e.W(sp, lo);
    e.W(sp + 4, hi);
    return (n & (n - 1)) ? lo % n : lo & (n - 1);
}
extern "C" uint32_t d2c_18370(uint32_t unit, const cl::Env& e) {
    uint32_t a = e.H(0xc1de, unit);
    if (!a) return 0;
    uint32_t b = e.H(0xc214, a), sp = unit ? unit + 0x6c : 0, ebp = b + 0xcc, ret = b;
    for (int32_t i = 0; i < 4; ++i, ebp += 2) {
        int32_t row = (int16_t)e.R(ebp);
        ret = (uint32_t)i;
        if (row < 0) return ret ? ret : b;
        ret = (uint32_t)i + 1;
        // percent roll: unsigned compare against the sign-extended chance
        uint64_t p = (uint64_t)e.R(sp) * 0x6ac690c5u + e.R(sp + 4);
        e.W(sp, (uint32_t)p);
        e.W(sp + 4, (uint32_t)(p >> 32));
        if ((uint32_t)p % 100 >= (uint32_t)(int32_t)(int16_t)(e.R(ebp + 8))) continue;
        uint32_t r = (row < (int32_t)e.R(e.dt + 0xa80)) ? e.R(e.dt + 0xa78) + (uint32_t)row * 0x1a8 : 0;
        int32_t n = (int32_t)e.R8(r + 0x30) - (int32_t)e.R8(r + 0x2f);
        uint32_t cnt = (n > 0 ? rnd18370(sp, (uint32_t)n, e) : 0) + e.R8(r + 0x2f);
        int16_t m = (int16_t)e.R(ebp + 0x10);
        int32_t c = m ? (int32_t)cnt * m : (int32_t)cnt;
        for (; c > 0; --c) e.H(0x181a0, unit, (uint32_t)row, 0, 0, 0, 0, 0, 0, 0, sp);
    }
    return 4;
}
