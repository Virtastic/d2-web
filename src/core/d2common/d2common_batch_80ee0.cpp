// 1.13c D2Common+0x80EE0: recursive rewrite of a room's linked-list entries. Retail pointers are
// 32-bit image addresses. room+0x64 -> data block; if non-null and bit0 of its first byte is clear,
// walk the list headed at data+0x30 (next at +0x2C), replacing each entry's +0x28 field equal to
// `from` with `to`. If any matched, recurse into every other neighbour room (array room+8, count
// room+0x2C) whose room+0x58 -> +0x1D0 matches this room's. param_4 is passed through unused.
#include <cstdint>
#include <emscripten.h>

namespace
{
	inline uint32_t& W(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a) + o); }
}

// 1.13c D2Common+0x80EE0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_80ee0(uint32_t room, uint32_t from, uint32_t to, uint32_t p4)
{
	uint32_t data = W(room, 0x64);
	if (!data || (*reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(data)) & 1)) return;
	bool hit = false;
	for (uint32_t e = W(data, 0x30); e; e = W(e, 0x2C))
		if (W(e, 0x28) == from) { hit = true; W(e, 0x28) = to; }
	if (!hit) return;
	for (int32_t i = 0; i < static_cast<int32_t>(W(room, 0x2C)); i++)
	{
		uint32_t n = W(W(room, 8), i * 4);
		if (n != room && W(W(n, 0x58), 0x1D0) == W(W(room, 0x58), 0x1D0))
			d2_fn_d2common_80ee0(n, from, to, p4);
	}
}
