#pragma once
// Retail-layout (1.13c) leap-attack landing step of D2Game.dll. Arguments are the 32-bit retail addresses of the skill, game and
// unit; the callees are D2Game thunks and register-convention helpers whose definitions here trap, so the proofs install
// stand-ins through D2Seam. The room lookup (0x1280) and town test (0xA808) are shared with ITEMS/ItemDropRetail113.h.
#include <cstdint>
#include "ITEMS/ItemDropRetail113.h"

// The landing step (defined with the 27030 marker): EAX = skill, stack game, unit, x, y. Returns 1 when the unit is sent on.
uint32_t D2Retail113_27030(uint32_t skill, uint32_t game, uint32_t unit, int32_t x, int32_t y);

uint32_t D2Retail113_AB44(uint32_t skill);                                  // skill param 4
uint32_t D2Retail113_AD3C(uint32_t skill);                                  // skill param 3
uint32_t D2Retail113_E03A0(uint32_t type, uint32_t game, uint32_t id);      // EAX type, ECX game, EDX id: server unit lookup
void D2Retail113_A78A(uint32_t skill, uint32_t flags);
void D2Retail113_ACE8(uint32_t unit, uint32_t frame);
void D2Retail113_ACEE(uint32_t path);
void D2Retail113_AB14(uint32_t path, uint32_t distance);
void D2Retail113_AD18(uint32_t path, uint32_t step);
void D2Retail113_A5FE(uint32_t path, uint32_t targetUnit);
int32_t D2Retail113_1100(uint32_t unit);                                    // ECX unit: x
int32_t D2Retail113_1130(uint32_t unit);                                    // ECX unit: y
uint32_t D2Retail113_A478(uint32_t unit);                                   // the unit's room
void D2Retail113_AB2C(uint32_t path, int32_t x, int32_t y);
void D2Retail113_AAFC(uint32_t path, uint32_t mode);
void D2Retail113_AD24(uint32_t path, uint32_t velocity);
void D2Retail113_AAF0(uint32_t path, uint32_t unit, uint32_t zero);
