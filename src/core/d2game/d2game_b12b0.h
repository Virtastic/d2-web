#pragma once
// Retail-layout (1.13c) D2Game+0xB12B0: the dead-minion search callback. Arguments are 32-bit retail addresses; the callees trap
// here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
// p1 unused, p2 owner (EDX, also the EBX of 0xB0A20), u candidate, p4 = { unit, count, dist, maxDist }
uint32_t D2Retail113_B12B0(uint32_t p1, uint32_t p2, uint32_t u, uint32_t p4);
uint32_t D2R_B12B0_Ord(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);   // D2Common ordinal n
uint32_t D2R_B12B0_B0A20(uint32_t ebx, uint32_t u);                                                      // distance, EBX = searcher
uint32_t D2R_B12B0_21260(uint32_t u);                                                                    // minion owner
