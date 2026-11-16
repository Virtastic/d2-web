// Env of the 1.13c D2Common+0x52210 port (state-0x79 skill gate): retail's three callees and its two pointer globals.
#pragma once
#include <cstdint>
struct D2Common52210Env {
    uint32_t dataTables;                                    // sgptDataTables (global 0x99E1C); only +0xC4 is read
    uint32_t maxLevel;                                      // the address held in global 0xA0B50 (the skill-level cap dword)
    uint32_t (*hasState)(uint32_t unit, uint32_t state);    // 0x37DB0: EAX = unit, ECX = state
    int32_t  (*bonus)(uint32_t skill, uint32_t unit);       // 0x4FCB0: EAX = skill (ESI = unit at the call)
    int32_t  (*formula)(uint32_t unit, uint32_t calc, int32_t id, int32_t level);   // 0x51BF0: stdcall, four words
};
extern "C" uint32_t D2Common_52210_StateSkillGate(uint32_t unit, uint32_t skill, const D2Common52210Env* e);
