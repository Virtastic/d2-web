// Env of the 1.13c D2Common+0x50930 port (does the unit hold what the skill's id asks for): retail's six callees as inputs.
#pragma once
#include <cstdint>
struct D2Common50930Env {
    uint32_t (*isPlayerish)(uint32_t unit);                                             // 0x30530 (stdcall 1)
    uint32_t (*getHands)(uint32_t inv, uint32_t* item, uint32_t* loc, uint32_t* flag);  // 0x20700 (stdcall 4)
    uint32_t (*otherHand)(uint32_t loc, uint32_t inv);                                  // 0x4EBF0 (EAX = loc, ECX = inv)
    uint32_t (*isType)(uint32_t item, uint32_t type);                                   // 0x24430 (stdcall 2)
    uint32_t (*quantity)(uint32_t item);                                                // 0x24C30 (stdcall 1)
    uint32_t (*getStat)(uint32_t item, uint32_t stat, uint32_t layer);                  // 0x39430 (stdcall 3)
};
extern "C" uint32_t D2Common_50930_HoldsSkillItem(uint32_t pSkill, uint32_t pUnit, const D2Common50930Env* e);
