// 1.13c D2Common+0x69820 (sub_69820): room-tile placement dispatcher. Retail layout: EAX = tile flag word (fl); stack = p1, room, kind, p4, p5, p6 (ret 0x18).
// Every callee is injectable with its retail register layout (the Raw export takes the seam table; the probe binds hook recorders):
//   0x68970 EAX=p1 ECX=room, stack 0, fl, p5, kind    0x69090 EAX=fl ECX=room, stack p4, p5, kind   0x691C0 EAX=fl, stack room, p4, p5, kind
//   0x68780 EAX=fl, stack room, 1, p4, p5, &out       0x695D0 EAX=type ECX=node, stack p1, room, out, fl, p4, p5
//   0x9222 (Fog alloc) ECX=p1, EDX=12, stack "RoomTile.cpp", 0x37B, 0     0x68B90 EAX=fl ECX=room, stack kind
//   0x69170 EAX=room ECX=node+4, stack p4, p5, fl, u  0x697C0 EBX=fl ESI=p5 EDI=p4, stack p1, room, kind
//   0x44740 EAX=rect ESI=x EDI=y (point in rect)      0x68DF0 EAX=room ECX=p5 EDX=p4 ESI=slot, stack fl, t
//   0x68F00 ECX=room EDX=out, stack p4, p5, fl, t, kind   0x69360 EDI=room, stack r, fl, kind   0x68CD0 EBX=fl ECX=room EDX=p4, stack 0, p5, t
#include <cstdint>

struct D2Common69820Env
{
	void (*f68970)(uint32_t eaxP1, uintptr_t ecxRoom, uint32_t zero, uint32_t fl, int32_t p5, uint32_t kind);
	void (*f69090)(uint32_t eaxFl, uintptr_t ecxRoom, int32_t p4, int32_t p5, uint32_t kind);
	void (*f691c0)(uint32_t eaxFl, uintptr_t room, int32_t p4, int32_t p5, uint32_t kind);
	uintptr_t (*f68780)(uint32_t eaxFl, uintptr_t room, uint32_t one, int32_t p4, int32_t p5, uint32_t* out);
	void (*f695d0)(uint32_t eaxType, uintptr_t ecxNode, uint32_t p1, uintptr_t room, uint32_t out, uint32_t fl, int32_t p4, int32_t p5);
	uintptr_t (*alloc)(uint32_t pool, uint32_t size, uint32_t srcName, int32_t line, int32_t zero);
	uint32_t (*f68b90)(uint32_t eaxFl, uintptr_t ecxRoom, uint32_t kind);
	uintptr_t (*f69170)(uintptr_t eaxRoom, uintptr_t ecxNode4, int32_t p4, int32_t p5, uint32_t fl, uint32_t u);
	void (*f697c0)(uint32_t ebxFl, int32_t esiP5, int32_t ediP4, uint32_t p1, uintptr_t room, uint32_t kind);
	int32_t (*f44740)(uintptr_t eaxRect, int32_t esiX, int32_t ediY);
	void (*f68df0)(uintptr_t eaxRoom, int32_t ecxP5, int32_t edxP4, uintptr_t esiSlot, uint32_t fl, uint32_t t);
	uintptr_t (*f68f00)(uintptr_t ecxRoom, uintptr_t edxOut, int32_t p4, int32_t p5, uint32_t fl, uint32_t t, uint32_t kind);
	void (*f69360)(uintptr_t ediRoom, uintptr_t r, uint32_t fl, uint32_t kind);
	void (*f68cd0)(uint32_t ebxFl, uintptr_t ecxRoom, int32_t edxP4, uint32_t zero, int32_t p5, uint32_t t);
};

namespace {
inline uint32_t& w(uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(a + o); }
}

