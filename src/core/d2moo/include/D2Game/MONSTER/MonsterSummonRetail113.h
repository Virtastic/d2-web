#pragma once
// Retail-layout (1.13c) owned-group spawner of D2Game.dll. Arguments are the 32-bit retail addresses of the owner unit and game;
// the callees are register-convention D2Game helpers (and one D2Common ordinal) whose definitions here trap, so the proofs
// install stand-ins through D2Seam.
#include <cstdint>

// The owned-group spawner (defined with the 245A0 marker): EAX = minCount, EBX = owner, stack (game, mode, maxCount). Spawns minCount..maxCount minions owned by owner.
void D2Retail113_245A0(int32_t minCount, uint32_t owner, uint32_t game, uint32_t mode, int32_t maxCount);

int32_t D2Retail113_21F60(uint32_t owner);                    // EAX = owner: the minion class
uint32_t D2Retail113_MonStatsCount();                         // *(*0x6FD1829C + 0xa80)
int32_t D2Retail113_11D0(uint32_t seed, int32_t range);       // ECX = seed pointer, ESI = range
uint32_t D2Retail113_EF570(uint32_t ecx, uint32_t edx, uint32_t eax, uint32_t game, uint32_t mode, uint32_t three, uint32_t flags);
uint32_t D2Retail113_EDDB0(uint32_t ecx, uint32_t edx, uint32_t eax, uint32_t game, uint32_t three, uint32_t flags);
void D2Retail113_23060(uint32_t unit, uint32_t owner, uint32_t zero);   // ECX = unit, EAX = owner
void D2Retail113_20CF0(uint32_t owner, uint32_t game);                  // EAX = owner
