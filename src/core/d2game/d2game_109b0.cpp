// Normal-quality item roll: seed from Q by value, roll quality, force normal (2), then the finishing dispatch.
// ECX = pP, EAX = Q, ESI = drop, [esp+4] = the word handed on (ret 4).
#include <cstdint>
#include "d2game_itemqual.h"
extern "C" void d2g_109b0(uint32_t* pP, uint32_t Q, void* drop, uint32_t arg, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x109B0
    const uint32_t P = *pP;
    d2g_itemqual_pre(env, P, Q, drop, 2);
    if (env->imp_b37e(P) == 0) env->sub10600(drop, P, arg); else env->sub10550(P, drop, arg);
}
