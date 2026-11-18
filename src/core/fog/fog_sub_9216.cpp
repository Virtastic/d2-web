// 1.13c Fog+0x9216: crash-context stash; saves EAX, [EBP+8] and EBP into three globals
// (DAT_6ff7e9a0/a4/a8, one 16-byte block at 0x6ff7e99c whose dword 0 is untouched). Register inputs are explicit parameters here.
#include <cstdint>

struct FogStash9216 { uint32_t g99c, g9a0, g9a4, g9a8; };
FogStash9216 g_fog_stash_9216;

extern "C" void fog_sub_9216(uint32_t eax, uint32_t ebp, uint32_t ebp_arg8) {  // 1.13c Fog+0x9216
    g_fog_stash_9216.g9a4 = ebp_arg8;
    g_fog_stash_9216.g9a0 = eax;
    g_fog_stash_9216.g9a8 = ebp;
}
