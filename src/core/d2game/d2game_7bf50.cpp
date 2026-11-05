// Monster AI step (fastcall + ret 4): ECX = game, EDX = unit, stack = AI context {+8 target, +0x18 active, +0x1C MonStats row}.
// Rolls the unit seed against the per-difficulty chance row[0x56 + 2*diff]; a hit queues an approach command (mode 4,
// 0x65 when the MonStats2 row has the 0x10 flag) through 0x75E10, a miss or an inactive context idles through 0xF22C0.
#include <emscripten.h>
#include "d2game_7b860.h"
using namespace d2g7b860;

// 1.13c D2Game+0x7BF50
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7bf50(const uint8_t* game, uint8_t* unit, const uint8_t* ctx, const D2g7b860Env* e) {
    const uint32_t target = rd(ctx, 8);
    const uint8_t* row = ptr(ctx, 0x1c);
    if (rd(ctx, 0x18) == 0) return e->f22c0(game, unit, 0x28);
    const uint64_t s = (uint64_t)rd(unit, 0x20) * 0x6ac690c5ull + rd(unit, 0x24);
    wr(unit, 0x24, (uint32_t)(s >> 32));
    wr(unit, 0x20, (uint32_t)s);
    const uint32_t diff = game[0x6d];
    if ((int32_t)((uint32_t)s % 100) >= rd16(row, 0x56 + 2 * diff)) return e->f22c0(game, unit, rd16(row, 0x5c + 2 * diff));
    e->a496(unit, 0);
    uint32_t blk[8] = {};
    blk[0] = 4;
    blk[1] = (uint32_t)(uintptr_t)unit;
    uint32_t code = 0x64;
    if (unit) {
        const uint8_t* rec = e->c1320(rd(unit, 4));
        if (rec && (rec[0x104] & rd(e->bitMasks, 0x10))) code = 0x65;
    }
    blk[5] = code << 8;
    blk[2] = target;
    return e->c75e10(game, blk, 1);
}
