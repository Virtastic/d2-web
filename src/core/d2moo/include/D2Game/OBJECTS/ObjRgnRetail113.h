#pragma once
// Retail-layout (1.13c) object-region control teardown of D2Game.dll over 32-bit retail addresses; the pool free is Fog's
// 0xA2EC, which the game reaches through its own pool type, so its definition traps and the proofs install a stand-in
// through D2Seam.
#include <cstdint>

// 1.13c D2Game+0xB8A0 (OBJRGN_FreeObjectControl; EDI = pGame): returns the result of the last free (0 for a null game or no control)
uint32_t D2Retail113_B8A0(uint32_t game);

// Fog 0xA2EC (free): pool = game+0x1C, block, the objrgn.cpp source-name word, line, 0
uint32_t D2Retail113_FogFree_A2EC(uint32_t pool, uint32_t block, uint32_t file, uint32_t line, uint32_t zero);
