// AI scan callback (fastcall + ret 8): EDX = scanning unit, stack = candidate, accumulator {+0 best, +4 distance, +8 life percent}.
#include <emscripten.h>
#include "d2game_7b860.h"
using namespace d2g7b860;

// 1.13c D2Game+0x7B860
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7b860(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e) {
    if (!cand) return 0;
    const uint32_t t = rd(cand, 0);
    if (t != 1 && t != 0) return 0;
    if (unit == cand) return 0;
    if (e->a688(cand) != 0) return 0;
    const int32_t d = e->c1900(unit, cand);
    if (d > (int32_t)rd(acc, 4)) return 0;
    const int32_t hp = e->a42a(cand, 6, 0);
    const int32_t mx = e->a3a0(cand);
    if (hp > (int32_t)((uint32_t)(mx / 100) * rd(acc, 8))) return 0;
    wr(acc, 0, (uint32_t)(uintptr_t)cand);
    wr(acc, 4, (uint32_t)d);
    return 0;
}
