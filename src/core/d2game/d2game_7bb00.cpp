// AI scan callback (fastcall + ret 8): EDX = scanning unit, stack = candidate, accumulator {+0 best, +4 distance}.
#include <emscripten.h>
#include "d2game_7b860.h"
using namespace d2g7b860;

// 1.13c D2Game+0x7BB00
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7bb00(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e) {
    if (unit == cand || !cand || rd(cand, 0) != 1) return 0;
    const uint32_t f = rd(cand, 0xc4);
    if ((f >> 9) & 1) return 0;
    if (!((f >> 1) & 1)) return 0;
    if (e->a688(cand) == 0) return 0;
    if (e->ad60(cand) != 0) return 0;
    if (e->a514(cand) != 0) return 0;
    if (!e->c13b0(rd(cand, 4), 0xc)) return 0;
    const int32_t d = e->c1900(unit, cand);
    if (d > (int32_t)rd(acc, 4)) return 0;
    wr(acc, 0, (uint32_t)(uintptr_t)cand);
    wr(acc, 4, (uint32_t)d);
    return 0;
}
