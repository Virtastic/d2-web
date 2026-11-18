#pragma once
// Retail-layout (1.13c) D2Game+0x2AAE0: game lookup by guid under the table lock. Addresses are 32-bit retail addresses; the callees trap
// here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
uint32_t D2Retail113_2AAE0(uint32_t table, uint32_t guid);
void     D2R_2AAE0_Enter(uint32_t pCs);
void     D2R_2AAE0_Leave(uint32_t pCs);
uint32_t D2R_2AAE0_Find(uint32_t table, uint32_t guid);   // 0x2130: EDX = table, ESI = guid
void     D2R_2AAE0_Lock(uint32_t pLock, uint32_t line);   // 0xA328: ECX = game+0x18, EDX = line
void     D2R_2AAE0_Fatal(uint32_t line);
