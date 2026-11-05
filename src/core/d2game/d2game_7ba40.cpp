// AI scan callback (fastcall + ret 8): EDX = scanning unit, stack = candidate, accumulator
// {+0 nearest, +4 nearest weak, +8 max distance, +0xC best distance, +0x10 best weak distance}; class 0x136 only.
#include <emscripten.h>
#include "d2game_7b860.h"
using namespace d2g7b860;

// 1.13c D2Game+0x7BA40
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7ba40(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e) {
    if (cand == unit || !cand || rd(cand, 0) != 1) return 0;
    if (e->a688(cand) != 0) return 0;
    if (!((rd(cand, 0xc4) >> 1) & 1)) return 0;
    const int32_t k = e->c18a0(cand);
    if (k < 0 || k >= e->tableCount || k != 0x136) return 0;
    const int32_t d = e->c1900(unit, cand);
    if (d > (int32_t)rd(acc, 8)) return 0;
    if (!e->a6f4(unit, cand)) return 0;
    if (e->a514(cand) != 0) return 0;
    if (d < (int32_t)rd(acc, 0xc)) { wr(acc, 0, (uint32_t)(uintptr_t)cand); wr(acc, 0xc, (uint32_t)d); }
    if (d < (int32_t)rd(acc, 0x10) && e->a718(cand) < 0x28) { wr(acc, 4, (uint32_t)(uintptr_t)cand); wr(acc, 0x10, (uint32_t)d); }
    return 0;
}
