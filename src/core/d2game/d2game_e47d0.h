// 1.13c D2Game+0xE47D0, 0xE4830 and 0xE4890 (size 87 each, ret 4): one shared body, three keys.
// ECX = game, EDX = unit. Two 0xE660 checks (keys: per routine, then 0x20326671); when both return 0
// and game+0x10F4 holds a list, the first node of type 0x10 gets its node+0x18 record's +0xC incremented.
#pragma once
#include <cstdint>

struct GE47d0Env { uint32_t (*e660)(uint32_t a, uint32_t s0); };

inline uint32_t d2game_e47d0_walk(uint32_t game, uint32_t unit, uint32_t key1, const GE47d0Env& env) {
    if (env.e660(unit, key1) != 0) return 0;
    if (env.e660(unit, 0x20326671) != 0) return 0;
    uint32_t hold = *(const uint32_t*)(uintptr_t)(game + 0x10f4);
    if (hold == 0) return 0;
    uint32_t n = *(const uint32_t*)(uintptr_t)hold;
    while (n != 0) {
        if (*(const uint32_t*)(uintptr_t)n == 0x10) {
            uint32_t rec = *(const uint32_t*)(uintptr_t)(n + 0x18);
            *(uint32_t*)(uintptr_t)(rec + 0xc) += 1;
            return 0;
        }
        n = *(const uint32_t*)(uintptr_t)(n + 0xf4);
    }
    return 0;
}
