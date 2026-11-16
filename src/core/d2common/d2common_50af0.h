// Env of the 1.13c D2Common+0x50AF0 port (can the unit pay the skill's cost): retail's globals and three callees as inputs.
#pragma once
#include <cstdint>
struct D2Common50AF0Env {
    uint32_t dataTables;                                            // sgptDataTables (only +0xC4 is read)
    uint32_t maxLevelPtr;                                           // [0xA0B50]: address of the max skill level dword
    int32_t (*bonus)(uint32_t skill, uint32_t unit);                // 0x4FCB0 (EAX = skill, ESI = unit)
    uint32_t (*statValid)(uint32_t unit, uint32_t stat);            // 0x37DB0 (EAX = unit, ECX = stat)
    int32_t (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);   // 0x38B70 (stdcall 3)
};
extern "C" uint32_t D2Common_50AF0_CanPayCost(uint32_t pSkill, uint32_t pUnit, const D2Common50AF0Env* e);
