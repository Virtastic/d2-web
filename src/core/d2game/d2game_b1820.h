#pragma once
// Retail-layout (1.13c) D2Game+0xB1820 (UnitTargetModeSwitch): the remembered-target lookup of a monster, by target mode.
// Arguments are 32-bit retail addresses; the callees trap here and the FN probe installs stand-ins through D2Seam.
#include <cstdint>
// EAX = unit; game; p2, p3; param_4 = out node word, param_5 = out value word. Returns 1 with both outs written, else 0.
uint32_t D2Retail113_B1820(uint32_t unit, uint32_t game, uint32_t p2, uint32_t p3, uint32_t* param_4, int32_t* param_5);
uint32_t D2R_B1820_48560(uint32_t unit);                                   // ECX = unit
uint32_t D2R_B1820_B0A20(uint32_t node, uint32_t unit);                    // EBX = ESI = node, stack unit
uint32_t D2R_B1820_AD78(uint32_t node, uint32_t unit, uint32_t four);
uint32_t D2R_B1820_A514(uint32_t unit);
uint32_t D2R_B1820_1450(uint32_t esi, uint32_t two);                       // ESI = unit+0x20
void     D2R_B1820_E0460(uint32_t unit, uint32_t bl, uint32_t one);
uint32_t D2R_B1820_1130(uint32_t unit);
uint32_t D2R_B1820_1100(uint32_t unit);
uint32_t D2R_B1820_A478(uint32_t unit, uint32_t r1100, uint32_t r1130);
uint32_t D2R_B1820_AD66(uint32_t v);
void     D2R_B1820_B17B0(uint32_t eax, uint32_t* blk, uint32_t game, uint32_t unit, uint32_t zero, int nwords);
uint32_t D2R_B1820_DFA90(uint32_t node);
