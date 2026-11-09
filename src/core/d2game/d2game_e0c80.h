// 1.13c D2Game+0xE0C80: unit teardown by kind. EAX = unit, EDI = game, stack p1 (ret 4). 32-bit retail words.
// Seams: slotRelease = +0xE0BF0 (EAX = unit, stack game); a6ee = stdcall(unit); a63a = stdcall(unit); finish = +0xE02A0 (ESI unit, EDX game).
// fatal never returns (unit 0 is 0x3AD, kind >= 6 is 0x3AE).
#pragma once
#include <cstdint>
struct D2GameE0C80Env {
    void (*slotRelease)(uint32_t unit, uint32_t game);
    void (*a6ee)(uint32_t unit);
    void (*a63a)(uint32_t unit);
    void (*finish)(uint32_t unit, uint32_t game);
    void (*fatal)(uint32_t code);
};
void d2game_e0c80_port(uint32_t unit, uint32_t game, uint32_t p1, const D2GameE0C80Env* e);
