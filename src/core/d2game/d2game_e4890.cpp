// 1.13c D2Game+0xE4890 (size 87, ret 4): key 0x20796571, then 0x20326671. See d2game_e47d0.h.
#include "d2game_e47d0.h"
uint32_t g_e4890(uint32_t ecx, uint32_t edx, const GE47d0Env& env) {
    return d2game_e47d0_walk(ecx, edx, 0x20796571, env);
}
