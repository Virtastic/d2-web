// D2Common 0x816C0: link the tiles of two rooms at (x, y) (x ECX, roomA EDX, y EAX, roomB on the stack).
#pragma once
#include <cstdint>
struct D2Common816c0Env {
	uintptr_t (*tile)(int32_t y, uintptr_t room, int32_t x);                        // 0x810A0: ECX = y, EDX = room, stack = x
	void (*link)(uintptr_t roomB, uint32_t tileB28, uint32_t tileA28, uint32_t grp); // 0x80EE0: (roomB, tileB+0x28, tileA+0x28, tileA+0x20)
};
extern "C" void D2Common_816C0_LinkRoomTiles(int32_t x, uintptr_t roomA, int32_t y, uintptr_t roomB, const D2Common816c0Env* env);
