// D2Common 1.13c batch: 0x7B3C0 on the retail 32-bit layout (tokens); the 0x4CE90 collision test and 0x7AFB0 are seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Common7b3c0Env {
	uint32_t (*sub4CE90)(uint32_t pMap, uint32_t x, uint32_t y, uint32_t nPattern, uint32_t nMask);
	// 0x7AFB0(EBX = node, stack info, ctx, packed new point, packed target): stdcall.
	uint32_t (*sub7AFB0)(uint32_t pNode, uint32_t pInfo, uint32_t pCtx, uint32_t tNew, uint32_t tTarget);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
uint16_t R16(uint32_t a) { uint16_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }
}

extern "C" {
// 1.13c D2Common+0x7B3C0 (EAX = node whose u16 x, y are at +0, +2; stack info, ctx, packed target; ret 0xC): for each of the eight
// neighbours that the collision test passes, relax or insert it through 0x7AFB0; 0 as soon as one insertion fails, else 1.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_7B3C0_ExploreChildren(uint32_t pNode, uint32_t pInfo, uint32_t pCtx, uint32_t tTarget, const D2Common7b3c0Env* env) {
	const uint32_t map = R32(pInfo + 8), pat = R32(pInfo + 0x28), mask = R32(pInfo + 0x2c);
	static const int dx[8] = {-1, -1, 1, 1, -1, 0, 1, 0}, dy[8] = {-1, 1, -1, 1, 0, -1, 0, 1};
	for (int i = 0; i < 8; ++i) {
		const uint16_t nx = static_cast<uint16_t>(R16(pNode) + dx[i]), ny = static_cast<uint16_t>(R16(pNode + 2) + dy[i]);
		if (env->sub4CE90(map, nx, ny, pat, mask) != 0) continue;
		if (env->sub7AFB0(pNode, pInfo, pCtx, (static_cast<uint32_t>(ny) << 16) | nx, tTarget) == 0) return 0;
	}
	return 1;
}
}
