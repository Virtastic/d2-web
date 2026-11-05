// Monster AI step (fastcall + ret 4): ECX = game, EDX = unit, stack = AI context {+0 state, +8 target, +0x14 range,
// +0x1C MonStats row}. Out of range, out of uses or no target: flag 0x20000 and a stop command through 0x75E10. Otherwise
// casts the row's skill (row[0x170], mode byte row[0x180]) once per state toggle and idles row[0x62 + 2*diff] in between.
#include <emscripten.h>
#include "d2game_7b860.h"
using namespace d2g7b860;

// 1.13c D2Game+0x7C1D0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7c1d0(const uint8_t* game, uint8_t* unit, const uint8_t* ctx, const D2g7b860Env* e) {
    uint8_t* st = (uint8_t*)(uintptr_t)rd(ctx, 0);
    const uint32_t target = rd(ctx, 8);
    const uint8_t* row = ptr(ctx, 0x1c);
    const uint32_t diff = game[0x6d];
    if (target != 0 && (int32_t)rd(ctx, 0x14) <= rd16(row, 0x56 + 2 * diff) && (int32_t)rd(st, 0x14) < rd16(row, 0x5c + 2 * diff)) {
        const int32_t skill = rd16(row, 0x170);
        if (skill >= 0 && rd(st, 0x18) == 0) {
            const uint32_t r = e->f2380(((uint32_t)skill & 0xffffff00u) | row[0x180], unit, skill, target, 0, 0);
            wr(st, 0x14, rd(st, 0x14) + 1);
            wr(st, 0x18, 1);
            return r;
        }
        wr(st, 0x18, 0);
        return e->f22c0(game, unit, rd16(row, 0x62 + 2 * diff));
    }
    if (unit) wr(unit, 0xc4, rd(unit, 0xc4) | 0x20000u);
    e->a496(unit, 0);
    uint32_t blk[8] = {};
    blk[1] = (uint32_t)(uintptr_t)unit;
    blk[5] = 0x64u << 8;
    return e->c75e10(game, blk, 1);
}
