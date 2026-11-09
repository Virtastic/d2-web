// 1.13c D2Game+0xEF8B0: EAX, ECX, EDX, ESI and five stack words (s3 = the boss flag, s4 only its low word), ret 0x14. One 10-dword spawn block
// ([0] ESI, [1] EDX, [2] 0, [3] EAX, [4] ECX, [5] 0, [6] s0, [7] s1, [8] s2, [9] low word s4; its high word is retail stack residue, 0 here)
// goes to 0xEEF10 (ECX = &block). A monster (type 1) it returns gets its monster data flags byte +8 |= 2 (s3 nonzero) | 1, +0x2C = its own
// unit id, +0x30 = 1, +0x28 = ESI. Returns 0xEEF10's unit (EAX is left holding it).
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xEF8B0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EF8B0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t s0, uint32_t s1, uint32_t s2,
                                                            uint32_t s3, uint32_t s4, const D2GameS3aEnv* env)
{
    uint32_t b[10] = {};
    b[3] = eax; b[4] = ecx; b[1] = edx; b[8] = s2; b[9] = s4 & 0xffff; b[5] = 0; b[0] = esi; b[2] = 0; b[6] = s0; b[7] = s1;
    const uint32_t res = (uint32_t)clB(env, b, 10, 0xEEF10, 0, env->h((uintptr_t)b), 0, 0, 0);
    if (!res) return 0;
    const uintptr_t u = env->p(res);
    const uint32_t id = rd32(u, 0xc);
    if (rd32(u, 0) != 1) return res;
    const uint32_t x = rd32(u, 0x14);
    const uintptr_t md = env->p(x ? rd32(env->p(x), 0x28) : 0);
    if (s3) wr8(md, 8, rd8(md, 8) | 2);
    wr8(md, 8, rd8(md, 8) | 1);
    wr32(md, 0x2c, id); wr32(md, 0x30, 1); wr32(md, 0x28, esi);
    return res;
}