extern "C" void D2Common_69820_Raw(uint32_t fl, uint32_t p1, uintptr_t room, uint32_t kind, int32_t p4, int32_t p5, int32_t p6, const D2Common69820Env* env)
{
	const uint32_t bv = (fl >> 0x14) & 0x3f;
	const uint8_t c9 = static_cast<uint8_t>(fl >> 8);
	const bool special = bv == 0x1e && (c9 == 0 || c9 == 1);
	if ((kind == 0xb || kind == 0xa) && bv >= 8)
		return;
	if (kind == 0 && special)
		fl |= 0x80000000u;
	const bool hi = (fl & 0x80000000u) != 0;
	if (hi) {
		if (kind == 8 || kind == 9) {
			const int32_t lvl = static_cast<int32_t>(w(w(room, 0x58), 0x1d0));
			if (lvl < 0x6f || (lvl > 0x70 && lvl != 0x75)) {
				env->f68970(p1, room, 0, fl, p5, kind);
				return;
			}
		} else if (kind == 0xa || kind == 0xb) {
			if (bv >= 8)
				return;
			env->f69090(fl, room, p4, p5, kind);
			env->f691c0(fl, room, p4, p5, kind);
			return;
		}
	}
	// bump: claim the next 0x30-byte slot of the room's slot table (room+0x54: count at +0xc, base at +0x1c)
	auto bump = [&](uint32_t nfl, uint32_t t) {
		const uintptr_t r54 = w(room, 0x54);
		const uintptr_t slot = w(r54, 0xc) * 0x30 + w(r54, 0x1c);
		w(slot, 0x20) = 0;
		w(r54, 0xc) = w(r54, 0xc) + 1;
		env->f68df0(room, p5, p4, slot, nfl, t);
	};
	if (fl & 4) {
		if (fl & 2) {
			if (special)
				fl &= 0xffffff7fu;
			uint32_t out = 0;
			const uintptr_t node = env->f68780(fl, room, 1, p4, p5, &out);
			if (node != 0) {
				env->f695d0(0, node, p1, room, out, fl, p4, p5);
				return;
			}
			uintptr_t n = w(w(room, 0x54), 0);
			for (; n != 0; n = w(n, 8))
				if (w(n, 0) != 0)
					break;
			if (n == 0) {
				n = env->alloc(p1, 12, 0xB2000000u | 0x900F0u, 0x37b, 0);
				w(n, 0) = 1;
				w(n, 4) = 0;
				w(n, 8) = w(w(room, 0x54), 0);
				w(w(room, 0x54), 0) = static_cast<uint32_t>(n);
			}
			const uint32_t u = env->f68b90(fl, room, 0);
			env->f69170(room, n + 4, p4, p5, fl, u);
			return;
		}
		if (fl & 1) {
			env->f697c0(fl, p5, p4, p1, room, kind);
			return;
		}
		if ((fl & 0x8000000u) && !hi) {
			env->f697c0(fl, p5, p4, p1, room, 0xd);
			return;
		}
	}
	if (fl & 2) {
		bump(fl, env->f68b90(fl, room, 0));
	} else if (p6 != 0) {
		if (env->f44740(room + 0x34, p4, p5)) {
			const uint32_t lvl = w(w(room, 0x58), 0x1d0);
			const uint32_t nfl = (fl & 0xffffff7fu) | 0x80000000u;
			const uint32_t t = env->f68b90(lvl == 0x4a ? 0x1e00100u : 0x1e00000u, room, 0);
			bump(nfl, t);
		}
	}
	if (fl & 1) {
		const uint32_t t = env->f68b90(fl, room, kind);
		const uintptr_t r = env->f68f00(room, 0, p4, p5, fl, t, kind);
		if ((kind == 0xb || kind == 0xa) && w(w(room, 0x58), 0x1d0) != 0x85)
			env->f69360(room, r, fl, kind);
	}
	if (fl & 0x8000000u)
		env->f68cd0(fl, room, p4, 0, p5, env->f68b90(fl, room, 0xd));
}
