// 1.13c Fog+0x9216: crash-context stash; saves EAX, [EBP+8] and EBP into three globals
// (DAT_6ff7e9a0/a4/a8). Register inputs are explicit parameters here.
#include <cstdint>

static uint32_t g_fog_e9a0, g_fog_e9a4, g_fog_e9a8;

extern "C" void fog_sub_9216(uint32_t eax, uint32_t ebp, uint32_t ebp_arg8) {  // 1.13c Fog+0x9216
    g_fog_e9a4 = ebp_arg8;
    g_fog_e9a0 = eax;
    g_fog_e9a8 = ebp;
}
