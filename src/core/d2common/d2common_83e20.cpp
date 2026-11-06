// 1.13c D2Common+0x83E20 (stdcall p1, p2, p3; ret 0xC): try the cells of a window in a seeded random order, place a preset at the first that fits.
// p1: [0] = unit U (+0x14 points at a flags word, +0x1D0 type, +0x1C4/+0x1C8 the 64-bit seed), [4] = grid block G (+8 w, +0xC h), [0x18] = gate (==1).
// p2: +0x4C scale, +0x44 result of the place step. p3: +8 w, +0xC h, +0x14 n. Window w = G.w - p3.w*scale + step (step -1 when the gate is 1 and
// U's flags word has bits 0xC, else +1), h = G.h - p3.h*scale + 1; n = w*h (0 returns 0). The cells (i % w, i / w) are shuffled with n swaps of two
// seeded picks, then walked: with the gate on, a type in 2..7 and w, h < 6 the centre cell (2,2) is skipped; 0x83B30(esi = p1; x, y, p3, p2) filters;
// r = seeded pick below p3.n (0 for n < 1, no seed step); 0x83990(ecx = r+1, edx = (p3.w+1)*(r+1), eax = x; x, y, p1, p3, p2) places; p2+0x44 of 0 or 1 ends with 1.
// The seed step is state = lo*0x6AC690C5 + hi, as 64 bits at U+0x1C4; the pick is (n-1)&lo for a power of two n, lo % n otherwise.
// Windows over 256 cells overflow retail's stack array and never occur (guard returns 0).
#include <cstdint>
#include <cstring>
#include "d2common_83e20.h"

static inline int32_t rd32(uint32_t b, uint32_t o) { int32_t v; std::memcpy(&v, (const void*)(uintptr_t)(b + o), 4); return v; }
static inline uint32_t ru32(uint32_t b, uint32_t o) { return (uint32_t)rd32(b, o); }

static uint32_t pick(uint32_t U, uint32_t n)
{
	uint64_t s = (uint64_t)ru32(U, 0x1c4) * 0x6ac690c5u + ru32(U, 0x1c8);
	std::memcpy((void*)(uintptr_t)(U + 0x1c4), &s, 8);
	return (n & (n - 1)) == 0 ? ((n - 1) & (uint32_t)s) : (uint32_t)s % n;
}

// 1.13c D2Common+0x83E20
extern "C" uint32_t d2c_83e20_run(uint32_t p1, uint32_t p2, uint32_t p3, const D2c83E20Env* e)
{
	const uint32_t U = ru32(p1, 0), G = ru32(p1, 4);
	const bool gate = rd32(p1, 0x18) == 1;
	const int32_t type = rd32(U, 0x1d0);
	const bool small = type >= 2 && type <= 7;
	const int32_t step = (gate && (ru32(ru32(U, 0x14), 0) & 0xc)) ? -1 : 1;
	const int32_t sc = rd32(p2, 0x4c);
	const int32_t w = rd32(G, 8) - rd32(p3, 8) * sc + step;
	const int32_t h = rd32(G, 0xc) - rd32(p3, 0xc) * sc + 1;
	const int32_t n = w * h;
	if (n == 0 || n > 256) return 0;
	const bool centre = gate && small && w < 6 && h < 6;
	int32_t cx[256], cy[256];
	for (int32_t i = 0; i < n; ++i) { cx[i] = i % w; cy[i] = i / w; }
	for (int32_t k = n; k > 0; --k) {
		const uint32_t a = pick(U, (uint32_t)n), b = pick(U, (uint32_t)n);
		const int32_t x = cx[a], y = cy[a];
		cx[a] = cx[b]; cy[a] = cy[b]; cx[b] = x; cy[b] = y;
	}
	for (int32_t i = 0; i < n; ++i) {
		const int32_t x = cx[i], y = cy[i];
		if (centre && x == 2 && y == 2) continue;
		if (!e->f83b30(p1, x, y, p3, p2)) continue;
		const int32_t pn = rd32(p3, 0x14);
		const uint32_t r = pn > 0 ? pick(U, (uint32_t)pn) : 0;
		e->f83990(r + 1, (uint32_t)(rd32(p3, 8) + 1) * (r + 1), (uint32_t)x, x, y, p1, p3, p2);
		const uint32_t res = ru32(p2, 0x44);
		if (res == 0 || res == 1) return 1;
	}
	return 0;
}
