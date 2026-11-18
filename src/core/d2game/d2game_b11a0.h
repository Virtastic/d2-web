#pragma once
// Retail-layout (1.13c) D2Game+0xB11A0: the room sweep that offers every live path node of the room's units to a callback.
// Arguments are 32-bit retail addresses; the callees trap here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
int32_t D2Retail113_B11A0(uint32_t arg1, uint32_t arg2, uint32_t arg3);   // the callback is D2R_B11A0_Hit (retail passes its address as arg4)
uint32_t D2R_B11A0_Room(uint32_t arg2);                                    // D2Common #10331 (0xA478)
void     D2R_B11A0_Units(uint32_t room, void** array, uint32_t* count);    // D2Common #10383 (0xA55C)
uint32_t D2R_B11A0_E03A0(uint32_t pathBase, uint32_t pos, uint32_t pathIndex);
uint32_t D2R_B11A0_Hit(uint32_t ecx, uint32_t edx, uint32_t value, uint32_t arg3);
