// D2Common 1.13c implementation batch (DRLG room helpers). Register-carried inputs are explicit
// parameters; addresses are uintptr_t; unresolved retail callees and static tables are seams.
#include <cstdint>
#include <emscripten.h>

namespace {
inline uint8_t* p(uintptr_t a) { return reinterpret_cast<uint8_t*>(a); }
inline uint32_t& w(uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
inline int32_t& iw(uintptr_t a, int o) { return *reinterpret_cast<int32_t*>(p(a) + o); }
// retail 32-bit layout: pointer-valued fields are 4 bytes wide
inline void put(uintptr_t a, int o, uintptr_t v) { w(a, o) = (uint32_t)v; }
// seed step: 64-bit {lo,hi} at a, new = lo*0x6AC690C5 + hi; returns low dword
inline uint32_t step(uintptr_t a) {
	uint64_t v = (uint64_t)w(a, 0) * 0x6AC690C5u + w(a, 4);
	*reinterpret_cast<uint64_t*>(p(a)) = v;
	return (uint32_t)v;
}
}

extern "C" {
// 1.13c D2Common+0x84110: walk the table at (EBX+0x50)->+0x50 (0x18-byte records, count at +0x4c)
// from a seed-picked start (unless EBX+0x44 is set), stop at the first record the worker accepts.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_84110(uintptr_t a1, uintptr_t ebx,
	uint32_t (*rng)(uintptr_t), int32_t (*work)(uintptr_t, uintptr_t, uintptr_t)) {
	uintptr_t t = w(ebx, 0x50);
	int32_t n = iw(t, 0x4c), i = 0;
	uint32_t start = 0;
	if (n == 0) return;
	if (iw(a1, 0x14) == -1) iw(a1, 0x14) = 0x3e;
	if (w(ebx, 0x44) == 0) start = rng(w(a1, 0) + 0x1c4);
	for (; i < n; ++i) {
		int32_t r = work(a1, ebx, (uintptr_t)w(t, 0x50) + (int32_t)(start + i) % n * 0x18);
		if (r != 0 && w(ebx, 0x44) == 0) return;
	}
}

// 1.13c D2Common+0x17B70: advance node n (EAX) one step (cursor +0x18, dir +0x1c, count +0x14); on the
// count reaching 3 climb parents (+0x20) until the root (*ctx, EDI), advancing each. Returns the node.
EMSCRIPTEN_KEEPALIVE uintptr_t d2_fn_d2common_17b70(uintptr_t n, const uint32_t* ctx) {
	auto adv = [](uintptr_t a) {
		uintptr_t c = w(a, 0x18) + 1;
		put(a, 0x18, c);
		*(uint8_t*)p(a + 0x1c) = (uint8_t)((*(int8_t*)p(c) + *(int8_t*)p(a + 0x1c)) & 3);
	};
	if (iw(n, 0x14) < 4) adv(n);
	if (++iw(n, 0x14) == 3) {
		while (n != *ctx) {
			n = w(n, 0x20);
			adv(n);
			if (++iw(n, 0x14) != 3) break;
		}
	}
	return n;
}

// 1.13c D2Common+0x17D00: bounded path search over the node pool in ctx (900 nodes of 10 dwords at
// dword 1, used-count at dword 0x2329); target {x,y} in tgt. Returns the node reached or 0.
EMSCRIPTEN_KEEPALIVE uintptr_t d2_fn_d2common_17d00(uintptr_t ctx, int32_t limit, int32_t a3, const int32_t* tgt,
	const int8_t* dy, const int8_t* dx, const int32_t* dirTbl /*stride 0xc*/, const uint8_t* rot /*stride 4*/,
	int32_t (*c17c80)(const int32_t*, int32_t), uintptr_t (*c17b70)(uintptr_t),
	int32_t (*c3e230)(int32_t, int32_t, int32_t)) {
	uintptr_t n = w(ctx, 0);
	const int32_t X = tgt[0], Y = tgt[1];
	for (;;) {
		if (iw(n, 12) == X && iw(n, 16) == Y) return n;
		int32_t y0 = iw(n, 16);
		uint8_t d = *(uint8_t*)p(n + 0x1c);
		int32_t x2 = dx[d] + iw(n, 12), y2 = dy[d] + y0;
		int32_t k = c17c80(tgt, a3);
		if (k != 0) {
			if (iw(n, 12) == x2 || (k = 3, y0 == y2)) k = 2;
			int32_t g = iw(n, 8);
			int32_t ax = x2 - X, ay = y2 - Y;
			if (ax < 0) ax = -ax;
			if (ay < 0) ay = -ay;
			int32_t lo = ax, hi = ay;
			if (!(ay <= ax)) { lo = ax; hi = ay; } else { lo = ay; hi = ax; }
			int32_t h = lo + hi * 2;
			int32_t f = h + g + k;
			if (f <= limit) {
				if (w(n, 0x24) == 0) {
					uintptr_t nn = 0;
					int32_t used = iw(ctx, 0x2329 * 4);
					if (used != 900) {
						nn = ctx + 4 + (uintptr_t)used * 40;
						iw(ctx, 0x2329 * 4) = used + 1;
						for (int j = 0; j < 10; ++j) w(nn, j * 4) = 0;
					}
					put(n, 0x24, nn);
					if (nn == 0) return 0;
					put(nn, 0x20, n);
				}
				n = w(n, 0x24);
				iw(n, 8) = g + k;
				iw(n, 4) = h;
				iw(n, 0) = f;
				put(n, 20, 0);
				int32_t q = c3e230(Y, y2, x2);
				int8_t cv = *(int8_t*)p(w(n, 0x20) + 0x1c);
				int8_t c4 = (int8_t)(dirTbl[q * 3] / 2);
				uint32_t ri = ((uint8_t)(cv - c4)) & 3;
				put(n, 24, (uintptr_t)(rot + ri * 4));
				*(uint8_t*)p(n + 0x1c) = (uint8_t)((rot[ri * 4] + c4) & 3);
				iw(n, 12) = x2;
				iw(n, 16) = y2;
				continue;
			}
		}
		n = c17b70(ctx);
		if (n == 0) return 0;
	}
}

// 1.13c D2Common+0x7EA40: first row 1..(h-2) of column x (EDI) whose kind (obj+0x14)->+4 is 0x1c and
// whose cell byte +2 low nibble is 1; writes {x,row} or {-1,-1}.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7ea40(uintptr_t obj, int32_t x, int32_t* outX, int32_t* outY) {
	uintptr_t g = w(obj, 0x14);
	int32_t end = iw(g, 0x5c) - 1;
	for (int32_t r = 1; r < end; ++r) {
		int32_t c = (int32_t)w(w(g, 8), r * 4) + x;
		if (w(w(g, 4), c * 4) == 0x1c && (*(uint8_t*)p(w(g, 0x2c) + 2 + (int32_t)(w(w(g, 0x30), r * 4) + x) * 4) & 0xf) == 1) {
			*outX = x; *outY = r; return;
		}
	}
	*outX = -1; *outY = -1;
}

