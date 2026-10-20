// D2Common 1.13c implementation batch (DRLG room/tile helpers). Register-carried inputs are explicit
// parameters; addresses are uintptr_t; retail callees, fatal exit and static tables are seams.
#include <cstdint>
#include <emscripten.h>

namespace {
inline uint8_t* p(uintptr_t a) { return reinterpret_cast<uint8_t*>(a); }
inline uint32_t& w(uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
inline int32_t& iw(uintptr_t a, int o) { return *reinterpret_cast<int32_t*>(p(a) + o); }
}

extern "C" {
// 1.13c D2Common+0x69170: take the next 0x30-byte slot from (obj+0x54) pool, push it on the list whose
// head slot is headSlot (4-byte pointer cell, may be 0), run worker 0x68DF0; returns the slot.
EMSCRIPTEN_KEEPALIVE uintptr_t d2_fn_d2common_69170(uintptr_t headSlot, uintptr_t eax_obj, int32_t a1, int32_t a2,
	uint32_t a3, int32_t a4, void (*w68df0)(int32_t, int32_t, uint32_t, int32_t)) {
	uintptr_t pool = w(eax_obj, 0x54);
	uintptr_t slot = (uintptr_t)(iw(pool, 0xc) * 0x30) + w(pool, 0x1c);
	if (headSlot == 0) {
		w(slot, 0x20) = 0;
	} else {
		w(slot, 0x20) = w(headSlot, 0);
		w(headSlot, 0) = (uint32_t)slot;
	}
	iw(pool, 0xc) += 1;
	w68df0(a2, a1, a3, a4);
	return slot;
}

// 1.13c D2Common+0x69360: (edi = level/room ctx). Find the list node at edi+0x4c whose (+0x10)->[0]
// equals table[a2>>20&0x3f] from 0x69950; if type byte (a2>>8) is 0/4 or 0x69090 accepts, link a1 onto the
// node's +0xc list and (when (+0x10)+0x24 set) add a flagged entry via 0x68F00. Fatal ids 0x2b1/0x2cb.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_69360(uintptr_t self, uintptr_t a1, uint32_t a2, uintptr_t a3, uintptr_t edi,
	uintptr_t (*w69950)(uintptr_t, uint32_t), int32_t (*w69090)(uintptr_t, int32_t, int32_t, uintptr_t),
	void (*w68800)(uintptr_t, uintptr_t), uint32_t (*w68b90)(uintptr_t, uintptr_t),
	uintptr_t (*w68f00)(uintptr_t, uintptr_t, int32_t, int32_t, uint32_t, uint32_t, uintptr_t),
	void (*fatal)(uint32_t) /* ordinals 10265 + 10024 + _exit(-1); seam, may return under test */) {
	uintptr_t n = w(edi, 0x4c);
	uintptr_t tbl = w69950(self, w(w(edi, 0x58), 0x1d0));
	for (;;) {
		if (n == 0) { fatal(0x2b1); return; }
		if (w(w(n, 0x10), 0) == w(tbl, (a2 >> 0x14 & 0x3f) * 4)) {
			uint8_t c = (uint8_t)(a2 >> 8);
			if ((c != 0 && c != 4) ||
				w69090(edi, iw(a1, 8) + iw(edi, 0x34), iw(edi, 0x38) + iw(a1, 0xc), a3) != 0) {
				w68800(a3, a3);
				w(a1, 0x20) = w(n, 0xc);
				uintptr_t q = w(n, 0x10);
				w(n, 0xc) = (uint32_t)a1;
				if (q == 0) { fatal(0x2cb); return; }
				if (w(q, 0x24) != 0) {
					int32_t k = iw(q, 0x28);
					uint32_t u = w68b90(edi, a3);
					uintptr_t r = w68f00(edi, n + 0x14, iw(a1, 8) + iw(edi, 0x34), iw(edi, 0x38) + iw(a1, 0xc),
						(uint32_t)k << 8 | a2, u, a3);
					if (r != 0) w(r, 0x14) |= 8;
				}
			}
			return;
		}
		n = w(n, 4);
	}
}

// 1.13c D2Common+0x697C0: dispatch; w68780 yields a node (or 0) and an out value; node -> 0x695D0, else 0x69470.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_697c0(uint32_t a1, uintptr_t a2, uintptr_t a3, uint32_t ebx, uint32_t edi, uint32_t esi,
	uintptr_t (*w68780)(uintptr_t, uint32_t, uint32_t, uint32_t, uintptr_t*),
	void (*w695d0)(uintptr_t, uint32_t, uintptr_t, uintptr_t, uint32_t, uint32_t, uint32_t),
	void (*w69470)(uintptr_t, uint32_t, uint32_t, uint32_t, uint32_t)) {
	uint32_t isNull = a3 == 0;
	uintptr_t out = 0;
	uintptr_t node = w68780(a2, isNull, edi, esi, &out);
	if (node != 0) { w695d0(node, a1, a2, out, ebx, edi, esi); return; }
	w69470(a2, a1, ebx, edi, esi);
}

