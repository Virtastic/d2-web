// 1.13c D2CMP+0x9DA0 (export #10041, 25 bytes): release a palette-shift block from 0xB930.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09DA0
extern "C" void d2cmp_9da0(const D2CmpPalEnv* env, uint8_t* block) {
    if (block != nullptr) env->vfree(block);
}
