// Decompile-derived, 1.13c-only. Seams: D2Common #10331/#11025/#10581/#10945 and the D2Game+0xEDDB0 helper.
#include <cstdint>

struct D2Game22b10Env {
    uint32_t (*unitRoom)(uint32_t unit);                       // 0xA478 (#10331)
    uint32_t (*roomCheck)(uint32_t room);                      // 0xB204 (#11025)
    void (*roomSet)(uint32_t room, uint32_t v);                // 0xADFC (#10581)
    void (*notify)(uint32_t one, uint32_t unit, uint32_t a, uint32_t b, uint32_t c);   // 0xEDDB0: ECX = 1, EDX = unit, stack a, b, c
    void (*unitSet)(uint32_t unit, uint32_t a, uint32_t b);    // 0xA3CA (#10945)
};

// 1.13c D2Game+0x22B10
// ECX = game-side word, EDX = unit, two unused stack words (ret 8). When the unit is null or has +0x10 clear: looks its room up and, if the
// room check passes, sets 0x227 on it and notifies (0xEDDB0 with ECX = 1, EDX = unit, stack ECX/1/0 ...); always then sets (unit, 0x68, 1).
extern "C" void d2game_unit_expire_22b10(uint32_t ecx, uint32_t unit, const D2Game22b10Env* env)
{
    if (unit && *(const uint32_t*)(uintptr_t)(unit + 0x10)) return;
    const uint32_t room = env->unitRoom(unit);
    if (env->roomCheck(room)) {
        env->roomSet(room, 0x227);
        env->notify(1, unit, ecx, 1, 0);
    }
    env->unitSet(unit, 0x68, 1);
}
