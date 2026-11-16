// 1.13c D2Common+0x69470 (sub_69470): room-tile add. Retail layout: EAX = type, ECX = room context, stack = pool, p2, x, y (ret 0x10).
// Types 10/11 are rejected when (x,y) lies outside the rect at ctx+0x34/0x38 (extent +0x3c/0x40). It looks in the list at (ctx+0x54)[0]
// (next link at +8) for a node whose first word is zero exactly when the type is nonzero; none found, it allocates a 12-byte node
// {type==0, 0, old head} through the Fog allocator (ECX = pool, EDX = 12, stack: "RoomTile.cpp", 0x37B, 0) and pushes it. Then it
// dispatches: type 0 to 0x69170, type 0xD to 0x68CD0, else 0x68F00 (+0x69360 for types 10/11) and returns node[1] (the 0x68F00 out value).
// Every callee and the allocator are injectable with retail's register layout.
#include <cstdint>

struct D2Common69470Env
{
	uintptr_t (*alloc)(uint32_t pool, uint32_t size, uint32_t srcName, int32_t line, int32_t zero);                  // Fog 10045 thunk 0x9222
	uint32_t (*f68b90)(uint32_t eaxP2, uintptr_t ecxSelf, uint32_t type);                                           // ret 4
	uintptr_t (*f69170)(uintptr_t eaxSelf, uintptr_t ecxNode4, int32_t x, int32_t y, uint32_t p2, uint32_t u);        // ret 0x10
	uintptr_t (*f68cd0)(uint32_t ebxP2, uintptr_t ecxSelf, int32_t edxX, uintptr_t node4, int32_t y, uint32_t u);     // ret 0xC
	void (*f68f00)(uintptr_t ecxSelf, uintptr_t edxNode4, int32_t x, int32_t y, uint32_t p2, uint32_t u, uint32_t type); // ret 0x14
	void (*f69360)(uintptr_t ediSelf, uintptr_t node1, uint32_t p2, uint32_t type);                                  // ret 0xC; EDI = self is an input (ECX is leftover, not)
};

namespace {
inline uint32_t& w(uintptr_t a, int o) { return *reinterpret_cast<uint32_t*>(a + o); }
inline int32_t& iw(uintptr_t a, int o) { return *reinterpret_cast<int32_t*>(a + o); }
}

extern "C" uintptr_t D2Common_69470_Raw(uint32_t type, uintptr_t self, uint32_t pool, uint32_t p2, int32_t x, int32_t y, const D2Common69470Env* env)
{
	if ((type == 0xb || type == 0xa) &&
	    (x < iw(self, 0x34) || y < iw(self, 0x38) || iw(self, 0x3c) + iw(self, 0x34) <= x || iw(self, 0x40) + iw(self, 0x38) <= y))
		return 0;
	const uint32_t flag = type == 0;
	uintptr_t n = w(w(self, 0x54), 0);
	for (; n != 0; n = w(n, 8))
		if ((w(n, 0) == 0) == (flag == 0))
			break;
	if (n == 0) {
		n = env->alloc(pool, 12, 0xB2000000u | 0x900F0u, 0x37b, 0);
		w(n, 0) = flag;
		w(n, 4) = 0;
		w(n, 8) = w(w(self, 0x54), 0);
		w(w(self, 0x54), 0) = static_cast<uint32_t>(n);
	}
	const uint32_t u = env->f68b90(p2, self, type);
	if (type == 0)
		return env->f69170(self, n + 4, x, y, p2, u);
	if (type == 0xd)
		return env->f68cd0(p2, self, x, n + 4, y, u);
	env->f68f00(self, n + 4, x, y, p2, u, type);
	const uintptr_t r = w(n, 4);
	if (type == 0xb || type == 0xa)
		env->f69360(self, r, p2, type);
	return r;
}
