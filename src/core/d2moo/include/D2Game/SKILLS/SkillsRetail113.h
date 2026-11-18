#pragma once
// Retail-layout (1.13c) skill routines of D2Game.dll over 32-bit retail addresses. The D2Game helpers and D2Common
// ordinals they call are trapping declarations that the FN probes replace through D2Seam.
#include <cstdint>

// 1.13c D2Game+0xD850 (SKILLS_SrvDo131_OverseerWhip; eax = skill id, ecx = pUnit, stack = pGame, pTarget, skill level)
uint32_t D2Retail113_D850(int32_t skillId, uint32_t unit, uint32_t game, uint32_t target, uint32_t level);

// callees, named by retail RVA (D2Game unless noted); registers and stack words as the probes log them
uint32_t D2Retail113_9EE50(uint32_t outDword, uint32_t target, uint32_t zero, uint32_t skillId, uint32_t level);
void D2Retail113_ACBE(uint32_t unitWord4, uint32_t zero, uint32_t levelSlot);
uint32_t D2Retail113_1D30(uint32_t cls, uint32_t levelSlotValue);
void D2Retail113_AFC50(uint32_t unit, uint32_t game, uint32_t v, uint32_t gameSlot);
void D2Retail113_9D230(uint32_t game, uint32_t unit, uint32_t playerWord, uint32_t flags);
void D2Retail113_75070(uint32_t block, uint32_t gameSlot);
void D2Retail113_75E10(uint32_t game, uint32_t block, uint32_t one);
void D2Retail113_A3CA(uint32_t unit, uint32_t a, uint32_t b);
uint32_t D2Retail113_A6A0(uint32_t pool, uint32_t z0, uint32_t z1, uint32_t type, uint32_t id);
void D2Retail113_A60A(uint32_t unit, uint32_t item, uint32_t one);
void D2Retail113_A5B6(uint32_t item, uint32_t a);
void D2Retail113_A6E2(uint32_t item, uint32_t a, uint32_t v, uint32_t z);
uint32_t D2Retail113_232B0(uint32_t w, uint32_t game, uint32_t unit, uint32_t zero);
