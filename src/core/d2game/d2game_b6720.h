#pragma once
// Retail-layout (1.13c) D2Game+0xB6720: the end-of-animation tail of the monster skills B6840 and B6A60. Arguments are 32-bit retail
// addresses; the three callees trap here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
uint32_t D2Retail113_B6720(uint32_t game, uint32_t unit);              // EAX = game, EBX = unit
uint32_t D2R_B6720_1320(uint32_t id);                                   // EAX = player class id: its class record
void     D2R_B6720_8E6F0(uint32_t node, uint32_t game);                 // EAX = node, EDI = game: release an attached node
void     D2R_B6720_8EA80(uint32_t game, uint32_t unit, uint32_t flag, uint32_t tick);   // ECX = game, EDX = unit, stack flag, tick
