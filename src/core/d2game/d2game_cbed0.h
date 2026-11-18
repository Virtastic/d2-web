#pragma once
// Retail-layout (1.13c) D2Game+0xCBED0 (EAX = unit): sends the 0x5A/0x0E notice through 0x8B400.
#include <cstdint>
void     D2Retail113_CBED0(uint32_t unit);
uint32_t D2R_CBED0_A4C0(uint32_t unit);                    // Ordinal 10920: the unit's client record
void     D2R_CBED0_8B400(uint32_t packet, uint32_t arg);   // EAX = packet (0x28 bytes), stack word arg
