// 1.13c D2Common+0x814C0: EDI = room, stack arg0 = grid {base ptr, row-offset table} (ret 4). Flags the grid cells of every
// movable entry (flags +0x14: (f & 0x1c000) == 0x4000, +0x1c != 0xF, bit 11 clear) of the room's own list (room+0x54 holds the
// +0x14 array, count +0x18, 0x30 bytes each), then of the same kind of entry in every other neighbour room (room+8 array, count room+0x2C)
// whose type-0 nodes (room+0x54 holds the list head; node +0 type, +4 entry list, +8 next; entry next +0x20) lie inside this room's rect
// (+0x34/+0x38 origin, +0x3C/+0x40 size). The retail call goes through the pointer at 0x9B244 (0x36C10: or [ecx], edx, edx = 1); done inline.
#include <cstdint>
namespace {
inline uint32_t& W(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(uintptr_t(a) + o); }
inline int32_t I(uint32_t a, int o) { return int32_t(W(a, o)); }
}
extern "C" void d2c_814c0_port(uint32_t room, uint32_t grid)
{
	uint32_t rd = W(room, 0x54);
	for (uint32_t i = 0, o = 0; i < W(rd, 0x18); i++, o += 0x30) {
		uint32_t e = W(rd, 0x14) + o, f = W(e, 0x14);
		if ((f & 0x1c000) == 0x4000 && W(e, 0x1c) != 0xf && (~(f >> 11) & 1)) {
			uint32_t idx = W(W(grid, 4), W(e, 0xc) * 4) + W(e, 8);
			W(W(grid, 0), idx * 4) |= 1;
		}
	}
	for (uint32_t i = 0; i < W(room, 0x2c); i++) {
		uint32_t n = W(W(room, 8), i * 4);
		if (n == room || !W(n, 0x54)) continue;
		for (uint32_t nd = W(W(n, 0x54), 0); nd; nd = W(nd, 8)) {
			if (W(nd, 0) != 0) continue;
			for (uint32_t e = W(nd, 4); e; e = W(e, 0x20)) {
				uint32_t f = W(e, 0x14);
				if ((f & 0x1c000) != 0x4000 || W(e, 0x1c) == 0xf || !(~(f >> 11) & 1)) continue;
				int32_t y = I(n, 0x38) + I(e, 0xc), x = I(n, 0x34) + I(e, 8);
				if (x >= I(room, 0x34) && y >= I(room, 0x38) && x <= I(room, 0x3c) + I(room, 0x34) && y <= I(room, 0x40) + I(room, 0x38)) {
					int32_t idx = I(W(grid, 4), (y - I(room, 0x38)) * 4) - I(room, 0x34) + x;
					W(W(grid, 0), uint32_t(idx) * 4) |= 1;
				}
			}
		}
	}
}
