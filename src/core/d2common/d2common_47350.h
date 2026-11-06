// D2Common 0x47350: item property 18 (a 3-int record packed into one stat value). EDX + 9 stack words (ret 0x24).
#pragma once
#include <cstdint>
struct D2Common47350Env13 {
	uint32_t dataTables;                                                   // sgptDataTables (+0xBCC stat rows, +0xBD4 count)
	uint32_t (*statList)(uint32_t eax, uint32_t ecx, uint32_t s0, uint32_t s1);   // 0x47020: EAX, ECX, stdcall 2
	void (*addStat)(uint32_t list, uint32_t id, uint32_t value, uint32_t s3);     // 0x3A280: stdcall 4
};
extern "C" int32_t D2Common_47350_With(uint32_t edx, uint32_t s0, uint32_t rec, uint32_t stat, uint32_t s6, uint32_t s7,
	const D2Common47350Env13* e);
