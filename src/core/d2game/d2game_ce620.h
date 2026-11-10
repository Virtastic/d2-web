// Seams of src/core/d2game/d2game_ce620.cpp (shared with the FN probe in src/core/fn), in retail register/stack order.
#pragma once
#include <cstdint>

struct D2GameCE620Env {
    uint32_t (*o10747)(uint32_t unit);                                  // thunk 0xA856 (stdcall 1)
    uint32_t (*f_cb1f0)(uint32_t ebx, uint32_t s0);                     // 0xCB1F0: EBX = unit, 1 stack word = skill (ret 4)
    uint32_t (*o10787)(uint32_t unit, uint32_t skill);                  // thunk 0xA8C8 (stdcall 2): the use mode
    uint32_t (*o10858)(uint32_t ecx);                                   // thunk 0xA928: ECX = skill, the skills.txt record
    uint32_t (*o10708)(uint32_t ecx, uint32_t edx, uint32_t s0);        // thunk 0xA3D0: ECX = unit, EDX = 0, 1 stack word = -1
    uint32_t (*o10952)(uint32_t unit, uint32_t skill);                  // thunk 0xAB68 (stdcall 2)
    uint32_t (*f_e03a0)(uint32_t eax, uint32_t ecx, uint32_t edx);      // 0xE03A0: EAX = type, ECX = game, EDX = id: the target unit
    uint32_t (*f_a68e)(uint32_t unit, uint32_t v);                      // thunk 0xA68E (stdcall 2): unit, 0x8F
    uint32_t (*f_e0a20)(uint32_t eax, uint32_t s0);                     // 0xE0A20: EAX = target, 1 stack word = game
    uint32_t (*f_ab7a)(uint32_t unit, uint32_t skill);                  // thunk 0xAB7A (stdcall 2)
    uint32_t (*f_aac0)(uint32_t tgt);                                   // thunk 0xAAC0 (stdcall 1)
    uint32_t (*f_a754)(uint32_t unit, uint32_t tgt, uint32_t v);        // thunk 0xA754 (stdcall 3)
    void (*f_78430)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);   // EAX = game, 5 stack words
    uint32_t (*f_a49c)(uint32_t skill, uint32_t file, uint32_t line);   // thunk 0xA49C (stdcall 3): skill, PlrMsg.cpp (module data), 0x7A6
    void (*f_ce120)(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t esi, uint32_t s0);   // 0xCE120: EAX = game, EBX = type, ECX = unit, ESI = id
};

// EAX = skill, ECX = unit, stack = game, target type, target id, flag; ret 0x10.
uint32_t d2game_ce620_port(uint32_t skill, uint32_t unit, uint32_t game, uint32_t type, uint32_t id, uint32_t flag, const D2GameCE620Env* e);
