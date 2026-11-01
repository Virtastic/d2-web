// 1.13c D2Game+0x51A90: ecx = a context block (its byte at +0x6d feeds 0x4DF60), edx = unit, stack a0 = skill id, a1 (ret 8). Validates the
// skill record (+0x54 and +0x82 words against the table limits), marks the unit (+0xC4 |= 0x40), asks 0xAC9A for the two values behind the
// record's +0x64 and +0x60 words, builds a 26-word request on the stack (ctx, unit, flag at +0xC set when a listed skill's +5 byte hits the
// mask byte at +4 of the global block, skill id, a1, 0x4DF60's result, up to five skill ids at +0x20.. with their 0xAC9A values at +0x38.., the
// +0x82 word at +0x4C) and hands it to 0xA0AD0 with the 0x50B70 callback.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_51a90(uint32_t ctx, uint32_t unit, uint32_t sid, uint32_t a1, uint32_t dt, uint32_t gv, const D2Seam* e)
{
    if ((int32_t)sid < 0 || (int32_t)sid >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = sid * 0x23C + d2rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t w54 = (int16_t)d2rd16(rec + 0x54);
    if (w54 < -1 || w54 >= (int32_t)d2rd32(dt + 0xBD4)) return 0;
    const int32_t w82 = (int16_t)d2rd16(rec + 0x82);
    if (w82 < 0 || w82 >= (int32_t)d2rd32(dt + 0xC4)) return 0;
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    const uint32_t res1 = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(rec + 0x64), sid, a1);
    const uint32_t res2 = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(rec + 0x60), sid, a1);
    const uint32_t flag = e->call(0x4DF60, (res2 & 0xFFFFFF00u) | d2rd8(ctx + 0x6D));
    uint32_t S[26] = {};
    S[0] = ctx; S[1] = unit; S[4] = sid; S[5] = a1; S[6] = flag; S[3] = 0;
    for (uint32_t i = 1; i < 6; i++) {
        const int32_t id = (int16_t)d2rd16(rec + 0x54 + 2 * i);
        if (id < 0 || id >= (int32_t)d2rd32(dt + 0xBD4)) { S[7 + i] = 0xFFFFFFFFu; break; }
        const uint32_t base = d2rd32(dt + 0xBCC);
        const uint32_t sk2 = base + (uint32_t)id * 0x144;
        if (!sk2) break;
        if (d2rd8(gv + 4) & d2rd8(sk2 + 5)) S[3] = 1;
        S[7 + i] = (uint32_t)id;
        S[13 + i] = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(rec + 0x68 + 4 * i), sid, a1);
    }
    S[19] = (uint32_t)w82;
    const uint32_t sp = (uint32_t)(uintptr_t)S;
    const uint32_t r = e->call(0xA0AD0, 0, ctx, 0, 0, 0, d2rd32(rec + 0x50), unit, res1, 0xB2050B70u, sp);
    e->blk(0xA0AD0, S, 26);
    return r;
}
