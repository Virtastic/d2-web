// 1.13c D2Common+0x4FE80: per-point value of the bonus carried by the unit's 0x2A state entry. Retail 32-bit layout; host addresses ride as uint32_t.
#pragma once
#include <cstdint>

struct D2c4FE80Env {
	uint32_t dt;       // sgptDataTables (module global 0x99E1C)
	uint32_t (*c37db0)(uint32_t unit, int32_t id);                    // EAX = unit, ECX = id (0x2A)
	int32_t (*c38b00)(uint32_t statList, uint32_t entry, uint32_t edi); // stack stat-list pointer, EBX = entry, EDI = 0x190000, ret 4
};
extern "C" int32_t d2c_4fe80_run(uint32_t unit, int32_t idx, const D2c4FE80Env* env);
