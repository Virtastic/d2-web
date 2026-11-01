// Finishing roll for an item of the new format: each of five item-type checks (0xD, 0x28, 7, 0x16, 0x12) runs its
// own step, then the staff mods. EAX = P, EDI = drop, one unused stack word (ret 4).
#include <cstdint>
#include "d2game_itemqual.h"
extern "C" void d2g_10550(uint32_t P, void* drop, uint32_t arg, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x10550
    (void)arg;
    if (env->imp_a430(P, 0xd)) env->sub10240(P, drop);
    if (env->imp_a430(P, 0x28)) env->imp_b1aa(P, d2g_itemqual_owner(drop));
    if (env->imp_a430(P, 7)) { env->imp_b1aa(P, d2g_itemqual_owner(drop)); env->imp_a65e(P, 0x10000, 1); }
    if (env->imp_a430(P, 0x16)) env->imp_b3b4(P, env->sub132a0(P, 1), 0);
    if (env->imp_a430(P, 0x12)) env->imp_b3b4(P, env->sub132a0(P, 0), 0);
    env->sub13de0(P, drop);
}
