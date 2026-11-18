// Bridge placer of the wilderness river, moved from the stand-alone d2common batch 84110 into the game tree.
// Raw retail-image offsets: the level object and its grid block are read at their 32-bit offsets.
#include <cstdint>
#include <emscripten.h>

namespace {
inline uint8_t* p(uintptr_t a) { return reinterpret_cast<uint8_t*>(a); }
inline uint32_t& w(uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
// seed step: 64-bit {lo,hi} at a, new = lo*0x6AC690C5 + hi; returns low dword
inline uint32_t step(uintptr_t a) {
	uint64_t v = (uint64_t)w(a, 0) * 0x6AC690C5u + w(a, 4);
	*reinterpret_cast<uint64_t*>(p(a)) = v;
	return (uint32_t)v;
}
}

// 1.13c D2Common+0x7EB40: (EDI = obj, EBX = x) pick, from a seed-rolled start among rows 1..h-3, a row
// whose flags are clear and cells have nibble 3; place two 0x1c markers via the placer.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_7eb40_port(uintptr_t obj, int32_t x,
	uint32_t (*place)(uintptr_t, int32_t, int32_t, int32_t, int32_t, int32_t)) {
	uintptr_t g = w(obj, 0x14);
	uint32_t h = w(g, 0x60), alt = w(g, 0) & 4;
	uint32_t m = h - 2, start = 0, ret = alt;
	if ((int32_t)m >= 1) {
		uint32_t s = step(obj + 0x1c4);
		if ((m & (h - 3)) == 0) start = (h - 3) & s;
		else start = s % m;
	} else return ret;
	for (uint32_t c = 0; (int32_t)c < (int32_t)m; ++c) {
		int32_t row = (int32_t)(start + c) % (int32_t)m + 1;
		uintptr_t g2 = w(obj, 0x14);
		uintptr_t cell = (uintptr_t)(int32_t)(w(w(g2, 0x30), row * 4) + x) * 4;
		uint32_t fl;
		if (alt == 0) {
			if ((w(w(g2, 0x2c) + cell, -4) & 0x1b81) != 0) continue;
			fl = w(w(g2, 0x2c) + cell, 8);
		} else fl = w(w(g2, 0x2c) + cell, -4);
		uintptr_t c2 = (uintptr_t)(int32_t)(w(w(g, 0x30), row * 4) + x) * 4;
		if ((fl & 0x1b81) == 0 && (*(uint8_t*)p(w(g, 0x2c) + 2 + c2) & 0xf) == 3 && (*(uint8_t*)p(w(g, 0x2c) + 6 + c2) & 0xf) == 3) {
			place(obj, x, row, 0x1c, 1, 0);
			return place(obj, x + 1, row, 0x1c, (alt != 0) + 2, 0);
		}
	}
	return m;   // retail falls out with the loop counter, which equals m
}
