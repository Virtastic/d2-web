// D2Common 0x48A40 (ECX, EDX, stack s0..s8; ret 0x24). Written from the 1.13c disassembly: s0 = unit, s1 = record,
// s3 = stat. An item unit (type 4) is refreshed first (0x46E50); unequal min/max roll through 0x1180 with the seed at
// item data +4 (+0x14 holds the data; none = 0), else at unit +0x20 (also for a null unit).
#include <cstdint>
#include "d2common_48a40.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }

// 1.13c D2Common+0x48A40
extern "C" uint32_t D2Common_48A40_With(uint32_t edx, uint32_t unit, uint32_t rec, uint32_t s2, uint32_t stat,
	uint32_t s6, uint32_t s7, const D2Common48a40Env* e)
{
	if (!rec) return 0;
	const bool item = unit && rd(unit) == 4;
	if (item) e->refresh(unit, uint32_t(int32_t(int16_t(stat))));
	int32_t hi = int32_t(rd(rec + 0xC)), lo = int32_t(rd(rec + 8));
	uint32_t v;
	if (hi == lo) v = uint32_t(lo);
	else {
		if (hi < lo) { const int32_t t = hi; hi = lo; lo = t; }
		uint32_t seed;
		if (item) { const uint32_t d = rd(unit + 0x14); seed = d ? d + 4 : 0; }
		else seed = unit + 0x20;
		v = e->roll(seed, uint32_t(hi), uint32_t(lo));
	}
	return e->apply(rec, edx, unit, s2, stat, 0, v, s6, s7);
}
