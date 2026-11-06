// D2Common 0x475E0: add or set one item stat (shifted by the stat row's save shift) on the list 0x47020 resolves.
// EAX gate + 8 stack words (ret 0x20). Callees arrive through Env with the registers retail loads for them.
#pragma once
#include <cstdint>
struct D2Common475e0Env {
	uint32_t dataTables;                                                          // sgptDataTables (+0xBCC rows, +0xBD4 count)
	uint32_t (*statList)(uint32_t eax, uint32_t ecx, uint32_t s0, uint32_t s1);   // 0x47020: EAX, ECX, stdcall 2
	void (*addStat)(uint32_t list, uint32_t id, uint32_t value, uint32_t s3);     // 0x3A280: stdcall 4
	void (*setStat)(uint32_t list, uint32_t id, uint32_t value, uint32_t s3);     // 0x3A330: stdcall 4
	uint32_t (*getStat)(uint32_t list, uint32_t id, uint32_t layer);              // 0x39140: stdcall 3
	int32_t (*findSlot)(uint32_t eax, uint32_t edi);                              // 0x382B0: EAX = stat array, EDI = id<<16
	uint32_t (*newSlot)(uint32_t eax, uint32_t esi, uint32_t s0);                 // 0x38640: EAX = id<<16, ESI = stat array, stdcall 1
	void (*changed)(uint32_t eax, uint32_t edi, uint32_t delta, uint32_t s1);     // 0x39F70: EAX = list, EDI = id<<16, stdcall 2
	void (*flush)(uint32_t eax, uint32_t ebx);                                    // 0x38550: EAX = id<<16, EBX = list
};
extern "C" int32_t D2Common_475E0_With(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
	uint32_t s5, uint32_t s6, uint32_t s7, const D2Common475e0Env* e);
