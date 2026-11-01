// Rare-quality item roll: seed, roll quality, force rare (6), write the low seed back, then name the rare by item format.
// ECX = pP, EAX = pQ, EBX = drop (plain ret).
#include <cstdint>
#include "d2game_itemqual.h"
extern "C" void d2g_10730(uint32_t* pP, uint32_t* pQ, void* drop, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x10730
    const uint32_t P = *pP;
    d2g_itemqual_pre(env, P, *pQ, drop, 6);
    uint32_t y = env->imp_a5ec(P);
    *pQ = env->imp_afb2(y);
    if (env->imp_b378(P) == 0) return;
    if (env->imp_b37e(P) < 1) env->sub14f20(drop); else env->sub15240(drop);
}
