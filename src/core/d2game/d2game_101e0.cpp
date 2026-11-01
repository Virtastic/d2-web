// Superior-quality item roll: seed the item from the low seed, roll quality, force superior (3) into the item and the drop,
// write the advanced low seed back, read the item format and run the finishing roll. EAX = pP, EBX = pQ, ESI = drop (plain ret).
#include <cstdint>
#include "d2game_itemqual.h"
extern "C" void d2g_101e0(uint32_t* pP, uint32_t* pQ, void* drop, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x101E0
    const uint32_t P = *pP;
    d2g_itemqual_pre(env, P, *pQ, drop, 3);
    uint32_t y = env->imp_a5ec(P);
    *pQ = env->imp_afb2(y);
    env->imp_b37e(P);
    env->sub140b0(P, drop);
}