// 1.13c D2Common+0x7EB40: (EDI = obj, EBX = x) pick, from a seed-rolled start among rows 1..h-3, a row
// whose flags are clear and cells have nibble 3; place two 0x1c markers via the placer.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7eb40(uintptr_t obj, int32_t x,
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
		ret = c + 1;
	}
	return ret;
}

// 1.13c D2Common+0x68970: (ECX = this, EAX = x) fire a sound for the table entry of param_2 when the
// tile lies inside the room, unless unit param_1 already has flag 0x20; then flag the unit.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_68970(uintptr_t self, int32_t x, uintptr_t unit, uint32_t id, int32_t y,
	const uint8_t* tbl /*retail 6fddbf34, stride 0x1c*/, int32_t (*find)(uintptr_t, uint32_t),
	int32_t (*c1280)(), uint32_t (*c10b0)(uintptr_t), void (*play)(int32_t, int32_t)) {
	if (unit != 0 && (w(unit, 0x14) & 0x20)) return;
	int32_t i = find(w(w(self, 0x58), 0x1d0), id);
	if (i == -1) return;
	const uint8_t* e = tbl + i * 0x1c;
	auto f = [&](int o) { return *reinterpret_cast<const int32_t*>(e + o); };
	int32_t px = (x - iw(self, 0x34)) * 5 + f(8);
	int32_t py = (y - iw(self, 0x38)) * 5 + f(12);
	if (px < 0 || py < 0 || iw(self, 0x3c) * 5 <= px || iw(self, 0x40) * 5 <= py) return;
	int32_t v = f(0), ty = f(4);
	if (ty == 1) v = c1280();
	else if (ty == 2 && v > 0x5a && v < 0x5d && c10b0(self + 0x14) == 0) goto done;
	play(ty, v);
done:
	if (unit != 0) w(unit, 0x14) |= 0x20;
}

