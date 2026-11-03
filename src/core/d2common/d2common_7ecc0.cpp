// 1.13c D2Common+0x7ECC0: ECX = level, EAX = x (kept in EBX), plain ret. Level +0x14 = outdoor info: rows h = +0x60, flag grid cells +0x2C
// with row offsets +0x30, kind grid cells +0x04 with row offsets +0x08. For every row two cells (x, x+1) are mapped to a tile value
// (kind 0: flags & 0x100 ? 0 : 3; kind 7 with flags & 0xF0000 == 0x30000: 3; else the 8-byte-stride .rdata table at 0x8ACA0) and
// sent to 0x42A70 (ECX level, EDX x, stack row, 0x1A / 0x1B, value, 0). Then, when the info flags have bit 0x14, 0x7EB40 (EDI = level, EBX = x).
// Returns the final EAX: the 0x7EB40 result, else the row count h.
#include <cstdint>
#include <emscripten.h>
typedef uint32_t (*D2C7ECC0Set)(uint32_t level, int32_t x, int32_t y, int32_t tbl, int32_t val, int32_t z);
typedef uint32_t (*D2C7ECC0Fn)(uint32_t level, int32_t x);
static const int32_t kTbl8ACA0[32] = {
	8, 8, 8, 8, 9, 9, 0xA, 0xB, 2, 2, 0, 3, 1, 1, 3, 0,
	0, 2, 0, 1, 1, 0, 2, 0, 2, 3, 1, 3, 3, 1, 3, 2 };
static inline uint32_t rd(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a) + o); }
static inline int32_t map(int32_t kind, uint32_t flags, int lane)
{
	if (kind == 0) return (flags & 0x100) ? 0 : 3;
	if (kind == 7 && (flags & 0xF0000) == 0x30000) return 3;
	return kTbl8ACA0[kind * 2 + lane];
}
extern "C" uint32_t d2c_7ecc0_port(uint32_t level, int32_t x, D2C7ECC0Set set, D2C7ECC0Fn f7eb40)
{
	const uint32_t g = rd(level, 0x14);
	int32_t h = int32_t(rd(g, 0x60));
	for (int32_t i = 0; i < h; i++) {
		const int32_t ib = int32_t(rd(rd(g, 0x30), i * 4)) + x, ia = int32_t(rd(rd(g, 8), i * 4)) + x;
		set(level, x, i, 0x1A, map(int32_t(rd(rd(g, 4), ia * 4)), rd(rd(g, 0x2C), ib * 4), 0), 0);
		set(level, x + 1, i, 0x1B, map(int32_t(rd(rd(g, 4), ia * 4 + 4)), rd(rd(g, 0x2C), ib * 4 + 4), 1), 0);
		h = int32_t(rd(g, 0x60));
	}
	if (rd(g, 0) & 0x14) return f7eb40(level, x);
	return uint32_t(h);
}
