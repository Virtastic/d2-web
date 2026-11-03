// D2Common 1.13c batch: 0x7B2C0 on the retail 32-bit layout (tokens), the 0x4CE90 collision test a seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Common7b2c0Env {
	// 0x4CE90(map, x, y, pattern, mask): stdcall, non-zero when the spot collides.
	uint32_t (*sub4CE90)(uint32_t pMap, uint32_t x, uint32_t y, uint32_t nPattern, uint32_t nMask);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
uint32_t R16(uint32_t a) { uint16_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }
}

extern "C" {
// 1.13c D2Common+0x7B2C0 (EAX = pPathInfo): true when the dynamic path (+0x30) has no target unit (+0x58), else true when any of
// the eight probes at +-2 around the target (x +4 u16, y +6 u16) is collision-free; stops at the first free one.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_7B2C0_TargetRoomCheck(uint32_t pInfo, const D2Common7b2c0Env* env) {
	if (R32(R32(pInfo + 0x30) + 0x58) == 0) return 1;
	const uint32_t x = R16(pInfo + 4), y = R16(pInfo + 6), map = R32(pInfo + 8), pat = R32(pInfo + 0x28), mask = R32(pInfo + 0x2c);
	static const int dx[8] = {-2, -2, 2, 2, -2, 0, 2, 0}, dy[8] = {-2, 2, -2, 2, 0, -2, 0, 2};
	for (int i = 0; i < 8; ++i) {
		const uint32_t c = env->sub4CE90(map, x + static_cast<uint32_t>(dx[i]), y + static_cast<uint32_t>(dy[i]), pat, mask);
		if (i == 7) return c == 0 ? 1 : 0;
		if (c == 0) return 1;
	}
	return 1;
}
}
