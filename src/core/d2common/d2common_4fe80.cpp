// 1.13c D2Common+0x4FE80 (stdcall, 2 args, ret 8): finds the 0x2A entry on the unit's stat block (list at +0x3C, then +0x40, linked by +0x2C, id at +0x14),
// reads the stat-list value, scales it by row +0x148 and divides by 8 rounding toward zero. Unit type (first dword) must be 0, 1 or 3.
#include <cstdint>
#include "d2common_4fe80.h"

namespace {
inline uint32_t R32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
}

extern "C" int32_t d2c_4fe80_run(uint32_t unit, int32_t idx, const D2c4FE80Env* e)
{
	const uint32_t dt = e->dt;
	if (!unit) return 0;
	const int32_t type = (int32_t)R32(unit);
	if (type < 0 || (type > 1 && type != 3)) return 0;
	if ((int32_t)R32(dt + 0xC4) <= 0x2A) return 0;
	if (!e->c37db0(unit, 0x2A)) return 0;
	const uint32_t blk = R32(unit + 0x5C);
	if (!blk || (int32_t)R32(blk + 0x10) >= 0) return 0;
	uint32_t n = R32(blk + 0x3C);
	while (n && R32(n + 0x14) != 0x2A) n = R32(n + 0x2C);
	if (!n) {
		n = R32(blk + 0x40);
		while (n && R32(n + 0x14) != 0x2A) n = R32(n + 0x2C);
		if (!n) return 0;
	}
	int32_t v = 0;
	if ((int32_t)R32(dt + 0xBD4) > 0x19 && R32(dt + 0xBCC) + 0x1FA4) v = e->c38b00(R32(dt + 0xBCC) + 0x1FA4, n, 0x190000);
	int32_t mul = 0;
	if (idx >= 0 && idx < (int32_t)R32(dt + 0xBA0)) {
		const uint32_t row = (uint32_t)idx * 0x23C + R32(dt + 0xB98);
		if (row) mul = (int32_t)R32(row + 0x148);
	}
	const uint32_t p = (uint32_t)v * (uint32_t)mul;
	return (int32_t)(p + (((int32_t)p >> 31) & 7)) >> 3;
}
