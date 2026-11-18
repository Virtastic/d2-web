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
}

extern "C" {
// D2Common 0x17B70: advance node n (EAX) one step (cursor +0x18, dir +0x1c, count +0x14); on the
// count reaching 3 climb parents (+0x20) until the root (*ctx, EDI), advancing each. Returns the node.
uintptr_t d2_batch_d2common_17b70(uintptr_t n, const uint32_t* ctx) {
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


// D2Common+0x7EA40 and 0x7EB40 live in the game tree (src/core/game/Retail).

// 1.13c D2Common+0x69090: (this) look up entry 0x6c (0x72 when a3 == 0xb) of the room's owner and,
// if the tile offset differs on both axes from the room origin, play the entry's sound.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_69090_old(uintptr_t self, int32_t x, int32_t y, int32_t a3,
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

// D2Common 0x68340: find the sound-table row for (levelType key, packed id). Group table A (retail
// 6fddbd68, 38 rows of {key, first, last} dwords) is scanned for key == a; inside [first,last] the row
// of table B (retail 6fddbf28, stride 0x1c) matches when its dword 0 == (id>>20 & 0x3f), dword 1 ==
// (id>>8 & 0xff) and dword 2 == c (the caller's EBX). Returns the row index or -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_68340_batch(int32_t a, uint32_t id, uint32_t c,
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
// stand-alone twin of D2Common 0x3E230 (the proven row runs sub_6FDAB610): direction index from the offsets (eax - x2, y1 - y2) (fastcall: ecx=y1, edx=y2,
// stack=x2, eax=x1). Same body as the private copy in d2common_batch_15840.cpp.
int32_t D2Common_3E230_DirFromOffsets(int32_t eax, int32_t y1, int32_t y2, int32_t x2) {
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
