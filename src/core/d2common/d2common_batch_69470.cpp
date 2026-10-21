// D2Common 1.13c implementation batch (DRLG room tile add). Register inputs explicit; retail callees and
// the source-tagged allocator (ordinal 10045) are seams.
#include <cstdint>
#include <emscripten.h>

namespace {
inline uint8_t* p(uintptr_t a) { return reinterpret_cast<uint8_t*>(a); }
inline uint32_t& w(uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
inline int32_t& iw(uintptr_t a, int o) { return *reinterpret_cast<int32_t*>(p(a) + o); }
}

extern "C" {
// 1.13c D2Common+0x69470: (ecx=self room ctx, eax=type, stack p1..p4). Types 10/11 are rejected when
// (p3,p4) lies outside the rect at self+0x34/0x38 (size +0x3c/0x40). Finds the node in the (self+0x54)[0]
// list whose [0] flag equals (type==0), else allocates one ({flag,0,next}) and pushes it. Then dispatches
// to 0x69170 (type 0), 0x68CD0 (type 13) or 0x68F00 (+0x69360 for types 10/11); returns the callee result.
EMSCRIPTEN_KEEPALIVE uintptr_t d2_fn_d2common_69470(uintptr_t self, uint32_t type, uint32_t p1_unused, uint32_t p2, int32_t p3, int32_t p4,
	uintptr_t (*alloc10045)(const char*, int32_t, int32_t), uint32_t (*w68b90)(uintptr_t, uint32_t),
	uintptr_t (*w69170)(uintptr_t, int32_t, int32_t, uint32_t, uint32_t),
	uintptr_t (*w68cd0)(uintptr_t, int32_t, uintptr_t, int32_t, uint32_t),
	void (*w68f00)(uintptr_t, uintptr_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t),
	void (*w69360)(uintptr_t, uintptr_t, uint32_t, uint32_t)) {
	if ((type == 0xb || type == 0xa) &&
		(p3 < iw(self, 0x34) || p4 < iw(self, 0x38) || iw(self, 0x3c) + iw(self, 0x34) <= p3 ||
		 iw(self, 0x40) + iw(self, 0x38) <= p4))
		return 0;
	uint32_t flag = type == 0;
	uintptr_t head = w(w(self, 0x54), 0);
	uintptr_t n = head;
	for (; n != 0; n = w(n, 8))
		if ((w(n, 0) == 0) == (flag == 0)) break; // found iff (*n==0)==(flag==0)
	if (n == 0) {
		n = alloc10045("..\\Source\\D2Common\\DRLG\\RoomTile.cpp", 0x37b, 0);
		w(n, 0) = flag;
		w(n, 4) = 0;
		w(n, 8) = w(w(self, 0x54), 0);
		w(w(self, 0x54), 0) = (uint32_t)n;
	}
	uint32_t u = w68b90(self, type);
	if (type == 0) return w69170(n + 4, p3, p4, p2, u);
	if (type == 0xd) return w68cd0(self, p3, n + 4, p4, u);
	w68f00(self, n + 4, p3, p4, p2, u, type);
	uintptr_t r = w(n, 4);
	if (type == 0xb || type == 0xa) w69360(self, r, p2, type);
	return r;
}
}
