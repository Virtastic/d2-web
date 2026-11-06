// D2Common 0x48630 / 0x487C0: item damage-property apply (ECX, EDX + 9 stack words, ret 0x24). Callees arrive through Env with
// the registers retail loads; 0x487C0 shares this Env.
#pragma once
#include <cstdint>
struct D2Common48630Env {
	uint32_t (*typeRow)(uint32_t eax, uint32_t ecx);                      // 0x45A40: EAX = the caller's EDX, ECX = unit
	uint32_t (*roll)(uint32_t eax, uint32_t ecx, uint32_t edx);           // 0x465B0: EAX = record max, ECX = record min, EDX = unit
	uint32_t (*flag)(uint32_t unit, uint32_t id);                         // 0x24430: stdcall 2 (unit, 0x2D)
	uint32_t (*cond)(uint32_t unit);                                      // 0x24C30: stdcall 1
	uint32_t (*apply)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5,
		uint32_t s6, uint32_t s7);                                        // 0x475E0: EAX = record, stdcall 8
};
extern "C" uint32_t D2Common_48630_With(uint32_t edx, uint32_t unit, uint32_t rec, uint32_t s2, uint32_t amount,
	uint32_t s6, uint32_t s7, const D2Common48630Env* e);
