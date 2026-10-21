// 1.13c D2Common+0x83E20: substitution placement. Visits every cell of the window in seed-shuffled
// order, tests each with the 0x83B30 worker, applies the first hit with the 0x83990 worker.
#include <cstdint>
#include <emscripten.h>

namespace {
inline uint8_t* p(uintptr_t a) { return reinterpret_cast<uint8_t*>(a); }
inline uint32_t& w(uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
inline int32_t& iw(uintptr_t a, int o) { return *reinterpret_cast<int32_t*>(p(a) + o); }
// seed roll in [0,n): n<1 -> 0 (no step); power of two -> mask; else modulo
inline uint32_t roll(uintptr_t s, int32_t n) {
	if (n < 1) return 0;
	uint64_t v = (uint64_t)w(s, 0) * 0x6AC690C5u + w(s, 4);
	*reinterpret_cast<uint64_t*>(p(s)) = v;
	return ((uint32_t)n & ((uint32_t)n - 1)) == 0 ? ((uint32_t)n - 1) & (uint32_t)v : (uint32_t)v % (uint32_t)n;
}
}

extern "C" {
// a1=param_1 (a1[0]=level, a1[1]=box, a1[6]=mode), a2=param_2 (sub record), a3=param_3 (group).
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_83e20(uintptr_t a1, uintptr_t a2, uintptr_t a3,
	int32_t (*test)(int32_t, int32_t, uintptr_t, uintptr_t),
	void (*apply)(int32_t, int32_t, int32_t, int32_t, uintptr_t, uintptr_t, uintptr_t)) {
	uintptr_t lvl = w(a1, 0), box = w(a1, 4);
	int32_t type = iw(lvl, 0x1d0);
	bool small = type >= 2 && type <= 7;
	int32_t step = (iw(a1, 0x18) != 1 || (*(uint32_t*)p(w(lvl, 0x14)) & 0xc) == 0) ? 1 : -1;
	int32_t cw = iw(box, 8) - iw(a3, 8) * iw(a2, 0x4c) + step;
	int32_t ch = iw(box, 0xc) - iw(a3, 0xc) * iw(a2, 0x4c) + 1;
	int32_t n = ch * cw;
	if (n == 0) return 0;
	bool skipCenter = iw(a1, 0x18) == 1 && small && cw <= 5 && ch <= 5;
	static int32_t xs[1024], ys[1024]; // ponytail: window assumed <=1024 cells (retail stack holds 1024)
	if (n > 1024) return 0;
	for (int32_t i = 0; i < n; ++i) { xs[i] = i % cw; ys[i] = i / cw; }
	for (int32_t c = n; c > 0; --c) {
		uint32_t a = roll(lvl + 0x1c4, n), b = roll(lvl + 0x1c4, n);
		int32_t tx = xs[a], ty = ys[a];
		xs[a] = xs[b]; ys[a] = ys[b]; xs[b] = tx; ys[b] = ty;
	}
	for (int32_t i = 0; i < n; ++i) {
		if (skipCenter && xs[i] == 2 && ys[i] == 2) continue;
		if (!test(xs[i], ys[i], a3, a2)) continue;
		uint32_t r = roll(lvl + 0x1c4, iw(a3, 0x14));
		apply(r + 1, (iw(a3, 8) + 1) * (r + 1), xs[i], ys[i], a1, a3, a2);
		if (iw(a2, 0x44) == 0 || iw(a2, 0x44) == 1) return 1;
	}
	return 0;
}
}
