// D2Common 0x49340: item min/max damage property (ECX, EDX + 9 stack words, ret 0x24). Callees arrive through Env with the
// registers retail loads; the two module globals (0x9FF6C link table, 0x9FB98 item-type rows) arrive as values.
#pragma once
#include <cstdint>
struct D2Common49340Env {
	uint32_t linkTable;                                                   // [0x9FF6C]
	uint32_t typeRows;                                                    // [0x9FB98], 0x1A8 bytes per row
	uint32_t (*roll)(uint32_t eax, uint32_t ecx, uint32_t edx);           // 0x465B0: EAX = record max, ECX = record min, EDX = unit
	uint32_t (*typeRow)(uint32_t eax, uint32_t ecx);                      // 0x45A40: EAX = the caller's EDX, ECX = unit
	uint32_t (*itemRec)(uint32_t cls);                                    // 0x719A0: stdcall 1
	int32_t (*link)(uint32_t table, uint32_t code, uint32_t zero);        // 0x9240 (Fog 10213 thunk): stdcall 3
	void (*setBase)(uint32_t unit, uint32_t stat, uint32_t v, uint32_t z);// 0x3A740: stdcall 4
	uint32_t (*flag)(uint32_t unit, uint32_t id);                         // 0x24430: stdcall 2
	uint32_t (*cond)(uint32_t unit);                                      // 0x24C30: stdcall 1
	int32_t (*mulDiv)(uint32_t ecx, uint32_t eax, uint32_t edx);          // 0x11E0: ECX = 100, EAX = amount, EDX = minimum
	uint32_t (*dmg)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
		uint32_t s5, uint32_t s6, uint32_t s7, uint32_t s8);              // 0x48630: stdcall 9
	uint32_t (*apply)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5,
		uint32_t s6, uint32_t s7);                                        // 0x475E0: EAX = record, stdcall 8
	void (*fatal)(uint32_t line);                                         // 0x921C / 0x9216 / exit(-1): does not return
};
extern "C" uint32_t D2Common_49340_With(uint32_t ecx, uint32_t edx, uint32_t unit, uint32_t rec, uint32_t s2, uint32_t s3,
	uint32_t amount, uint32_t s6, uint32_t s7, uint32_t s8, const D2Common49340Env* e);
