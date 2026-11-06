// D2Common 0x46A40 (#10890): stdcall(unit), ret 4. Callees arrive through Env with the registers retail loads.
#pragma once
#include <cstdint>
struct D2Common46A40Env {
	uint32_t dataTables;                                                 // sgptDataTables (0x99E1C)
	uint32_t (*flag)(uint32_t unit, uint32_t id);                        // 0x24430: stdcall 2
	uint32_t (*getStat)(uint32_t ebx, uint32_t edi, uint32_t row);       // 0x38B00: EBX = list, EDI = stat << 16, stdcall 1
	uint32_t (*setStat)(uint32_t list, uint32_t stat, uint32_t v, uint32_t layer); // 0x3A280: stdcall 4
	uint32_t (*update)(uint32_t eax, uint32_t ebx);                      // 0x38550: EAX = stat << 16, EBX = list, no stack
	void (*fatal)(uint32_t line);                                        // null unit (line 0x706)
};
extern "C" uint32_t D2Common_46A40_With(uint32_t unit, const D2Common46A40Env* e);
