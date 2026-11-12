// 1.13c D2Common+0x854C0 (register call, ESI = level): jungle edge levels (types 0x4C..0x4E) lay their preset tiles.
// Level fields (retail 32-bit layout): +0x1B4 drlg (difficulty byte at +0x450), +0x1B8 jungle-def count, +0x1BC tile-code grid,
// +0x1C4 seed, +0x1D0 level type. cols/rows = the type-0x4C level-def size row for the difficulty, >> 5 (rounded toward zero).
// The roll range is 2, or 6 when the def count is 3. A null grid is fatal (0x27) after the roll; a third link slot is fatal (0x47).
// Row walk: type 0x4C last row places tile 0x23D, type 0x4E first row places 0x23E, each flagged when a neighbouring code is zero and
// advancing the grid index by 2; other cells place their code (0 skipped), codes above 0x23E are rebased by the per-type offset and
// take the next link-slot value of the rolled row. Callees and the retail tables are seams so a probe can script them.
#include <cstdint>
#include "d2common_854c0.h"

namespace {
inline int32_t I32(uint32_t base, uint32_t off) { return *(const int32_t*)(uintptr_t)(base + off); }
inline int32_t Div32(int32_t v) { return (v + ((v >> 31) & 0x1F)) >> 5; }
}

// EAX at return: the level type when it is not a jungle edge type, the grid pointer when there are no rows, else what the last
// row left: the place call's EAX for a 0x23D/0x23E row, the column count for an ordinary row (reloaded for the column loop test).
extern "C" uint32_t d2c_854c0_run(uint32_t level, const D2c854C0Env* e)
{
	const int32_t type = I32(level, 0x1D0);
	if (type < 0x4C || type > 0x4E) return uint32_t(type);
	const uint32_t defs = e->dataTables && I32(e->dataTables, 0xC5C) > 0x4C ? e->levelDefBase + 0x2E50 : 0;
	const uint32_t diff = *(const uint8_t*)(uintptr_t)(uint32_t)(I32(level, 0x1B4) + 0x450);
	const int32_t cols = Div32(I32(defs, 0xC + diff * 4));
	const int32_t rows = Div32(I32(defs, 0x18 + diff * 4));
	int32_t idx = 0, slots = 0;
	const uint32_t rollv = e->roll(level + 0x1C4, (I32(level, 0x1B8) == 3) * 4 + 2);
	const uint32_t grid = (uint32_t)I32(level, 0x1BC);
	if (!grid) { e->fatal(0x27); return 0; }
	uint32_t eax = grid;
	for (int32_t row = 0; row < rows; ++row) {
		if (type == 0x4C && row == rows - 1) {
			eax = e->place(level, 0, row * 4, 0x23D, I32(grid, (rows * cols - 1) * 4) == 0, 0);
			idx += 2;
		} else if (type == 0x4E && row == 0) {
			eax = e->place(level, 0, 0, 0x23E, I32(grid, 4) == 0, 0);
			idx += 2;
		} else {
			eax = uint32_t(cols);
			for (int32_t col = 0; col < cols; ++col) {
				int32_t tile = I32(grid, idx * 4), link = -1;
				++idx;
				if (tile > 0x23E) {
					if (slots > 2) { e->fatal(0x47); return 0; }
					tile += e->typeOff[type - 0x4C];
					link = e->layerTab[slots + rollv * 3];
					++slots;
				}
				if (tile != 0) e->place(level, col * 4, row * 4, tile, link, 0);
			}
		}
	}
	return eax;
}
