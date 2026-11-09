// 1.13c D2Game+0xEF570: EAX, ECX = spawn class and flag, EDX = the source unit, four stack words (s3 only its low word), ret 0x10.
// Fills a 10-dword spawn block on its stack and hands it to 0xEEF10 (ECX = &block); returns 0xEEF10's result. Block: [0] s0, [1] room of
// the unit (0xA478), [2] s1, [3] EAX, [4] ECX, [5] 0, [6]/[7] the unit's x/y (path +0xC/+0x10 for types 2, 4, 5; 0xA454/0xA3B8 of the
// path otherwise, 0 without one), [8] s2, [9] low word s3 (its high word is stack residue in retail; 0 here).
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xEF570
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EF570(uint32_t eax, uint32_t ecx, uintptr_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3,
                                                            const D2GameS3aEnv* env)
{
    uint32_t b[10] = {};
    b[5] = 0; b[3] = eax; b[4] = ecx;
    b[1] = (uint32_t)cl(env, 0xA478, 0, 0, 0, 0, 0, { env->h(unit) });
    b[0] = s0; b[8] = s2; b[2] = s1; b[9] = s3 & 0xffff;
    const uint32_t t = rd32(unit, 0);
    const bool sp = t == 2 || (t >= 4 && t <= 5);
    const uint32_t path = rd32(unit, 0x2c);
    if (sp) b[6] = rd32(env->p(path), 0xc);
    else b[6] = path ? (uint32_t)cl(env, 0xA454, 0, 0, 0, 0, 0, { path }) : 0;
    if (sp) b[7] = rd32(env->p(path), 0x10);
    else b[7] = path ? (uint32_t)cl(env, 0xA3B8, 0, 0, 0, 0, 0, { path }) : 0;
    return (uint32_t)clB(env, b, 10, 0xEEF10, 0, env->h((uintptr_t)b), 0, 0, 0);
}
