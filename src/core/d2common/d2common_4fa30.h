// 1.13c D2Common+0x4FA30: can the unit use the table row (skill-style requirement test). Retail 32-bit layout; host addresses ride as uint32_t.
#pragma once
#include <cstdint>

struct D2c4FA30Env {
	uint32_t dt;       // sgptDataTables (module global 0x99E1C)
	uint32_t maskPtr;  // dword held at module offset 0x890B0: address of the bit-mask table (byte +8 is tested)
	int32_t (*c38a80)(uint32_t statList, uint32_t edi);              // EDI = 0xC0000, stack stat-list pointer, ret 4
	int32_t (*c4f510)(uint32_t unit, int32_t idx);                    // stdcall, ret 8
	int32_t (*c38b70)(uint32_t unit, int32_t stat, int32_t layer);    // stdcall, ret 0xC
};
extern "C" uint32_t d2c_4fa30_run(uint32_t unit, int32_t idx, const D2c4FA30Env* env);
