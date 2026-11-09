// 1.13c D2Game+0xEF7D0: EAX = count, ESI = the spawn record id, seven stack words (s0 = owner unit, s6 only its low word), ret 0x1C.
// Null owner: 0. Else count times: a 10-dword spawn block ([0] ESI, [1] owner room 0xA478, [2] 0, [3] s3, [4] s4, [5] 0, [6] s1, [7] s2,
// [8] s5, [9] low word s6; its high word is retail stack residue, 0 here) goes to 0xEEF10 (ECX = &block). Each unit it returns sets the
// result to 1, a monster (type 1) gets its monster data +0x2C = owner id, +0x30 = 1, +0x28 = ESI, then 0x20CF0 (EAX = owner, EDI = unit, stack ESI).
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xEF7D0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EF7D0(int32_t count, uint32_t esi, uintptr_t owner, uint32_t s1, uint32_t s2, uint32_t s3,
                                                            uint32_t s4, uint32_t s5, uint32_t s6, const D2GameS3aEnv* env)
{
    if (!owner) return 0;
    uint32_t ret = 0;
    const uint32_t oh = env->h(owner);
    const uint32_t room = (uint32_t)cl(env, 0xA478, 0, 0, 0, 0, 0, { oh });
    const uint32_t oid = rd32(owner, 0xc);
    if (count <= 0) return ret;
    uint32_t b[10] = {};
    do {
        b[8] = s5; b[3] = s3; b[4] = s4; b[7] = s2; b[5] = 0; b[1] = room; b[0] = esi; b[2] = 0; b[9] = s6 & 0xffff; b[6] = s1;
        const uint32_t res = (uint32_t)clB(env, b, 10, 0xEEF10, 0, env->h((uintptr_t)b), 0, 0, 0);
        if (res) {
            ret = 1;
            const uintptr_t u = env->p(res);
            if (rd32(u, 0) == 1) {
                const uint32_t x = rd32(u, 0x14);
                const uintptr_t md = env->p(x ? rd32(env->p(x), 0x28) : 0);
                wr32(md, 0x2c, oid); wr32(md, 0x30, 1); wr32(md, 0x28, esi);
            }
            clE(env, res, 0x20CF0, oh, 0, 0, 0, 0, { esi });
        }
    } while (--count != 0);
    return ret;
}
