// 1.13c D2Game+0x40280 -- monster-mode periodic area-damage callback: skill id 0xB26A(unit) selects a skill row (dt +0xB98, 0x23C stride, count dt +0xBA0); owner 0xE03A0 when
// unit flag bit 10 (else 0). When 0xB28E(unit) is a multiple of the period (monster row +0x38, else 0xAC9A(owner, skill +0x144, skill, 0xB0FC level), at least 1) a damage
// block is filled by 0x3A4E0(0, &block), block +0 |= row +0xA8, block word +4 low half |= row +0xAC, and 0xA0C70 (EAX = game; unit, 0, 0, duration, frames (skill +0x50 or
// 0x8583), callback 0xC1210, &block, 0) runs with duration = row +0x3C else 0xAC9A(owner, skill +0x64, skill, level), at least 1. Returns 0x3F320(game) with EBX = unit; 2 when
// the unit, monster row or skill row is missing.
// fastcall: ECX = game, EDX = unit
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f40280(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return 2;
    const uint32_t mrow = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!mrow) return 2;
    const int32_t sid = (int32_t)e.imp(0xB26A, unit);
    if (sid < 0 || sid >= (int32_t)e.rd32(dt + 0xba0)) return 2;
    const uint32_t srow = (uint32_t)sid * 0x23cu + e.rd32(dt + 0xb98);
    if (!srow) return 2;
    const uint32_t owner = (e.rd32(unit + 0xc8) >> 10 & 1) ? e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98)) : 0;
    const uint32_t lvl = e.imp(0xB0FC, unit);
    const int32_t m = (int32_t)e.imp(0xB28E, unit);
    int32_t per = (int32_t)e.rd32(mrow + 0x38);
    if (per <= 0) {
        per = (int32_t)e.imp(0xAC9A, owner, e.rd32(srow + 0x144), (uint32_t)sid, lvl);
        if (per < 1) per = 1;
    }
    if (m % per == 0) {
        uint32_t blk[28] = {};
        e.h(0x3A4E0, 0, 0, 0, 0, 0, 0, e.stk(blk));
        { const uint32_t key = (2u << 24) | 0x3A4E0; uint32_t off = 0; for (const uint32_t* p = e.sc; p[0]; p += 2) if (p[0] == key) blk[off++] = p[1]; }
        int32_t dur = (int32_t)e.rd32(mrow + 0x3c);
        if (dur <= 0) {
            dur = (int32_t)e.imp(0xAC9A, owner, e.rd32(srow + 0x64), (uint32_t)sid, lvl);
            if (dur < 1) dur = 1;
        }
        blk[0] |= e.rd32(mrow + 0xa8);
        blk[1] = (blk[1] & 0xffff0000u) | ((blk[1] | e.rd16(mrow + 0xac)) & 0xffffu);
        uint32_t frames = e.rd32(srow + 0x50);
        if (!frames) frames = 0x8583;
        e.hs(0xA0C70, game, 0, 0, 0, 0, { unit, 0, 0, (uint32_t)dur, frames, 0xB2000000u | 0xC1210, e.stk(blk), 0 }, 6, 3, 0x7);
    }
    return e.h(0x3F320, 0, 0, 0, unit, 0, game);
}
}
