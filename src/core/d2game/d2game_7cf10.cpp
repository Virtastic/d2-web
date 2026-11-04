// 0x7CF10 nearest-target scan callback with the retail register and stack arguments.
// fastcall + ret 8: ECX = game, EDX = scanning unit, stack = candidate, accumulator
// {+0 nearest, +4 nearest dist, +8 near count, +0xC lowest-life unit, +0x10 lowest life,
//  +0x14 nearest hostile, +0x18 its dist, +0x1C seen count}.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "d2game_7cf10.h"

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7cf10(uint32_t game, const uint8_t* unit, const uint8_t* cand,
                                                       uint8_t* acc, const D2g7cf10Env* e) {
    auto rd = [](const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; };
    auto wr = [](uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); };
    if (unit == cand || e->dead(cand) != 0 || !cand || !((rd(cand, 0xc4) >> 2) & 1)) return 0;
    if (e->check(cand, unit, game) == 0) return 0;
    const int32_t d = e->dist(unit, cand);
    if (d > 0x400) return 0;
    wr(acc, 0x1c, rd(acc, 0x1c) + 1);
    if (d <= 0xe1) wr(acc, 8, rd(acc, 8) + 1);
    if (d < (int32_t)rd(acc, 4)) { wr(acc, 0, (uint32_t)(uintptr_t)cand); wr(acc, 4, (uint32_t)d); }
    const int32_t hp = e->stat(cand, 6, 0) >> 8;
    if (hp < (int32_t)rd(acc, 0x10)) { wr(acc, 0xc, (uint32_t)(uintptr_t)cand); wr(acc, 0x10, (uint32_t)hp); }
    if (d < (int32_t)rd(acc, 0x18) && unit && rd(unit, 0) == 0) { wr(acc, 0x14, (uint32_t)(uintptr_t)cand); wr(acc, 0x18, (uint32_t)d); }
    return 0;
}
