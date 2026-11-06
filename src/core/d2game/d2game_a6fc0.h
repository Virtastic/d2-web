// Extra callee seams of the 1.13c D2Game+0xA6FC0 port, next to D2Game77dEnv (which carries 0xF1B10, 0xF22C0, 0xF2220, 0xF2C10, 0xF2D80 and 0x2A68E).
#pragma once
#include <cstdint>
#include "d2game_77d_env.h"
struct GA6fc0Extra {
    void     (*fD230)(uint32_t game, uint32_t unit, uint32_t ctl, uint32_t zero);   // 0x9D230: ECX = game, EDX = unit, stack ctl and 0 (ret 8)
    void     (*f8E6F0)(uint32_t node, uint32_t game);                               // 0x8E6F0: EAX = event node, EDI = game
    void     (*f8EA80)(uint32_t game, uint32_t unit, uint32_t kind, uint32_t tick, uint32_t a, uint32_t b, uint32_t c);   // 0x8EA80: ECX = game, EDX = unit, 5 stack words
    uint32_t (*f1560)(uint32_t cls, uint32_t v);                                    // 0x1560: EAX = class id, EDX = value
    uint32_t (*f15D0)(uint32_t cls, uint32_t v);                                    // 0x15D0: EAX = class id, EDX = value
};
extern "C" void d2game_a6fc0_port(const D2Game77dEnv* e, const GA6fc0Extra* x, uint32_t game, uint32_t unit, uint32_t task);
