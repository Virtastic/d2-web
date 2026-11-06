// D2Common 0x4A670, written from the 1.13c disassembly. EAX = room; level L = room +0x58,
// pool = *(L +0x1B4) +0x478, grids G = room +0x20. Act tier from the level id (L +0x1D0) against 0x28/0x4B/0x67/0x6D/0x400; level
// row = levels + id * 0x9C when 0 <= id < dt +0xC5C. Three grids (G, G +0x14, G +0x28) of (sx +1, sy +1) from room +0x3C/+0x40;
// the first 8x8 cells of G +0x28 are set to 0x40002. Act 0 adds grid G +0x3C (sx +3, sy +3), 0x3DC90 and 0x3DB20. room +0x54 gets
// a zeroed 0x2C-byte block if null. A 15-dword work block goes to 0x84AD0 for the row's +0x40 (room flag bits 16-17) and +0x44
// (bits 12-15) when those bits are set, then for G +0x64/+0x68/+0x6C. Level type (L +0x1C0) picks the flags ORed into every cell
// of G +0x28 that has none of 0x3F0FF80. G +0x14 and G +0x28 are released; EAX is the second release's.
#include <cstdint>
#include "d2common_4a670.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }
static inline uint32_t cell(uint32_t g, uint32_t r, uint32_t c) { return rd(g) + (rd(rd(g + 4) + 4 * r) + c) * 4; }

// 1.13c D2Common+0x4A670
extern "C" uint32_t D2Common_4A670_With(uint32_t room, const D2Common4A670Env* e)
{
	static const int32_t tier[6] = { 0, 0x28, 0x4b, 0x67, 0x6d, 0x400 };
	const uint32_t lv = rd(room + 0x58), pool = rd(rd(lv + 0x1b4) + 0x478);
	const int32_t id = int32_t(rd(lv + 0x1d0));
	uint8_t act = 0;
	for (uint32_t i = 1; i <= 5; i++) if (id < tier[i]) { act = uint8_t(i - 1); break; }
	const uint32_t g = rd(room + 0x20);
	const uint32_t row = (id >= 0 && id < int32_t(rd(e->dataTables + 0xc5c))) ? e->levels + uint32_t(id) * 0x9c : 0;
	const uint32_t fl = rd(room + 0x28), b16 = (fl >> 16) & 3, b12 = (fl >> 12) & 0xf;
	const uint32_t w = rd(room + 0x3c) + 1, h = rd(room + 0x40) + 1;
	e->grid(pool, g, w, h);
	e->grid(pool, g + 0x14, w, h);
	e->grid(pool, g + 0x28, w, h);
	const uint32_t g28 = g + 0x28;
	for (uint32_t r = 0; r < 8; r++) for (uint32_t c = 0; c < 8; c++) e->set(cell(g28, r, c), 0x40002);
	if (!act) {
		e->grid(rd(rd(lv + 0x1b4) + 0x478), rd(room + 0x20) + 0x3c, rd(room + 0x3c) + 3, rd(room + 0x40) + 3);
		e->edge(room, lv);
		e->fill(room);
	}
	if (!rd(room + 0x54)) {
		const uint32_t b = e->alloc(rd(rd(rd(room + 0x58) + 0x1b4) + 0x478), 0x2c, 0x6fde00f0, 0x533, 0);
		wr(room + 0x54, b);
		for (uint32_t i = 0; i < 11; i++) wr(b + 4 * i, 0);
	}
	uint32_t wk[15] = { 0 };
	wk[0] = room; wk[1] = g; wk[5] = g + 0x14; wk[9] = g28; wk[10] = 0; wk[11] = 1;
	if (b16) { wk[12] = rd(row + 0x40); wk[13] = 0; wk[14] = b16; e->tiles(wk); }
	if (b12) { wk[12] = rd(row + 0x44); wk[13] = 0; wk[14] = b12; e->tiles(wk); }
	wk[12] = rd(g + 0x64); wk[13] = rd(g + 0x68); wk[14] = rd(g + 0x6c); e->tiles(wk);
	uint32_t f = 0;
	switch (rd(lv + 0x1c0)) {
	case 0x10: f = 0x100; break;
	case 0x15: f = 0x120000; break;
	case 0x16: f = 0x100000; break;
	case 0x1b: f = 0xa00000; break;
	case 0x1c: f = 0x1600000; break;
	case 0x1f: f = rd(lv + 0x1d0) == 0x75 ? 0x600000 : 0; break;
	default: break;
	}
	if (int32_t(h) > 0)
		for (uint32_t r = 0; int32_t(r) < int32_t(h); r++)
			for (uint32_t c = 0; int32_t(c) < int32_t(w); c++)
				if (!(rd(cell(g28, r, c)) & 0x3f0ff80u)) e->orr(cell(g28, r, c), f);
	e->release(g + 0x14);
	return e->release(g28);
}
