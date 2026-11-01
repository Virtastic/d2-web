// Finishing dispatch: the item-format probe picks the old-format or the new-format finishing roll.
// EAX = drop, ESI = P, EBX = the stack word handed on (plain ret).
#include <cstdint>
#include "d2game_itemqual.h"
extern "C" void d2g_107b0(void* drop, uint32_t ebx, uint32_t P, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x107B0
    if (env->imp_b37e(P) == 0) env->sub10600(drop, P, ebx); else env->sub10550(P, drop, ebx);
}
