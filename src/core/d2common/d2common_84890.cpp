// 1.13c D2Common+0x84890 (stdcall ctx, param2; ret 8): scatter preset objects over a room grid.
// ctx = grid struct: ctx[0] = level A (+0x3C/+0x40 dims, +0x14/+0x18 the 64-bit seed: seed = lo*0x6AC690C5 + hi stored as
// 64 bits, the low half is the roll), A+0x58, +0x1B4, +0x478 = the value handed to 0x84220; ctx[0xD] indexes the per-class
// chance table at param2+0x11C. P = param2[0x14]: P[0] mode, P[0x13] entry count, P[0x14] entries (0x18 apart; +8 w, +0xC h,
// +0x14 n). Mode 1: every interior cell passing 0x834D0 gets 0x84220(.., 0). Mode 2: every cell passing 0x837F0 whose roll
// % 100 exceeds the chance gets 0x84220(.., (w+1)*(roll+1)), roll = seed & (n-1) for a power-of-two n, seed % n otherwise,
// 0 for n < 1 (no seed step). Callees are seams; the play path passes the real ones (d2common_batch_81bc0.cpp).
#include <cstdint>
#include <cstring>
#include "d2common_84890.h"

extern "C" void d2c_84890_run(void* ctxv, void* param2v, const D2c84890Env* env)
{
	auto rdp = [](const char* b, int o) { return (char*)(uintptr_t)*(const uint32_t*)(b + o); };
	auto rd = [](const char* b, int o) { return *(const int32_t*)(b + o); };
	auto seedStep = [](char* A) {
		uint64_t s = (uint64_t)*(uint32_t*)(A + 0x14) * 0x6ac690c5u + *(uint32_t*)(A + 0x18);
		std::memcpy(A + 0x14, &s, 8);
		return (uint32_t)s;
	};
	char* ctx = (char*)ctxv;
	char* param2 = (char*)param2v;
	char* A = rdp(ctx, 0);
	const uint32_t v = (uint32_t)rd(rdp(rdp(A, 0x58), 0x1b4), 0x478);
	char* P = rdp(param2, 0x50);
	const int w0 = rd(A, 0x3c), h0 = rd(A, 0x40);
	for (int i = 0; i < rd(P, 0x4c); ++i) {
		char* e = rdp(P, 0x50) + i * 0x18;
		const int w = w0 - rd(e, 8) + 1, h = h0 - rd(e, 0xc) + 1;
		if (w <= 0 || h <= 0) continue;
		if (rd(P, 0) == 1) {
			for (int y = 1; y < h; ++y)
				for (int x = 1; x < w; ++x)
					if (env->match1(x, y, ctx, e, param2)) env->place(v, x, y, ctx, e, param2, 0);
		} else if (rd(P, 0) == 2) {
			for (int y = 0; y < h; ++y)
				for (int x = 0; x < w; ++x) {
					if (!env->match2(y, x, ctx, e, param2)) continue;
					const int chance = rd(param2, 0x11c + rd(ctx, 0x34) * 4);
					if (chance >= (int)(seedStep(A) % 100)) continue;
					const int32_t n = rd(e, 0x14);
					uint32_t r = 0;
					if (n > 0) r = (n & (n - 1)) == 0 ? ((uint32_t)(n - 1) & seedStep(A)) : seedStep(A) % (uint32_t)n;
					env->place(v, x, y, ctx, e, param2, (rd(e, 8) + 1) * (int)(r + 1));
				}
		}
	}
}
