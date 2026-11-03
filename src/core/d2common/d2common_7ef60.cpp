// 1.13c D2Common+0x7EF60: ESI = level, ECX forwarded to 0x7E9F0, EAX unused (returned unchanged on the early exit), plain ret.
// Level type 0x27 does nothing. Outdoor info g = level+0x14 (flags +0, kind grid cells +4 with row offsets +8, width +0x5C, height +0x60):
//  1. flags & 0xC and 0x7E9F0(EAX level, ECX fwd, EDX w - 2) true: 0x7ECC0 (ECX level, EAX w - 2).
//  2. flags & 0x20 and not 0x40: roll the seed; scan the kind grid (bit 0 of the roll picks the scan order) for the first cell 0x10 (tile 0x19)
//     or 0x11 (tile 0x18), call 0x42A70 (ECX level, EDX x, y, tile, -1, 0), set flag 0x40; no cell found ends the process (fatal path, never run).
//  3. flags & 0x1C and not 0x40: roll the seed again, place tile 0x33 (0x34 on level type 2) at x = w - (4 | !(flags >> 4 & 1)), or 3 on an
//     odd roll, and y = h - 4, or 3 when roll bit 1 is set; set flag 0x40.
// Returns the final EAX: the flags word read after step 2, or the step 3 call's result.
#include <cstdint>
#include <emscripten.h>
typedef uint32_t (*D2C7EF60Test)(uint32_t level, uint32_t ecx, int32_t x);
typedef uint32_t (*D2C7EF60Fill)(uint32_t level, int32_t x);
typedef uint32_t (*D2C7EF60Put)(uint32_t level, int32_t x, int32_t y, int32_t tile, int32_t m1, int32_t z);
static inline uint32_t rd(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a) + o); }
static inline uint32_t& wr(uint32_t a, uint32_t o) { return *reinterpret_cast<uint32_t*>(uintptr_t(a) + o); }
static inline uint32_t roll(uint32_t level)
{
	const uint64_t n = uint64_t(rd(level, 0x1C4)) * 0x6AC690C5u + rd(level, 0x1C8);
	wr(level, 0x1C4) = uint32_t(n);
	wr(level, 0x1C8) = uint32_t(n >> 32);
	return uint32_t(n);
}
extern "C" uint32_t d2c_7ef60_port(uint32_t level, uint32_t ecx, uint32_t eax0, D2C7EF60Test f7e9f0, D2C7EF60Fill f7ecc0, D2C7EF60Put f42a70)
{
	if (rd(level, 0x1D0) == 0x27) return eax0;
	const uint32_t g = rd(level, 0x14);
	if ((rd(g, 0) & 0xC) && f7e9f0(level, ecx, int32_t(rd(g, 0x5C)) - 2)) f7ecc0(level, int32_t(rd(g, 0x5C)) - 2);
	if ((rd(g, 0) & 0x20) && !(rd(g, 0) & 0x40)) {
		const uint32_t s = roll(level);
		const int32_t w = int32_t(rd(g, 0x5C)), h = int32_t(rd(g, 0x60));
		bool found = false;
		for (int32_t a = 0; a < h && !found; a++)
			for (int32_t b = 0; b < w && !found; b++) {
				const int32_t row = (s & 1) ? b : a, col = (s & 1) ? a : b;   // odd roll: outer counter is x
				const int32_t cell = int32_t(rd(rd(g, 4), (rd(rd(g, 8), (s & 1 ? b : a) * 4) + (s & 1 ? a : b)) * 4));
				(void)row; (void)col;
				if (cell == 0x10 || cell == 0x11) {
					if (s & 1) f42a70(level, a, b, cell == 0x10 ? 0x19 : 0x18, -1, 0);
					else f42a70(level, b, a, cell == 0x10 ? 0x19 : 0x18, -1, 0);
					wr(g, 0) |= 0x40;
					found = true;
				}
			}
		if (!found) __builtin_trap();
	}
	const uint32_t fl = rd(g, 0);
	if ((fl & 0x1C) && !(fl & 0x40)) {
		int32_t x = int32_t(rd(g, 0x5C)) - int32_t((~(fl >> 4) & 1) | 4);
		const int32_t h4 = int32_t(rd(g, 0x60)) - 4;
		const uint32_t s = roll(level);
		if (s & 1) x = 3;
		const int32_t y = ((s & 3) >> 1) == 0 ? h4 : 3;
		const uint32_t r = f42a70(level, x, y, (rd(level, 0x1D0) == 2) + 0x33, -1, 0);
		wr(g, 0) |= 0x40;
		return r;
	}
	return fl;
}
