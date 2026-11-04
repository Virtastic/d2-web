// D2Common 0x816C0 port, written from the 1.13c decompile. Room fields are retail 32-bit words read at retail offsets.
#include <cstdint>
#include "d2common_816c0.h"

static inline int32_t rdi(uintptr_t a) { return *reinterpret_cast<const int32_t*>(a); }
static inline uint32_t rdu(uintptr_t a) { return *reinterpret_cast<const uint32_t*>(a); }

// 1.13c D2Common+0x816C0
extern "C" void D2Common_816C0_LinkRoomTiles(int32_t x, uintptr_t roomA, int32_t y, uintptr_t roomB, const D2Common816c0Env* env)
{
	if (!(rdi(roomA + 0x34) <= x && rdi(roomA + 0x38) <= y &&
	      x <= rdi(roomA + 0x3c) + rdi(roomA + 0x34) && y <= rdi(roomA + 0x40) + rdi(roomA + 0x38)))
		return;
	const uintptr_t tA = env->tile(y, roomA, x);
	const uintptr_t tB = env->tile(y, roomB, x);
	const uint32_t a28 = rdu(tA + 0x28);
	if (a28 == 0) return;
	const uint32_t b28 = rdu(tB + 0x28);
	if (b28 == 0) return;
	if (rdu(rdu(roomA + 0x58) + 0x1d0) != rdu(rdu(roomB + 0x58) + 0x1d0)) return;
	if (a28 != b28 && rdu(tA + 0x20) == rdu(tB + 0x20))
		env->link(roomB, b28, a28, rdu(tA + 0x20));
}
