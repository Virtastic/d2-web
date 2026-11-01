// Finishing roll for an item of the old format: one step chosen by the item type, else the staff mods and the
// generic finish. ECX = P, EAX = drop, one stack word (ret 4).
#include <cstdint>
#include "d2game_itemqual.h"
extern "C" void d2g_10600(void* drop, uint32_t P, uint32_t arg, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x10600
    switch (env->imp_aa66(P)) {
    case 7:    env->imp_b1aa(P, d2g_itemqual_owner(drop)); env->imp_a65e(P, 0x10000, 1); return;
    case 0xd:  env->sub10240(P, drop); return;
    case 0x12: env->imp_b3b4(P, env->sub132a0(P, 0), 0); return;
    case 0x16: env->imp_b3b4(P, env->sub132a0(P, 1), 0); return;
    case 0x28: env->imp_b1aa(P, d2g_itemqual_owner(drop)); return;
    default:   env->sub13de0(P, drop); env->subec90(P, arg); return;
    }
}
