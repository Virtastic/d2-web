// D2Common 1.13c batch. Retail keeps the search context on the stack (from -0x36FE0); here it is one static block with
// the same 32-bit layout (see d2common_708f0.cpp for the offsets), so 0x708F0 and 0x70650 walk it as retail does.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

int32_t d2_fn_d2common_3e190(uint32_t a, uint32_t b);  // 1.13c D2Common+0x3E190 (d2common_batch_b58b.cpp)
extern "C" uint32_t d2c_708f0_retail(uint32_t ctx, int32_t cutoff, uint32_t pi);
extern "C" uint32_t d2c_708f0_seq0();
extern "C" uint32_t D2Common_70650_PathPoints(const uint8_t* owner, const uint8_t* node);

namespace {
const int32_t kDir[25] = {5, 4, 4, 4, 3, 6, 5, 4, 3, 2, 6, 6, 6, 2, 2, 6, 7, 0, 1, 2, 7, 0, 0, 0, 1};  // DAT_6fddc320 +0 (12-byte rows)
alignas(16) uint8_t g_ctx[0x36FD8];
inline void W32(uint32_t o, uint32_t v) { std::memcpy(g_ctx + o, &v, 4); }
inline void W16(uint32_t o, uint16_t v) { std::memcpy(g_ctx + o, &v, 2); }
inline uint32_t R32(uint32_t o) { uint32_t v; std::memcpy(&v, g_ctx + o, 4); return v; }
}  // namespace

// 1.13c D2Common+0x70BB0: set up the (w+6)x(h+6) cost grid around the room rect, then run the walker with a cutoff
// growing by 5 until it finds a node, runs out of nodes or reaches the limit; flatten the chain (0 when above 0x4D).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2common_70bb0_port(uint32_t owner, const uint16_t* p2, const uint32_t* p3)
{
	const int32_t w = *reinterpret_cast<const int32_t*>(p2 + 4), h = *reinterpret_cast<const int32_t*>(p2 + 6);
	if (50000 < w * h) __builtin_trap();  // retail logs then _exit(-1)
	const uint32_t n = static_cast<uint32_t>((w + 6) * (h + 6)) & 0x3fffffff;
	if (n > (sizeof(g_ctx) - 0x6290) / 4) __builtin_trap();  // retail would clear past its frame
	std::memset(g_ctx + 0x6290, 0, n * 4);
	const uint32_t ctx = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_ctx));
	W16(0x6280, p2[4]); W16(0x6278, p2[0]); W16(0x627A, p2[2]); W16(0x6282, p2[6]);
	W16(0x627C, static_cast<uint16_t>(p2[4] + p2[0]));
	W16(0x627E, static_cast<uint16_t>(p2[6] + p2[2]));
	W32(0x6284, static_cast<uint32_t>(p2[4]) + 6);
	W32(0x6288, 3u - p2[0]);
	W32(0x628C, 3u - p2[2]);
	const uint32_t start = p3[0], goal = p3[1];
	int32_t dx = static_cast<int32_t>(goal & 0xffff) - static_cast<int32_t>(start & 0xffff);
	if (dx < 0) dx = -dx;
	int32_t dy = static_cast<int32_t>(goal >> 16) - static_cast<int32_t>(start >> 16);
	if (dy < 0) dy = -dy;
	int32_t lo = dx, hi = dx;
	if (dx < dy || dx <= dy) hi = dy; else lo = dy;
	const uint16_t d16 = static_cast<uint16_t>(lo + hi * 2);
	// the 28-byte start node copied into node 0 on every attempt (retail -0x36FFC)
	uint8_t s[0x1C] = {};
	const uint32_t d32 = d16, fffd = 0xfffd, dir = static_cast<uint32_t>(kDir[d2_fn_d2common_3e190(start, goal)]) & 7, seq = d2c_708f0_seq0();
	std::memcpy(s + 0, &d16, 2); std::memcpy(s + 2, &d32, 4); std::memcpy(s + 6, &fffd, 2);
	std::memcpy(s + 8, &start, 4); std::memcpy(s + 0xC, &seq, 4); std::memcpy(s + 0x10, &dir, 4);
	std::memset(g_ctx + 4, 0, 0x24);
	W32(0, ctx + 4);
	int32_t cur = static_cast<int16_t>(d16), limit;
	const uint8_t b = static_cast<uint8_t>(p3[7]);
	if (p3[8] == 0) {
		limit = static_cast<int16_t>(b > cur ? b : cur);
	} else {
		limit = cur;
		if (p3[8] == 0x10) {
			cur = static_cast<int16_t>(static_cast<int16_t>(d16) / 2 + static_cast<int16_t>(d32));
			limit = static_cast<int16_t>(b > cur ? b : cur);
		}
	}
	const uint32_t pi = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p3));
	uint32_t r;
	do {
		W32(0x6274, 1);
		std::memcpy(g_ctx + 4, s, sizeof(s));
		r = d2c_708f0_retail(ctx, cur, pi);
		cur += 5;
	} while (((r || R32(0x6274) != 900) && (W32(0x6274, 1), !r)) && cur < limit);
	uint32_t out = 0;
	if (r) {
		out = D2Common_70650_PathPoints(reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(owner)),
		                                reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(r)));
		if (out > 0x4d) out = 0;
	}
	return out;
}
