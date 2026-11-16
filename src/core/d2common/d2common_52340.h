// Env of the 1.13c D2Common+0x52340 port (#10787, the skill-use check): retail's eight callees and the max-level global.
#pragma once
#include <cstdint>
struct D2Common52340Env {
    uint32_t maxLevel;                                   // the address held in global 0xA0B50 (the skill-level cap dword)
    int32_t  (*bonus)(uint32_t skill, uint32_t unit);    // 0x4FCB0: EAX = skill, ESI = unit
    uint32_t (*c50930)(uint32_t skill, uint32_t unit);   // 0x50930: ECX = skill, stack unit
    uint32_t (*c50f00)(uint32_t skill, uint32_t unit);   // 0x50F00: EAX = skill, stack unit
    uint32_t (*c50af0)(uint32_t skill, uint32_t unit);   // 0x50AF0: EAX = skill, ECX = unit
    uint32_t (*c4f590)(uint32_t skill, uint32_t unit);   // 0x4F590: EAX = skill, EBX = unit
    uint32_t (*c4fdc0)(uint32_t unit, uint32_t skill);   // 0x4FDC0: stdcall(unit, skill)
    uint32_t (*c50870)(uint32_t skill, uint32_t unit);   // 0x50870: EAX = skill, ECX = unit
    uint32_t (*c52210)(uint32_t unit, uint32_t skill);   // 0x52210: EAX = unit, stack skill
};
extern "C" uint32_t D2Common_52340_SkillUseCheck(uint32_t unit, uint32_t skill, const D2Common52340Env* e);
