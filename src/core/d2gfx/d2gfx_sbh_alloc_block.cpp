// D2Gfx 1.13c ___sbh_alloc_block (MSVC 2003 small-block heap). Retail 32-bit addresses are held as
// uint32_t; the globals and the two heap-internal callees are host seams.
#include <cstdint>
#include <emscripten.h>

struct D2GfxSbhHeap { uint32_t count, capacity; uint8_t* regions; void* heap; uint32_t cursor, cache, cache_grp; };
extern "C" D2GfxSbhHeap g_D2Gfx_sbh;
extern "C" uint32_t* D2Gfx_46EB_SbhAllocNewRegion();
extern "C" int32_t D2Gfx_47A2_SbhAllocNewGroup(uint32_t region);
static inline uint32_t* W(uint32_t a) { return (uint32_t*)(uintptr_t)a; }

// 1.13c D2Gfx+0x04B87 (764 B): ___sbh_alloc_block(size). Round size+0x17 to 16, pick the first region
// (searching from the cursor, wrapping) whose bucket bitmaps have a bucket >= need, else a region with a
// free group slot, else a new region; commit a new group when needed. Split the best free block, relink
// the remainder into its bucket (maintaining the per-bucket counts at +4 and the bitmaps), mark the
// allocation with size|1 at both ends, and return the payload (block+4); 0 on failure.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Gfx_4B87_SbhAllocBlock(uint32_t size) {
	D2GfxSbhHeap* s = &g_D2Gfx_sbh;
	uint32_t uVar7 = (size + 0x17) & 0xfffffff0u;
	int32_t iVar8 = ((int32_t)(size + 0x17) >> 4) - 1;
	uint32_t end = (uint32_t)(uintptr_t)s->regions + s->count * 20;
	uint32_t pr = s->cursor, uVar14, local_c;
	if (iVar8 < 0x20) { uVar14 = 0xffffffffu >> (iVar8 & 0x1f); local_c = 0xffffffffu; }
	else { uVar14 = 0; local_c = 0xffffffffu >> ((iVar8 - 0x20) & 0x1f); }
	auto fits = [&](uint32_t p) { return (W(p)[1] & local_c) || (W(p)[0] & uVar14); };
	while (pr < end && !fits(pr)) pr += 20;
	if (pr == end) {
		uint32_t q = (uint32_t)(uintptr_t)s->regions;
		while (q < s->cursor && !fits(q)) q += 20;
		pr = q;
		if (q == s->cursor) {
			while (q < end && W(q)[2] == 0) q += 20;
			uint32_t q2 = (uint32_t)(uintptr_t)s->regions;
			pr = q;
			if (q == end) {
				while (q2 < s->cursor && W(q2)[2] == 0) q2 += 20;
				pr = q2;
				if (q2 == s->cursor) { pr = (uint32_t)(uintptr_t)D2Gfx_46EB_SbhAllocNewRegion(); if (!pr) return 0; }
			}
			int32_t g = D2Gfx_47A2_SbhAllocNewGroup(pr);
			*W(W(pr)[4]) = (uint32_t)g;
			if (*W(W(pr)[4]) == 0xffffffffu) return 0;
		}
	}
	uint32_t c = W(pr)[4];  // control block
	int32_t grp = (int32_t)*W(c);
	auto bm0 = [&](int32_t g) -> uint32_t& { return W(c)[g + 0x11]; };
	auto bm1 = [&](int32_t g) -> uint32_t& { return W(c)[g + 0x31]; };
	if (grp == -1 || ((bm1(grp) & local_c) == 0 && (bm0(grp) & uVar14) == 0)) {
		grp = 0;
		if ((bm1(0) & local_c) == 0 && (bm0(0) & uVar14) == 0) {
			do { grp++; } while ((bm1(grp) & local_c) == 0 && (bm0(grp) & uVar14) == 0);
		}
	}
	uint32_t piVar3 = c + (uint32_t)(grp * 0x81 + 0x51) * 4;  // group header
	int32_t bk = 0;
	uint32_t m = bm0(grp) & uVar14;
	if (m == 0) { m = bm1(grp) & local_c; bk = 0x20; }
	for (; (int32_t)m >= 0; m <<= 1) bk++;
	uint32_t blk = W(piVar3)[bk * 2 + 1];
	int32_t rem = (int32_t)(W(blk)[0] - uVar7);
	int32_t nb = (rem >> 4) - 1;
	if (nb > 0x3f) nb = 0x3f;
	s->cursor = pr;
	if (nb != bk) {
		if (W(blk)[1] == W(blk)[2]) {
			uint8_t* cnt = (uint8_t*)(uintptr_t)(c + bk + 4);
			if (bk < 0x20) {
				uint32_t k = ~(0x80000000u >> (bk & 0x1f));
				bm0(grp) &= k; if (--*cnt == 0) W(pr)[0] &= k;
			} else {
				uint32_t k = ~(0x80000000u >> ((bk - 0x20) & 0x1f));
				bm1(grp) &= k; if (--*cnt == 0) W(pr)[1] &= k;
			}
		}
		W(W(blk)[2] + 4)[0] = W(blk)[1];
		W(W(blk)[1] + 8)[0] = W(blk)[2];
		if (rem != 0) {
			uint32_t head = piVar3 + nb * 8;
			uint32_t tail = W(head)[1];
			W(blk)[2] = head; W(blk)[1] = tail; W(head)[1] = blk;
			W(W(blk)[1] + 8)[0] = blk;
			if (W(blk)[1] == W(blk)[2]) {
				uint8_t* cnt = (uint8_t*)(uintptr_t)(c + nb + 4);
				uint8_t old = (*cnt)++;
				if (nb < 0x20) {
					if (old == 0) W(pr)[0] |= 0x80000000u >> (nb & 0x1f);
					bm0(grp) |= 0x80000000u >> (nb & 0x1f);
				} else {
					if (old == 0) W(pr)[1] |= 0x80000000u >> ((nb - 0x20) & 0x1f);
					bm1(grp) |= 0x80000000u >> ((nb - 0x20) & 0x1f);
				}
			}
		}
	}
	if (rem != 0) { W(blk)[0] = (uint32_t)rem; W(blk + rem - 4)[0] = (uint32_t)rem; }
	blk += (uint32_t)rem;
	W(blk)[0] = uVar7 + 1;
	W(blk + uVar7 - 4)[0] = uVar7 + 1;
	uint32_t was = W(piVar3)[0]++;
	if (was == 0 && pr == s->cache && (uint32_t)grp == s->cache_grp) s->cache = 0;
	*W(c) = (uint32_t)grp;
	return blk + 4;
}