// 1.13c D2Common+0x816C0: (ecx=x, edx=roomA, eax=y, a3=roomB). If (x,y) is inside roomA's rect (+0x34/38,
// size +0x3c/40) fetch both tiles via 0x810A0; when both have +0x28 set, same +0x58->+0x1d0, differing
// +0x28 and equal +0x20, call 0x80EE0(roomB, tileB+0x28, tileA+0x28, tileA+0x20).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_816c0(int32_t x, uintptr_t roomA, int32_t y, uintptr_t roomB,
	uintptr_t (*w810a0)(int32_t, uintptr_t, int32_t), void (*w80ee0)(uintptr_t, uint32_t, uint32_t, uint32_t)) {
	if (!(iw(roomA, 0x34) <= x && iw(roomA, 0x38) <= y &&
		x <= iw(roomA, 0x3c) + iw(roomA, 0x34) && y <= iw(roomA, 0x40) + iw(roomA, 0x38))) return;
	uintptr_t a = w810a0(y, roomA, x), b = w810a0(y, roomB, x);
	uint32_t ta = w(a, 0x28);
	if (ta == 0) return;
	uint32_t tb = w(b, 0x28);
	if (tb == 0) return;
	if (w(w(roomA, 0x58), 0x1d0) != w(w(roomB, 0x58), 0x1d0)) return;
	if (ta != tb && w(a, 0x20) == w(b, 0x20)) w80ee0(roomB, tb, ta, w(a, 0x20));
}

// 1.13c D2Common+0x817A0: recursive flood over a tile grid. ctx (4-byte ptr cells): [0]=room, [1]=flag grid,
// [2]=optional type grid, [5]=visited grid; grids are {rows base, row-offset table}. dirXY = DAT_6fdda80c
// ({dx,dy} x4), cls = DAT_6fdda758, mask = DAT_6fdda7ac (5 dwords per class); cb = PTR_FUN_6fdeb244.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_817a0(uintptr_t ctx, int32_t x, int32_t y, int32_t dir,
	const int32_t* dirXY, const int32_t* cls, const uint32_t* mask, void (*cb)()) {
	for (;;) {
		uintptr_t room = w(ctx, 0);
		int32_t ny = iw(room, 0x38) + y, nx = iw(room, 0x34) + x;
		if (nx < iw(room, 0x34) || ny < iw(room, 0x38)) return;
		if (iw(room, 0x3c) + iw(room, 0x34) < nx) return;
		if (iw(room, 0x40) + iw(room, 0x38) < ny) return;
		uintptr_t g1 = w(ctx, 4), g5 = w(ctx, 0x14), g2 = w(ctx, 8);
		if ((w(w(g1, 0), ((uint32_t)iw(w(g1, 4), y * 4) + x) * 4) & 0x10000000) != 0) return;
		if (w(w(g5, 0), ((uint32_t)iw(w(g5, 4), y * 4) + x) * 4) == 0) {
			cb();
			for (int32_t i = 0; i < 4; ++i)
				d2_fn_d2common_817a0(ctx, dirXY[i * 2] + x, dirXY[i * 2 + 1] + y, i, dirXY, cls, mask, cb);
			return;
		}
		int32_t t = 0;
		if (g2 != 0 && w(g2, 0) != 0)
			t = iw(w(g2, 0), ((uint32_t)iw(w(g2, 4), y * 4) + x) * 4);
		uint32_t m = mask[dir + cls[t] * 5];
		if (m & 1) cb();
		if ((m & 2) && dir != 2) d2_fn_d2common_817a0(ctx, x + 1, y, 0, dirXY, cls, mask, cb);
		if ((m & 4) && dir != 3) d2_fn_d2common_817a0(ctx, x, y + 1, 1, dirXY, cls, mask, cb);
		if ((m & 8) && dir != 0) d2_fn_d2common_817a0(ctx, x - 1, y, 2, dirXY, cls, mask, cb);
		if ((m & 0x10) && dir != 1) d2_fn_d2common_817a0(ctx, x, y - 1, 3, dirXY, cls, mask, cb);
		if ((m & 0x20) == 0) return;
		y += 1; dir = -1; x += 1;
	}
}
}
