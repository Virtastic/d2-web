// D2Common 1.13c implementation: room-tile placement dispatcher. eax carries the tile flag word (`fl`).
// Every callee (incl. sub_68DF0 and the Ordinal_10045 allocator) is a seam; the glue binds register args.
#include <cstdint>
#include <emscripten.h>

extern "C" {
struct D2C69820Seams {
	void (*f8970)(uintptr_t room, int32_t z, uint32_t fl, int32_t p5);
	void (*f9090)(uintptr_t room, int32_t p4, int32_t p5, int32_t kind);
	void (*f91c0)(uintptr_t room, int32_t p4, int32_t p5, int32_t kind);
	uintptr_t (*f8780)(uintptr_t room, int32_t one, int32_t p4, int32_t p5, int32_t* out);
	void (*f95d0)(uintptr_t a, uint32_t p1, uintptr_t room, int32_t o, uint32_t fl, int32_t p4, int32_t p5);
	uintptr_t (*alloc)(void); // Ordinal_10045(RoomTile.cpp,0x37b,0)
	uint32_t (*f8b90)(uintptr_t room, int32_t kind);
	void (*f9170)(uintptr_t node1, int32_t p4, int32_t p5, uint32_t fl, uint32_t t);
	void (*f97c0)(uint32_t p1, uintptr_t room, int32_t kind);
	int (*f4740)(void);
	void (*f8df0)(int32_t p5, int32_t p4, uint32_t fl, uint32_t t); // sub_68DF0, slot/room bound by glue
	uintptr_t (*f8f00)(uintptr_t room, int32_t z, int32_t p4, int32_t p5, uint32_t fl, uint32_t t, int32_t kind);
	void (*f9360)(uintptr_t r58, uintptr_t n, uint32_t fl, int32_t kind);
	void (*f8cd0)(uintptr_t room, int32_t p4, int32_t z, int32_t p5, uint32_t t);
};

// 1.13c D2Common+0x69820
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_69820(uint32_t fl, uint32_t p1, uintptr_t room, int32_t kind,
	int32_t p4, int32_t p5, int32_t p6, const D2C69820Seams* s) {
	auto I = [](uintptr_t a, uint32_t o) { return *reinterpret_cast<int32_t*>(a + o); };
	uint32_t bv = (fl >> 0x14) & 0x3f;
	uint8_t c9 = (uint8_t)(fl >> 8);
	int32_t k = kind;
	if ((k == 0xb || k == 0xa) && bv >= 8) return;
	if (k == 0 && bv == 0x1e && (c9 == 0 || c9 == 1)) fl |= 0x80000000u;
	if (fl & 0x80000000u) {
		if (k == 8 || k == 9) {
			int32_t v = I(I(room, 0x58), 0x1d0);
			if (v < 0x6f || (0x70 < v && v != 0x75)) { s->f8970(room, 0, fl, p5); return; }
		} else if (k == 0xa || k == 0xb) {
			if (bv > 7) return;
			s->f9090(room, p4, p5, k);
			s->f91c0(room, p4, p5, k);
			return;
		}
	}
	if (fl & 4) {
		if (fl & 2) {
			if (bv == 0x1e && (c9 == 0 || c9 == 1)) fl &= 0xffffff7fu;
			int32_t out = 0;
			uintptr_t n = s->f8780(room, 1, p4, p5, &out);
			if (n) { s->f95d0(n, p1, room, out, fl, p4, p5); return; }
			uintptr_t* head = reinterpret_cast<uintptr_t*>(I(room, 0x54));
			uintptr_t p = *head;
			for (; p; p = (uintptr_t)I(p, 8)) if (I(p, 0)) goto found;
			p = s->alloc();
			*reinterpret_cast<int32_t*>(p) = 1;
			*reinterpret_cast<int32_t*>(p + 4) = 0;
			*reinterpret_cast<int32_t*>(p + 8) = (int32_t)*head;
			*head = p;
		found:
			s->f9170(p + 4, p4, p5, fl, s->f8b90(room, 0));
			return;
		}
		if (fl & 1) { s->f97c0(p1, room, k); return; }
		if ((fl & 0x8000000) && !(fl & 0x80000000u)) { s->f97c0(p1, room, 0xd); return; }
	}
	auto bump = [&](uint32_t t, uint32_t f) {
		uintptr_t r54 = (uintptr_t)I(room, 0x54);
		*reinterpret_cast<int32_t*>((uintptr_t)(I(r54, 0xc) * 0x30 + I(r54, 0x1c) + 0x20)) = 0;
		*reinterpret_cast<int32_t*>(r54 + 0xc) = I(r54, 0xc) + 1;
		s->f8df0(p5, p4, f, t);
	};
	if (!(fl & 2)) {
		if (p6 != 0 && s->f4740()) bump(s->f8b90(room, 0), (fl & 0xffffff7fu) | 0x80000000u);
	} else {
		bump(s->f8b90(room, 0), fl);
	}
	if (fl & 1) {
		uint32_t t = s->f8b90(room, k);
		uintptr_t n = s->f8f00(room, 0, p4, p5, fl, t, k);
		if ((k == 0xb || k == 0xa) && I(I(room, 0x58), 0x1d0) != 0x85) s->f9360(I(room, 0x58), n, fl, k);
	}
	if (fl & 0x8000000) s->f8cd0(room, p4, 0, p5, s->f8b90(room, 0xd));
}
}
