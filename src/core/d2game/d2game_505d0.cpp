// D2Game+0x505D0 -- SkillNec_ApplySkillEffect (build the effect request for skill `sk` and hand it to 0xA0AD0): ECX game, EDX unit, stack skill id and level (ret 8).
// Skill row (dt +0xB98, stride 0x23C): sk outside 0..dt +0xBA0, a null row, word +0x54 below -1 or at/over dt +0xBD4, or word +0x82 negative or at/over dt +0xC4: 0.
// Else unit +0xC4 |= 0x40 (unit non-null); flag = word +0x82 is 0x17 or 0x38 (0x4DF80, pure). r1 = 0xAC9A(unit, row +0x64, sk, lvl), v = 0xAC9A(unit, row +0x60, sk, lvl); with flag
// v = v / [0xA7BA(game byte +0x6D) +0x1C] (signed, when the record and the divisor are non-zero: 0x4DF60). A 26-dword block {game, unit, flag, hit, sk, lvl, v, six stat ids, six values, then the
// sign-extended words +0x82 +0x84 +0x86 +0x88 +0x8A +0x8C +0x8E} where for i < 6 the word +0x54+2i: negative or at/over dt +0xBD4 stores -1 and ends the loop; 0x10D0(EAX = word) null ends it;
// else hit = 1 when [D2Game+0xF8BE0] byte +4 & record byte +5, id[i] = word, value[i] = 0xAC9A(unit, row +0x68+4i, sk, lvl). Returns 0xA0AD0(ECX = game; row +0x50, unit, r1, D2Game+0x50250, block).
#include "d2game_seam4e.h"
// 1.13c D2Game+0x505D0
extern "C" uint32_t d2game_505d0(uint32_t game, uint32_t unit, int32_t sk, uint32_t lvl, uint32_t dt, uint32_t gb, const D2Seam* e)
{
    if (sk < 0 || sk >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = d2rd32(dt + 0xB98) + (uint32_t)sk * 0x23C;
    if (!row) return 0;
    const int16_t w54 = (int16_t)d2rd16(row + 0x54), w82 = (int16_t)d2rd16(row + 0x82);
    if (w54 < -1 || (int32_t)w54 >= (int32_t)d2rd32(dt + 0xBD4)) return 0;
    if (w82 < 0 || (int32_t)w82 >= (int32_t)d2rd32(dt + 0xC4)) return 0;
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    const uint32_t flag = (w82 == 0x17 || w82 == 0x38) ? 1 : 0;
    const uint32_t r1 = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x64), (uint32_t)sk, lvl);
    uint32_t v = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x60), (uint32_t)sk, lvl);
    if (flag) {
        const uint32_t p = e->call(0xA7BA, 0, 0, 0, 0, 0, d2rd8(game + 0x6D));
        if (p && (int32_t)d2rd32(p + 0x1C)) v = (uint32_t)(int32_t)((int64_t)(int32_t)v / (int32_t)d2rd32(p + 0x1C));
    }
    uint32_t q[26] = {};
    q[0] = game; q[1] = unit; q[2] = flag; q[4] = (uint32_t)sk; q[5] = lvl; q[6] = v;
    for (int i = 0; i < 6; i++) {
        const int16_t w = (int16_t)d2rd16(row + 0x54 + 2 * i);
        if (w < 0 || (int32_t)w >= (int32_t)d2rd32(dt + 0xBD4)) { q[7 + i] = 0xFFFFFFFFu; break; }
        const uint32_t p = e->call(0x10D0, (uint32_t)(int32_t)w);
        if (!p) break;
        if (d2rd8(gb + 4) & d2rd8(p + 5)) q[3] = 1;
        q[7 + i] = (uint32_t)(int32_t)w;
        q[13 + i] = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x68 + 4 * i), (uint32_t)sk, lvl);
    }
    q[19] = (uint32_t)(int32_t)w82;
    q[20] = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x84); q[21] = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x86);
    q[22] = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x88); q[23] = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x8A);
    q[24] = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x8C); q[25] = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x8E);
    const uint32_t ret = e->call(0xA0AD0, 0, game, 0, 0, 0, d2rd32(row + 0x50), unit, r1, 0xB2000000u | 0x50250u, (uint32_t)(uintptr_t)q);
    e->blk(0xA0AD0, q, 26);
    return ret;
}
