// D2Game+0xA6AB0 seams: start a skill on a target, each callee reachable through the table so the FN replay can hook it.
#pragma once
#include <cstdint>
struct D2GameA6AB0Env {
    void* (*getSkill)(void* unit, uint32_t skillId, int32_t owner);   // 0xA3D0 D2Common 10708 (ECX unit, EDX skill id, stack owner), ret 4
    uint32_t (*getRoom)(void* unit);                                   // 0xA478 D2Common 10331 (stack unit), ret 4
    uint32_t (*roomBlocked)(uint32_t room);                            // 0xA808 D2Common 10057 (stack room), ret 4
    uint32_t (*skillMode)(void* unit, void* skill);                    // 0xAB7A D2Common 10002 (stack unit, skill), ret 8
    uint32_t (*skillLevel)(void* skill);                               // 0xAE1A D2Common 10136 (stack skill), ret 4; only AL is used
    uint32_t (*doSkill)(void* unit, uint32_t game, uint8_t level, uint32_t skillId, void* target, uint32_t x, uint32_t y);
                                                                       // 0xF2380 (ECX unit, EBX game, AL level, stack skillId, target, x, y), ret 0x10
    uint32_t (*setMode)(uint32_t game, void* unit, void* target, uint32_t mode, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
                                                                       // 0xF2540 (ECX game, EDX unit, stack target, mode, a, b, c, d), ret 0x18
};
// EAX = other, ECX = skill id, ESI = target, EDI = unit; stack game, flag, x, y (ret 0x10).
uint32_t d2game_a6ab0(const D2GameA6AB0Env& env, const void* other, uint32_t skillId, uint8_t* target, void* unit,
                      uint32_t game, uint32_t flag, uint32_t x, uint32_t y);
