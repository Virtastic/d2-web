// 1.13c D2Game+0xA4B90: monster mode handler. State 1: flag the unit and send a mode-0 packet.
// Else read the unit's position pair, clear its path and send a mode-4 packet (flag byte 0x64, or 0x65 when the class
// record's byte +0x104 meets gdwBitMasks[4]), then set state = 1. Written from the disassembly (the decompile drops the
// register and stack arguments of every callee and the block contents).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_a4b90.h"

typedef uintptr_t A;
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(A p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// 1.13c D2Game+0xA4B90
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_game_A4B90(A game, A unit, A ctx, const D2GameA4B90Env* e)
{
    A st = (A)rd32(ctx, 0);
    uint32_t blk[8] = {};
    if (rd32(st, 0x18) == 1) {
        if (unit) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x20000);
        e->o11103(unit, 0);
        blk[1] = (uint32_t)unit;
        blk[5] = 0x6400;
        e->f95E10(game, blk, 1);
        return;
    }
    int32_t t = (int32_t)rd32(unit, 0);
    bool pathT = t == 2 || (t > 3 && t <= 5);
    A d = (A)rd32(unit, 0x2C);
    uint32_t y, x;
    if (pathT) y = rd32(d, 0x10); else y = d ? (uint32_t)e->o10750(d) : 0;
    if (pathT) x = rd32(d, 0x0C); else x = d ? (uint32_t)e->o10867(d) : 0;
    e->o11103(unit, 0);
    uint32_t flag = 0x64;
    A rec = e->f21320(rd32(unit, 4));
    if (rec && (((const uint8_t*)rec)[0x104] & e->masks[4])) flag = 0x65;
    blk[0] = 4; blk[1] = (uint32_t)unit; blk[3] = x - 10; blk[4] = y; blk[5] = flag << 8;
    e->f95E10(game, blk, 1);
    wr32(st, 0x18, 1);
}