// 1.13c D2Common+0x69090: (this) look up entry 0x6c (0x72 when a3 == 0xb) of the room's owner and,
// if the tile offset differs on both axes from the room origin, play the entry's sound.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_69090(uintptr_t self, int32_t x, int32_t y, int32_t a3,
	void (*c19950)(uintptr_t, uint32_t), uintptr_t (*c6c5d0)(int32_t), void (*play)(int32_t, uint32_t)) {
	c19950(self, w(w(self, 0x58), 0x1d0));
	uintptr_t e = c6c5d0(((a3 != 0xb) - 1u & 6) + 0x6c);
	if (e != 0 && x - iw(self, 0x34) != iw(self, 0x3c) && y - iw(self, 0x38) != iw(self, 0x40)) {
		play(5, w(e, 0));
		return 1;
	}
	return 0;
}
}

// 1.13c D2Common+0x68340: find the sound-table row for (levelType key, packed id). Group table A (retail
// 6fddbd68, 38 rows of {key, first, last} dwords) is scanned for key == a; inside [first,last] the row
// of table B (retail 6fddbf28, stride 0x1c) matches when its dword 0 == (id>>20 & 0x3f), dword 1 ==
// (id>>8 & 0xff) and dword 2 == c (the caller's EBX). Returns the row index or -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_68340(int32_t a, uint32_t id, uint32_t c,
	const int32_t* ga /*retail 6fddbd68*/, const uint8_t* tb /*retail 6fddbf28*/) {
	for (int r = 0; r < 38; ++r) {
		const int32_t* g = ga + r * 3;
		if (g[0] != a) continue;
		for (int32_t i = g[1]; i <= g[2]; ++i) {
			const uint32_t* e = reinterpret_cast<const uint32_t*>(tb + i * 0x1c);
			if ((id >> 20 & 0x3f) == e[0] && (id >> 8 & 0xff) == e[1] && c == e[2]) return i;
		}
	}
	return -1;
}

extern "C" {
// 1.13c D2Common+0x3E230: direction index from the offsets (eax - x2, y1 - y2) (fastcall: ecx=y1, edx=y2,
// stack=x2, eax=x1). Same body as the private copy in d2common_batch_15840.cpp.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_3e230(int32_t eax, int32_t y1, int32_t y2, int32_t x2) {
	uint32_t a = (uint32_t)(eax - x2), b = (uint32_t)(y1 - y2);
	int32_t ax = (int32_t)a < 0 ? -(int32_t)a : (int32_t)a, ay = (int32_t)b < 0 ? -(int32_t)b : (int32_t)b;
	if (ax < ay * 2) {
		if (ax * 2 <= ay) {
			if ((int32_t)a < 0) { a = 0xffffffffu; goto L; }
			a &= 1;
		}
	} else if ((int32_t)b < 0) b = 0xffffffffu;
	else b &= 1;
	if ((int32_t)a < -1) a = 0xfffffffeu; else if ((int32_t)a > 1) a = 2;
L:
	if ((int32_t)b < -1) return (int32_t)(a * 5 + 10);
	if ((int32_t)b > 1) b = 2;
	return (int32_t)(a * 5 + 0xc + b);
}
}
