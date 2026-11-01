// 1.13c D2Game+0x50470: stack a0 (a context block: its byte +0x6D selects a 0xA7BA record), a1 (unit), a2 (callback word, ECX of the call), a3 (ret 0x10).
// Nothing happens unless the skill table holds more than 0x47 records; record 0x47 (the +0x9EA4 slot) is the one used. Builds the 26-word request
// on the stack: ctx, unit, 1, flag (set when a listed skill's +5 byte hits the mask byte at +4 of the global block), 0x47, a3, the 0xAC9A value for
// the record's +0x60 word (divided by the 0xA7BA record's +0x1C word when that is nonzero), up to six skill ids at word 7 (a negative or
// out-of-range id stores -1 and stops) with their 0xAC9A values at word 13, and the record's +0x82 word at word 19; then 0x50250(ECX = a2, EDX = request).
#include "d2game_seam4e.h"
extern "C" void d2game_50470(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t dt, uint32_t gv, const D2Seam* e)
{
    if ((int32_t)d2rd32(dt + 0xBA0) <= 0x47) return;
    const uint32_t rec = d2rd32(dt + 0xB98) + 0x9EA4;
    if (!rec) return;
    int32_t v = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, a1, d2rd32(rec + 0x60), 0x47, a3);
    const uint32_t p = e->call(0xA7BA, 0, 0, 0, 0, 0, d2rd8(a0 + 0x6D));
    if (p) { const int32_t dv = (int32_t)d2rd32(p + 0x1C); if (dv) v = v / dv; }
    uint32_t S[26] = {};
    S[0] = a0; S[1] = a1; S[2] = 1; S[4] = 0x47; S[5] = a3; S[6] = (uint32_t)v;
    uint32_t i = 0;
    for (; i < 6; i++) {
        const int32_t id = (int16_t)d2rd16(rec + 0x54 + 2 * i);
        if (id < 0 || id >= (int32_t)d2rd32(dt + 0xBD4)) { S[7 + i] = 0xFFFFFFFFu; break; }
        const uint32_t sk2 = d2rd32(dt + 0xBCC) + (uint32_t)id * 0x144;
        if (!sk2) break;
        if (d2rd8(gv + 4) & d2rd8(sk2 + 5)) S[3] = 1;
        S[7 + i] = (uint32_t)id;
        S[13 + i] = e->call(0xAC9A, 0, 0, 0, 0, 0, a1, d2rd32(rec + 0x68 + 4 * i), 0x47, a3);
    }
    S[19] = (uint32_t)(int32_t)(int16_t)d2rd16(rec + 0x82);
    e->call(0x50250, 0, a2, (uint32_t)(uintptr_t)S);
}
