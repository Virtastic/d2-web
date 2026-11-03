// 1.13c D2Game+0xA4A10: charge step of the skill a monster/player unit is using.
// Failure of the unit-record walk sends a mode-0 reset packet and returns 1; success reads the charge counter st[+0x18]
// (negative: initialised through the skill row), and either idles (0xF21E0) and returns 1, or consumes one and returns 0.
// Written from the disassembly (the decompile drops the register arguments of 0xE03A0, 0xA3F4 and 0xF21E0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_a4a10.h"

typedef uintptr_t A;
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(A p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline int16_t rd16s(A p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

// 1.13c D2Game+0xA4A10
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_game_A4A10(A unit, A game, A ctx, uint32_t consume, const D2GameA4A10Env* e)
{
    bool ok = false;
    if (unit && rd32(unit, 0) == 1) {
        A p = (A)rd32(unit, 0x14);
        A q = p ? (A)rd32(p, 0x28) : 0;
        A c = q ? (A)rd32(q, 0x28) : 0;
        if (c) {
            A r = e->fE03A0(rd32(q, 0x30), rd32(q, 0x2C), c);
            if (r) { A r2 = e->oA478(r); if (r2 && !e->oA808(r2)) ok = true; }
        }
    }
    if (!ok) {
        e->o11103(unit, 0);
        uint32_t blk[8] = {};
        blk[1] = (uint32_t)unit; blk[5] = 0x6400;
        e->f95E10(game, blk, 1);
        return 1;
    }
    A st = (A)rd32(ctx, 0);
    uint32_t cnt = rd32(st, 0x18);
    if ((int32_t)cnt < 0) {
        int32_t skill = rd16s((A)rd32(ctx, 0x1C), 0x170);
        A row = 0;
        if (skill >= 0 && skill < (int32_t)rd32(e->dt, 0xBA0)) row = (A)((uint32_t)skill * 0x23C + rd32(e->dt, 0xB98));
        if (!row) { e->fF21E0(0, game, 0, 0); return 1; }
        A a = e->oA3F4(unit, skill);
        if (!a) { e->fF21E0(0, game, 0, 0); return 1; }
        A b = e->oA400(unit, a, 1);
        wr32((A)rd32(ctx, 0), 0x18, e->oAC9A(unit, (A)rd32(row, 0x144), skill, b));
        cnt = rd32(st, 0x18);
    }
    if ((int32_t)cnt <= 0) { e->fF21E0(0, game, 0, 0); return 1; }
    if (consume) wr32(st, 0x18, cnt - 1);
    return 0;
}
