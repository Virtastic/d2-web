#pragma once
// Retail-layout (1.13c) D2Game+0xB1660: the best-attack-target search callback. Arguments are 32-bit retail addresses; the callees
// trap here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
// unused = the hidden ECX word, p2 = EDX (the searcher), unit = candidate, p4 = result struct
uint32_t D2Retail113_B1660(uint32_t unused, uint32_t p2, uint32_t unit, uint32_t p4);
uint32_t D2R_B1660_Ord(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
uint32_t D2R_B1660_DFA90(uint32_t unit);
uint32_t D2R_B1660_E0A60(uint32_t a, uint32_t b);
uint32_t D2R_B1660_218E0();
uint32_t D2R_B1660_21130(uint32_t unit);
uint32_t D2R_B1660_21100(uint32_t unit);
int32_t  D2R_B1660_B0A20(uint32_t unit);
uint32_t D2R_B1660_B08A0(uint32_t unit);
