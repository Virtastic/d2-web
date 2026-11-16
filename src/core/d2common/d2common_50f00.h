// Env of the 1.13c D2Common+0x50F00 port (skill item-type requirement on the two hand items): retail's three callees.
#pragma once
#include <cstdint>
struct D2Common50F00Env {
    uint32_t (*gridSlot)(uint32_t inv);                                  // 0x1E1B0: EAX = 0, EBX = inventory, stack = the grid global
    uint32_t (*weapon)(uint32_t inv);                                    // 0x20320: stack = inventory
    uint32_t (*fits)(uint32_t eax, uint32_t a, uint32_t b, uint32_t txt);   // 0x50610: EAX = 0/1, stack = two items and the skills.txt record
};
extern "C" uint32_t D2Common_50F00_SkillItemTypes(uint32_t skill, uint32_t unit, const D2Common50F00Env* e);
