// Magic-quality item roll: seed, roll quality, force magic (4), write the low seed back, then roll the two affix sets.
// EAX = pP, ESI = pQ, EBX = drop (plain ret).
#include <cstdint>
#include "d2game_itemqual.h"
extern "C" void d2g_104f0(uint32_t* pP, uint32_t* pQ, void* drop, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x104F0
    const uint32_t P = *pP;
    d2g_itemqual_pre(env, P, *pQ, drop, 4);
    uint32_t y = env->imp_a5ec(P);
    *pQ = env->imp_afb2(y);
    env->sub103c0(pP, drop);
}
