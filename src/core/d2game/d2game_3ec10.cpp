// 1.13c D2Game+0x3EC10: ecx = pGame, edx = missile, [esp+4] = target unit, ret 4. skill = #B26A(m), level = #B0FC(m); the skill record (stride 0x23C, count dt+0xBA0)
// must exist, the missile have flag bit 10 and its owner (#E03A0(eax = m[0x94], edx = m[0x98], ecx = game)) resolve, else 1; a null target returns 0. The record's
// int16 [+0x82] must be in 0..dt[0xC4). dur = #AC9A(owner, row[0x144], skill, level) clamped to >= 5; ex = #A5D4(target, short). When ex is 0 a unit is made with
// #A6A0(game[0x1C], 2, game[0xA8] + dur, owner[0], owner[0xC]) (null returns 1) and gets #A5B6, #A75A(.., fixed table), #A60A(target, nu, 1), #A3CA(target, short, 1);
// ex stays 0 on that path (the retail code reloads the first result). Then #448E0(eax = row; target, ex, skill, level), #A706(target, short, 1),
// #A7C6(ex, game[0xA8] + dur), #8EA80(ecx = game, edx = target; 0xC, game[0xA8] + dur, 0, 0, 0). Returns 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_3ec10(uint32_t game, uint32_t m, uint32_t target, uint32_t dt, const D2Seam* e)
{
    const int32_t skill = (int32_t)e->call(0xB26A, 0, 0, 0, 0, 0, m);
    const uint32_t level = e->call(0xB0FC, 0, 0, 0, 0, 0, m);
    if (skill < 0 || skill >= (int32_t)d2rd32(dt + 0xBA0)) return 1;
    const uint32_t row = skill * 0x23C + d2rd32(dt + 0xB98);
    if (!row || !m) return 1;
    if (!(d2rd32(m + 0xC8) >> 10 & 1)) return 1;
    const uint32_t owner = e->call(0xE03A0, d2rd32(m + 0x94), game, d2rd32(m + 0x98));
    if (!owner) return 1;
    if (!target) return 0;
    const int16_t sh = (int16_t)d2rd16(row + 0x82);
    if (sh < 0 || (int32_t)sh >= (int32_t)d2rd32(dt + 0xC4)) return 1;
    int32_t dur = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, owner, d2rd32(row + 0x144), (uint32_t)skill, level);
    if (dur < 5) dur = 5;
    const uint32_t ex = e->call(0xA5D4, 0, 0, 0, 0, 0, target, (uint32_t)(int32_t)sh);
    if (!ex) {
        const uint32_t nu = e->call(0xA6A0, 0, 0, 0, 0, 0, d2rd32(game + 0x1C), 2, d2rd32(game + 0xA8) + (uint32_t)dur, d2rd32(owner), d2rd32(owner + 0xC));
        if (!nu) return 1;
        e->call(0xA5B6, 0, 0, 0, 0, 0, nu, (uint32_t)(int32_t)sh);
        e->call(0xA75A, 0, 0, 0, 0, 0, nu, 0xB2000000u | 0x9F7A0u);
        e->call(0xA60A, 0, 0, 0, 0, 0, target, nu, 1);
        e->call(0xA3CA, 0, 0, 0, 0, 0, target, (uint32_t)(int32_t)sh, 1);
    }
    e->call(0x448E0, row, 0, 0, 0, 0, target, ex, (uint32_t)skill, level);
    e->call(0xA706, 0, 0, 0, 0, 0, target, (uint32_t)(int32_t)sh, 1);
    e->call(0xA7C6, 0, 0, 0, 0, 0, ex, d2rd32(game + 0xA8) + (uint32_t)dur);
    e->call(0x8EA80, 0, game, target, 0, 0, 0xC, d2rd32(game + 0xA8) + (uint32_t)dur, 0, 0, 0);
    return 0;
}
