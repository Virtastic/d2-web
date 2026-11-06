// D2Common 0x43190 (one stack word: the level; ret 4). Written from the 1.13c disassembly. The level type at +0x1C0
// picks the DT1 mask; the outdoor block at level+0x14 holds four grids (+0, +0x14, +0x28, +0x3C: cell table +4, row
// offsets +8 of each grid) and the cell counts (+0x5C wide, +0x60 high). Tiles start at level+0x1C/+0x20 and step 8.
#include <cstdint>
#include "d2common_43190.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }
static inline uint32_t cell(uint32_t grid, int32_t x, int32_t y) { return rd(rd(grid + 4) + (rd(rd(grid + 8) + uint32_t(y) * 4) + uint32_t(x)) * 4); }

// 1.13c D2Common+0x43190
extern "C" void D2Common_43190_With(uint32_t level, const D2Common43190Env* e)
{
	uint32_t mask = 0;
	switch (rd(level + 0x1C0)) {
	case 2: mask = 0x44103; break;
	case 0x10: case 0x16: case 0x1B: case 0x1C: mask = 1; break;
	case 0x15: mask = 4; break;
	case 0x1E: case 0x1F: mask = 0x11; break;
	}
	const uint32_t o = rd(level + 0x14);
	uint32_t ty = rd(level + 0x20);
	for (int32_t y = 0; y < int32_t(rd(o + 0x60)); ++y, ty += 8) {
		uint32_t tx = rd(level + 0x1C);
		for (int32_t x = 0; x < int32_t(rd(o + 0x5C)); ++x, tx += 8) {
			const uint32_t flags = cell(o + 0x14, x, y);
			const uint32_t packed = cell(o + 0x28, x, y);
			if (packed & 0x200) {
				const uint32_t preset = cell(o, x, y);
				if (preset) {
					uint32_t coords[4] = { tx, ty, 0, 0 };
					const uint32_t map = e->allocMap(level + 0x1C4, level, preset, coords);
					wr(map + 4, (packed >> 16) & 0xF);
					e->buildArea(level, map, flags, 0);
				}
			} else if (!(packed & 0x100)) {
				e->addRoom(flags, level, tx, ty, 8, 8, packed, cell(o + 0x3C, x, y), mask);
			}
		}
	}
}
