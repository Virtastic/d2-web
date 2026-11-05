// AI scan callback (fastcall + ret 8): EDX = scanning unit, stack = candidate, accumulator
// {+0 idle best, +4 wounded best, +8 picks, +0xC seen, +0x10 max distance, +0x14 alt test, +0x18 skip state test}.
#include <emscripten.h>
#include "d2game_7b860.h"
using namespace d2g7b860;

// 1.13c D2Game+0x7B8E0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7b8e0(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e) {
    if (!cand || rd(cand, 0) != 1 || unit == cand) return 0;
    if (!e->a6f4(unit, cand)) return 0;
    if (e->a514(cand) == 2) return 0;
    if (!((rd(cand, 0xc4) >> 1) & 1)) return 0;
    if (e->ad60(cand) != 0) return 0;
    const uint8_t* row = e->c1220(rd(cand, 4));
    if (rd(acc, 0x14) != 0) {
        if (!e->a508(cand)) return 0;
    } else if (!(e->bitMasks[0xc] & row[0xd])) return 0;
    if (rd(acc, 0x18) == 0 && e->c22a80(cand, 8) != 0) return 0;
    if (e->c1900(unit, cand) > (int32_t)rd(acc, 0x10)) return 0;
    wr(acc, 0xc, rd(acc, 0xc) + 1);
    if (rd(cand, 0x10) == 0xc && e->ad4e(cand) == 0) {
        wr(acc, 0, (uint32_t)(uintptr_t)cand);
        wr(acc, 8, rd(acc, 8) + 1);
        return 0;
    }
    const int32_t hp = e->a42a(cand, 6, 0);
    if (e->a3a0(cand) == hp) return 0;
    if (rd(cand, 0x10) == 0) return 0;
    wr(acc, 4, (uint32_t)(uintptr_t)cand);
    wr(acc, 8, rd(acc, 8) + 1);
    return 0;
}
