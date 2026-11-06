// D2Common 0x487C0 (ECX unused, EDX, stack s0..s8; ret 0x24). Written from the 1.13c disassembly; the 0x48630 shape with the
// type-row bytes +0xFE (stat 0x15), +0x102 (stat 0x17), +0x100 (stat 0x9F), and a nonzero minimum b with b + amount <= 0
// clamps the applied amount to 1 - b.
#include <cstdint>
#include "d2common_487c0.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline uint8_t rb(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }

static uint32_t minAdj(uint32_t b, uint32_t amt) {
	if (b && int32_t(b + amt) <= 0) return 1u - b;
	return amt;
}

// 1.13c D2Common+0x487C0
extern "C" uint32_t D2Common_487C0_With(uint32_t edx, uint32_t unit, uint32_t rec, uint32_t s2, uint32_t amount,
	uint32_t s6, uint32_t s7, const D2Common48630Env* e)
{
	if (!rec) return 0;
	if (!unit || rd(unit) != 4) return 0;
	const uint32_t row = e->typeRow(edx, unit);
	if (!row) return 0;
	if (!amount) amount = e->roll(rd(rec + 0xC), rd(rec + 8), unit);
	if (!e->flag(unit, 0x2d) || rb(row + 0xfe) || !rb(row + 0x102)) {
		const uint32_t v = minAdj(rb(row + 0xfe), amount);
		if (v) e->apply(rec, edx, unit, s2, 0x15, 0, v, s6, s7);
	}
	if (!e->flag(unit, 0x2d) || rb(row + 0x102) || !rb(row + 0xfe)) {
		const uint32_t v = minAdj(rb(row + 0x102), amount);
		if (v) e->apply(rec, edx, unit, s2, 0x17, 0, v, s6, s7);
	}
	if (e->flag(unit, 0x2d) && !e->cond(unit)) return amount;
	const uint32_t v = minAdj(rb(row + 0x100), amount);
	if (v) e->apply(rec, edx, unit, s2, 0x9f, 0, v, s6, s7);
	return amount;
}
