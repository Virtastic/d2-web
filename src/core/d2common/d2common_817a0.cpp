// 1.13c D2Common+0x817A0: fastcall (ECX ctx, EDX x, stack y, stack dir), ret 8. Recursive flood over a tile grid.
// ctx (4-byte cells): [0] room (origin +0x34/+0x38, size +0x3C/+0x40), [1] visited grid, [2] optional type grid (or 0),
// [5] second grid, [7] the mark to or into visited cells; a grid is {cell base, row-offset table}, cell (x, y) = base[off[y] + x].
// The cell callback behind the pointer at 0x9B244 is 0x36C10 (or [ecx], edx) and is done inline. Tables are the retail words
// at 0x8A758 (class per type, from index 0), the mask rows follow from index 21 (5 words per class, indexed by dir + 5 * class)
// and the four step pairs at 0x8A80C.
#include <cstdint>
namespace {
const int32_t kTab[45] = { -1, 0, 1, 2, 2, 0, 1, 3, 0, 1, 0, 1, 4, -1, 4, 0, 0, 0, 0, 0,
	0x17, 0, 5, 0x15, 0x11, 0xF, 3, 0, 9, 7, 0x27, 0, 0, 5, 3, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
const int32_t kStep[8] = { 1, 0, 0, 1, -1, 0, 0, -1 };
inline uint32_t& W(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(uintptr_t(a) + o); }
inline int32_t I(uint32_t a, int o) { return int32_t(W(a, o)); }
inline uint32_t& Cell(uint32_t grid, int32_t x, int32_t y) { return W(W(grid, 0), (uint32_t(I(W(grid, 4), y * 4)) + uint32_t(x)) * 4); }
}
// EAX at return, as retail leaves it: the room pointer when x/y fall outside the room's left/top/right edge, room+0x40 plus
// room+0x38 past the bottom edge, the visited cell index when the cell is already visited; otherwise the last value the
// mask walk loaded (the class, the visited cell base, the dir word, or the last recursive call's EAX). The cell or-in
// (0x36C10) leaves EAX alone.
extern "C" uint32_t d2c_817a0_port(uint32_t ctx, int32_t x, int32_t y, int32_t dir)
{
	for (;;) {
		uint32_t room = W(ctx, 0);
		int32_t ny = I(room, 0x38) + y, nx = I(room, 0x34) + x;
		if (nx < I(room, 0x34) || ny < I(room, 0x38)) return room;
		if (I(room, 0x3c) + I(room, 0x34) < nx) return room;
		if (I(room, 0x40) + I(room, 0x38) < ny) return W(room, 0x40) + W(room, 0x38);
		uint32_t vis = W(ctx, 4), vidx = W(W(vis, 4), y * 4) + uint32_t(x);
		if (W(W(vis, 0), vidx * 4) & 0x10000000) return vidx;
		uint32_t eax = 0;
		if (Cell(W(ctx, 0x14), x, y) == 0) {
			Cell(vis, x, y) |= W(ctx, 0x1c);
			for (int32_t i = 0; i < 4; i++) eax = d2c_817a0_port(ctx, kStep[i * 2] + x, kStep[i * 2 + 1] + y, i);
			return eax;
		}
		uint32_t g2 = W(ctx, 8);
		int32_t t = 0;
		if (g2 && W(g2, 0)) t = int32_t(Cell(g2, x, y));
		eax = uint32_t(kTab[t]);
		uint32_t m = uint32_t(kTab[21 + dir + kTab[t] * 5]);
		if (m & 1) { Cell(vis, x, y) |= W(ctx, 0x1c); eax = W(vis, 0); }
		if ((m & 2) && dir != 2) eax = d2c_817a0_port(ctx, x + 1, y, 0);
		if ((m & 4) && dir != 3) eax = d2c_817a0_port(ctx, x, y + 1, 1);
		if (m & 8) { eax = uint32_t(dir); if (dir != 0) eax = d2c_817a0_port(ctx, x - 1, y, 2); }
		if ((m & 0x10) && dir != 1) eax = d2c_817a0_port(ctx, x, y - 1, 3);
		if (!(m & 0x20)) return eax;
		y += 1; dir = -1; x += 1;
	}
}
