// 1.13c D2Game+0xB7750 weighted pick of one of 16 entries (retail asm: ESI = the rolling unit, whose seed is the dwords at +0x20 / +0x24;
// stack, ret 0xC: w0 = game (byte +0x6D = difficulty), w1 = unit (only handed to D2Common), w2 = enable flag; returns the picked index, 1 when
// the running sum never passes the roll).
// Weights (a 16-dword stack local): 0 0 0 0 0 0 0 0 | 0x14 0x1E 0x96 10 10 0x46 0x28 0; entry 1 = (difficulty 0 ? 0x4B : 0x32) + 100 - #11027(esi);
// difficulty 0 also sets entry 13 to 0x2D; entry 10 becomes 200 when #11027(unit) < 0x21; #10494(unit, 0xB) clears entries 8, 11, 13; a clear
// flag clears entries 11, 12, 13. Roll: seed = seed * 0x6AC690C5 + (seed >> 32) (64-bit, low half stored at +0x20, high at +0x24), index = low
// half modulo the weight sum (an AND for a power of two); none when the sum is not positive (roll 0).
#include "d2game_kmem.h"

namespace d2g_b7750 {
struct Env {
    D2gMem m;
    uint32_t (*ord11027)(uint32_t unit);                       // 0xA718, 1 stack word
    uint32_t (*ord10494)(uint32_t unit, uint32_t state);       // 0xA68E, 2 stack words
};
inline uint32_t run(const Env& e, uint32_t game, uint32_t unit, uint32_t flag, uint32_t esi) {
    const D2gMem& m = e.m;
    int32_t t[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0x14, 0x1E, 0x96, 10, 10, 0x46, 0x28, 0 };
    uint32_t edi = 0x32;
    if (m.r8(game + 0x6D) == 0) { edi = 0x4B; t[13] = 0x2D; }
    if ((int32_t)e.ord11027(unit) < 0x21) t[10] = 0xC8;
    t[1] = (int32_t)(edi + (0x64u - e.ord11027(esi)));
    if (e.ord10494(unit, 0xB)) { t[8] = 0; t[13] = 0; t[11] = 0; }
    if (flag == 0) { t[11] = 0; t[13] = 0; t[12] = 0; }
    int32_t sum = 0;
    for (int i = 0; i < 16; i++) sum = (int32_t)((uint32_t)sum + (uint32_t)t[i]);
    uint32_t roll = 0;
    if (sum > 0) {
        uint64_t s = (uint64_t)m.r32(esi + 0x20) * 0x6AC690C5ull + m.r32(esi + 0x24);
        m.w32(esi + 0x24, (uint32_t)(s >> 32));
        m.w32(esi + 0x20, (uint32_t)s);
        uint32_t lo = (uint32_t)s, n = (uint32_t)sum;
        roll = (n & (n - 1)) == 0 ? (lo & (n - 1)) : lo % n;
    }
    int32_t acc = 0;
    for (uint32_t i = 0; i < 16; i++) {
        acc = (int32_t)((uint32_t)acc + (uint32_t)t[i]);
        if ((int32_t)roll < acc) return i;
    }
    return 1;
}
}
