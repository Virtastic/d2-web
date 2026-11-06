// 1.13c D2Common+0x4FA30 (stdcall, 2 args, ret 8): true when table row idx is usable by the unit.
// dt +0xB98 row base (0x23C bytes per row), +0xBA0 row count, +0xBCC stat-list table, +0xBD4 its count. Row +5 mask byte, +0x176/+0x178/+0x17A/+0x17C
// signed 16-bit minimums for stats 0, 2, 1, 3. unit +0x5C stat block.
#include <cstdint>
#include "d2common_4fa30.h"

namespace {
inline uint32_t R32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline uint32_t R8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
inline int32_t R16s(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
}

extern "C" uint32_t d2c_4fa30_run(uint32_t unit, int32_t idx, const D2c4FA30Env* e)
{
	const uint32_t dt = e->dt;
	if (idx < 0 || idx >= (int32_t)R32(dt + 0xBA0)) return 0;
	const uint32_t row = (uint32_t)idx * 0x23C + R32(dt + 0xB98);
	if (!row) return 0;
	if (!(R8(e->maskPtr + 8) & R8(row + 5))) return 0;
	int32_t cap = 0;
	if (unit && R32(unit + 0x5C) && (int32_t)R32(dt + 0xBD4) > 0xC && R32(dt + 0xBCC) + 0xF30) cap = e->c38a80(R32(dt + 0xBCC) + 0xF30, 0xC0000);
	if (e->c4f510(unit, idx) > cap) return 0;
	if (R16s(row + 0x176) > e->c38b70(unit, 0, 0)) return 0;
	if (R16s(row + 0x178) > e->c38b70(unit, 2, 0)) return 0;
	if (R16s(row + 0x17A) > e->c38b70(unit, 1, 0)) return 0;
	return R16s(row + 0x17C) <= e->c38b70(unit, 3, 0);
}
