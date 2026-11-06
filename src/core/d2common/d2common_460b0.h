// D2Common 0x460B0: stdcall(mode, s1, s2, s3, s4, s5), ret 0x18. Callees arrive through Env with the registers retail loads.
#pragma once
#include <cstdint>
struct D2Common460B0Env {
	int32_t (*index)(uint32_t s2);                     // 0x22640: stdcall 1
	uint32_t (*rowA)(uint32_t eax);                    // 0x1830: EAX = index, no stack
	uint32_t (*rowB)(uint32_t eax);                    // 0x1800: EAX = index, no stack
	uint32_t (*kind)(uint32_t s2);                     // 0x221C0: stdcall 1 (only AX is tested)
	uint32_t (*slot)(uint32_t eax, uint32_t ecx, uint32_t edx); // 0x458C0: EAX = i, ECX = s3, EDX = s4, no stack
	// 0x45AA0: EAX, ECX, EDX, EDI = the entry, 7 stack words (callee pops them)
	uint32_t (*emit)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t edi, uint32_t w0, uint32_t w1, uint32_t w2,
		uint32_t w3, uint32_t w4, uint32_t w5, uint32_t w6);
	void (*fatal)(uint32_t line);                      // 0x79 (mode 0, null s3), 0x7A (mode 0, i >= 3), 0x81 (mode 1, i >= 12)
};
extern "C" uint32_t D2Common_460B0_With(uint32_t mode, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5,
	const D2Common460B0Env* e);
