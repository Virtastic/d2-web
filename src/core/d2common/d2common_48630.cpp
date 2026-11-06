// D2Common 0x48630 (ECX unused, EDX, stack s0..s8; ret 0x24). Written from the 1.13c disassembly: s0 = unit (item only), s1 =
// record, s2 passes through, s5 = amount (0: rolled by 0x465B0 from record +8/+0xC). The type row 0x45A40 returns gives three
// minimum bytes: +0xFF (stat 0x16), +0x103 (stat 0x18), +0x101 (stat 0xA0). Each nonzero minimum b with b + amount <= 0 turns
// the applied amount into -b. Flag 0x2D (0x24430) skips the 0x16/0x18 pair member whose byte is zero while the other is set,
// and skips 0xA0 unless 0x24C30 holds. Returns the amount (as rolled).
#include <cstdint>
#include "d2common_48630.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline uint8_t rb(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }

static uint32_t minAdj(uint32_t b, uint32_t amt) {
	if (b && int32_t(b + amt) <= 0) return uint32_t(0) - b;
	return amt;
}

// 1.13c D2Common+0x48630
extern "C" uint32_t D2Common_48630_With(uint32_t edx, uint32_t unit, uint32_t rec, uint32_t s2, uint32_t amount,
	uint32_t s6, uint32_t s7, const D2Common48630Env* e)
{
	if (!rec) return 0;
	if (!unit || rd(unit) != 4) return 0;
	const uint32_t row = e->typeRow(edx, unit);
	if (!row) return 0;
	if (!amount) amount = e->roll(rd(rec + 0xC), rd(rec + 8), unit);
	if (!e->flag(unit, 0x2d) || rb(row + 0xff) || !rb(row + 0x103)) {
		const uint32_t v = minAdj(rb(row + 0xff), amount);
		if (v) e->apply(rec, edx, unit, s2, 0x16, 0, v, s6, s7);
	}
	if (!e->flag(unit, 0x2d) || rb(row + 0x103) || !rb(row + 0xff)) {
		const uint32_t v = minAdj(rb(row + 0x103), amount);
		if (v) e->apply(rec, edx, unit, s2, 0x18, 0, v, s6, s7);
	}
	if (e->flag(unit, 0x2d) && !e->cond(unit)) return amount;
	const uint32_t v = minAdj(rb(row + 0x101), amount);
	if (v) e->apply(rec, edx, unit, s2, 0xa0, 0, v, s6, s7);
	return amount;
}
