// D2Common 0x46600 (#10699 ITEMMODS_GetItemCharges): stdcall(unit, skill, level, pValue, pList), ret 0x14. Callees arrive
// through Env with the registers retail loads; the recursion into contained items stays inside the port.
#pragma once
#include <cstdint>
struct D2Common46600Env {
	uint32_t dataTables;                                       // sgptDataTables (0x99E1C)
	uint32_t (*statList)(uint32_t unit, uint32_t flag);        // 0x384F0: stdcall 2
	uint32_t (*getStat)(uint32_t ebx, uint32_t edi, uint32_t row); // 0x38B00: EBX = list, EDI = 0xCC0000 | layer, stdcall 1 (stat row)
	uint32_t (*first)(uint32_t inv);                           // 0x1E190: stdcall 1
	uint32_t (*next)(uint32_t item);                           // 0x1E8F0: stdcall 1
};
extern "C" uint32_t D2Common_46600_With(uint32_t unit, int32_t skill, int32_t level, uint32_t pValue, uint32_t pList,
	const D2Common46600Env* e);
