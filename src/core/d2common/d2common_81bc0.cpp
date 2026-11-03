// 1.13c D2Common+0x81BC0: ESI = ctx (the 0x817A0 context), plain ret. Walks every cell (x 0..room w, y 0..room h; the room is ctx[0],
// size +0x3C/+0x40) whose visited-grid (ctx[1]) cell lacks 0x10000000: bumps the counter ctx[6], sets ctx[7] = (counter & 0xFFFFFFF) | 0x10000000
// (| 0x30000000 instead when the tile word of grid ctx[4] has (w & 0x1E0FF00) == 0x1E00000 or its sign bit set), then floods from the cell
// with 0x817A0(ctx, x, y, -1).
#include <cstdint>
extern "C" void d2c_817a0_port(uint32_t ctx, int32_t x, int32_t y, int32_t dir);
extern "C" void d2c_81bc0_port(uint32_t ctx)
{
	auto W = [](uint32_t a, int o) -> uint32_t& { return *reinterpret_cast<uint32_t*>(uintptr_t(a) + o); };
	auto cell = [&](uint32_t grid, int32_t x, int32_t y) -> uint32_t {
		return W(W(grid, 0), (W(W(grid, 4), y * 4) + uint32_t(x)) * 4);
	};
	int32_t w = int32_t(W(W(ctx, 0), 0x3c)), h = int32_t(W(W(ctx, 0), 0x40));
	for (int32_t y = 0; y <= h; y++)
		for (int32_t x = 0; x <= w; x++) {
			if (cell(W(ctx, 4), x, y) & 0x10000000) continue;
			uint32_t n = W(ctx, 0x18) + 1;
			W(ctx, 0x18) = n;
			uint32_t m = n & 0xfffffff;
			W(ctx, 0x1c) = m | 0x10000000;
			uint32_t t = cell(W(ctx, 0x10), x, y);
			if ((t & 0x1e0ff00) == 0x1e00000 || int32_t(t) < 0) W(ctx, 0x1c) = m | 0x30000000;
			d2c_817a0_port(ctx, x, y, -1);
		}
}
