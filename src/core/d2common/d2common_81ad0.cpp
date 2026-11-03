// 1.13c D2Common+0x81AD0: ESI = room, plain ret. For every other neighbour room (array room+8, count room+0x2C) that has a
// data block (+0x64) and whose rect (+0x34/+0x38 origin, +0x3C/+0x40 size) touches this room's (the gap on each axis, signed, below 1),
// calls 0x816C0 along this room's four edges: for each x across the top and bottom rows, then each y down the left and right columns,
// (ECX x, EAX y, EDX neighbour, stack room). cb is the 0x816C0 stand-in.
#include <cstdint>
typedef void (*D2C81AD0Cb)(int32_t x, int32_t y, uint32_t nbr, uint32_t room);
extern "C" void d2c_81ad0_port(uint32_t room, D2C81AD0Cb cb)
{
	auto W = [](uint32_t a, int o) -> int32_t& { return *reinterpret_cast<int32_t*>(uintptr_t(a) + o); };
	for (int32_t i = 0; i < W(room, 0x2c); i++) {
		uint32_t n = uint32_t(W(W(room, 8), i * 4));
		if (n == room || !W(n, 0x64)) continue;
		int32_t dx = W(room, 0x34) < W(n, 0x34) ? (W(n, 0x34) - W(room, 0x3c)) - W(room, 0x34)
		                                       : (W(room, 0x34) - W(n, 0x3c)) - W(n, 0x34);
		int32_t dy = W(room, 0x38) < W(n, 0x38) ? (W(n, 0x38) - W(room, 0x40)) - W(room, 0x38)
		                                       : (W(room, 0x38) - W(n, 0x40)) - W(n, 0x38);
		if (dx >= 1 || dy >= 1) continue;
		for (int32_t x = W(room, 0x34); x <= W(room, 0x3c) + W(room, 0x34); x++) {
			cb(x, W(room, 0x38), n, room);
			cb(x, W(room, 0x40) + W(room, 0x38), n, room);
		}
		for (int32_t y = W(room, 0x38); y <= W(room, 0x40) + W(room, 0x38); y++) {
			cb(W(room, 0x34), y, n, room);
			cb(W(room, 0x3c) + W(room, 0x34), y, n, room);
		}
	}
}
