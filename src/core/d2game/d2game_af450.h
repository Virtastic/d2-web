#pragma once
// Retail-layout (1.13c) D2Game+0xAF450 (UnitDeathAndRespawnStep, the town-portal cast): arguments are 32-bit retail addresses; the
// callees trap here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
// ECX = game, EDX = unit; edi is the caller's EDI, which the routine reads without setting (non-zero adds the 0x8C3B0 notification and
// returns the created object instead of the 0xA24C0 result). The recorded retail calls carry EDI = 0.
uint32_t D2Retail113_AF450(uint32_t game, uint32_t unit, uint32_t edi);
uint32_t D2R_AF450_Room(uint32_t unit);                                 // #10331 (0xA478)
uint32_t D2R_AF450_Area(uint32_t room);                                 // #10826 (0xA556)
uint32_t D2R_AF450_B10459(uint32_t area);                               // #10459 (0xA56E), result byte
uint32_t D2R_AF450_C10585(uint32_t b);                                  // #10585 (0xAADE)
uint32_t D2R_AF450_S10057(uint32_t room);                               // #10057 (0xA808)
void     D2R_AF450_386F0(uint32_t unit);
uint32_t D2R_AF450_1130(uint32_t unit);
uint32_t D2R_AF450_1100(uint32_t unit);
uint32_t D2R_AF450_A24C0(uint32_t game, uint32_t unit, uint32_t room, uint32_t r1100, uint32_t r1130, uint32_t c, uint32_t* out);
void     D2R_AF450_DFFD0(uint32_t unit, uint32_t a, uint32_t b);
void     D2R_AF450_37480(uint32_t unit, uint32_t v);
void     D2R_AF450_2E4F0(uint32_t obj, uint32_t v);
uint32_t D2R_AF450_E1570(uint32_t game, uint32_t obj);
void     D2R_AF450_5E3C0(uint32_t obj, uint32_t game);
uint32_t D2R_AF450_A4C0(uint32_t unit);                                 // #10920
void     D2R_AF450_8C3B0(uint32_t al, uint32_t cl, uint32_t edx, uint32_t arg);
