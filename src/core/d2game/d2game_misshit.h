// 1.13c D2Game: the pieces the missile hit callbacks share. dt is the data table block (the dword behind the
// imported table pointer). The missile record of a unit is [dt+0xB64] + class * 0x1A4 for a class (unit +4)
// inside 0 .. [dt+0xB6C]; the skill record is [dt+0xB98] + id * 0x23C for an id inside 0 .. [dt+0xBA0]. A
// missile whose flag word (+0xC8) has bit 10 names its owner at +0x94 (type) and +0x98 (id), looked up by
// 0xE03A0 (eax = type, ecx = game, edx = id). The area hit 0xA0C70 takes eax = game and eight stack words:
// owner, x, y, range, filter flags, the per-unit callback, its argument block and a zero.
#pragma once
#include <cstdint>
#include "d2game_unitxy.h"

template <class E> uint32_t D2GameMissRec(E& e, uint32_t dt, uint32_t unit)
{
    if (!unit) return 0;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    return e.rd32(dt + 0xB64) + (uint32_t)id * 0x1A4;
}
template <class E> uint32_t D2GameSkillRec(E& e, uint32_t dt, uint32_t skill)
{
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    return e.rd32(dt + 0xB98) + skill * 0x23C;
}
template <class E> bool D2GameMissOwned(E& e, uint32_t unit) { return (e.rd32(unit + 0xC8) >> 10) & 1; }
template <class E> uint32_t D2GameMissOwner(E& e, uint32_t game, uint32_t unit)
{
    return e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
}
// blk[0] |= record +0xA8; the low word of blk[1] |= the record's word at +0xAC
template <class E> void D2GameMissHitFlags(E& e, uint32_t rec, uint32_t* blk)
{
    blk[0] |= e.rd32(rec + 0xA8);
    blk[1] |= e.rd32(rec + 0xAC) & 0xFFFF;
}
template <class E> void D2GameAreaHit(E& e, uint32_t game, uint32_t owner, uint32_t x, uint32_t y, uint32_t range,
                                      uint32_t flags, uint32_t cb, const uint32_t* blk, uint32_t nblk)
{
    const uint32_t w[4] = { flags, 0xB2000000u | cb, e.stk(blk), 0 };
    e.h(0xA0C70, game, 0, 0, 0, 0, owner, x, y, range, w[0], w[1], w[2], w[3]);
    e.x(0xA0C70, w, 4);
    e.blk(0xA0C70, blk, nblk);
}
