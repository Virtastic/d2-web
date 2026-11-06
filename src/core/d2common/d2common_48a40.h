// D2Common 0x48A40: item property with a random min..max amount (record +8/+0xC), applied through 0x475E0.
// ECX (unused), EDX + 9 stack words (ret 0x24). Callees arrive through Env with the registers retail loads.
#pragma once
#include <cstdint>
struct D2Common48a40Env {
	void (*refresh)(uint32_t esi, uint32_t stat);                       // 0x46E50: ESI = unit, stdcall 1 (stat, sign-extended)
	uint32_t (*roll)(uint32_t ecx, uint32_t eax, uint32_t edi);         // 0x1180: ECX = seed, EAX = max, EDI = min
	uint32_t (*apply)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5,
		uint32_t s6, uint32_t s7);                                      // 0x475E0: EAX = record, stdcall 8
};
extern "C" uint32_t D2Common_48A40_With(uint32_t edx, uint32_t unit, uint32_t rec, uint32_t s2, uint32_t stat,
	uint32_t s6, uint32_t s7, const D2Common48a40Env* e);
