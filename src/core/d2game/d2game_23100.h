#pragma once
// Retail-layout (1.13c) D2Game+0x23100: does a unique-monster modifier row (entry) suit this unit. Arguments are 32-bit retail addresses;
// the callees and the two module globals it reads trap here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
// stack (unit, entry, force), ret 0xC. entry +4 version cap u16, +6 enabled, +9 pick kind, +0xA/+0xC the two exclude mon types (s16).
uint32_t D2Retail113_23100(uint32_t unit, uint32_t entry, int32_t force);
uint32_t D2R_23100_2790(uint32_t unit, uint32_t monType);                // EAX = unit, EDI = mon type: is the unit nested in that type
uint32_t D2R_23100_1560(uint32_t cls, uint32_t mode);                    // EAX = class, EDX = mode: MonStats2 mode flag
uint32_t D2R_23100_15D0(uint32_t cls, uint32_t flag);                    // EAX = class, EDX = flag index: MonStats flag
uint32_t D2R_23100_Tables();                                             // *sgptDataTables (the tables block)
uint32_t D2R_23100_BitMask1();                                           // gdwBitMasks[1]
