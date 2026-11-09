// 1.13c D2Game+0xEAF80: EAX = unit (player, unit[0] == 0). Brings the unit's base stats 0..3 to the class row at dt[0xBC4] + unit[4] * 0xC4
// (row +0x30, +0x31, +0x32, +0x33), adjusting stat 4 and calling EAE20 / EAD10 (EAX = delta, ESI = unit) for the +0x32 / +0x33 deltas.
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xEAF80
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_EAF80(uintptr_t unit, uintptr_t dt, const D2GameS3aEnv* env)
{
    if (!unit || rd32(unit, 0) != 0) return;
    int32_t cls = (int32_t)rd32(unit, 4);
    if (cls < 0 || cls >= (int32_t)rd32(dt, 0xbc8)) return;
    const uint32_t rh = rd32(dt, 0xbc4) + (uint32_t)cls * 0xc4;   // retail tests the 32-bit sum
    if (rh == 0) return;
    const uintptr_t row = env->p(rh);
    const uint32_t u = env->h(unit);
    auto get = [&](uint32_t s) { return (uint32_t)cl(env, 0xA3E2, 0, 0, 0, 0, 0, { u, s, 0 }); };
    auto add = [&](uint32_t s, uint32_t v) { cl(env, 0xA4D2, 0, 0, 0, 0, 0, { u, s, v, 0 }); };
    auto fix = [&](uint32_t s, uint32_t want) {
        uint32_t d = want - get(s);
        if (d) { add(4, 0u - d); add(s, d); cl(env, 0xA4DE, 0, 0, 0, 0, 0, { u }); }
    };
    fix(0, rd8(row, 0x30));
    cl(env, 0xEAE20, rd8(row, 0x32) - get(1), 0, 0, 0, u);
    fix(2, rd8(row, 0x31));
    cl(env, 0xEAD10, rd8(row, 0x33) - get(3), 0, 0, 0, u);
}
