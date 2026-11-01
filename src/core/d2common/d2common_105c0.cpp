// 1.13c D2Common+0x105C0: place one room with preset (a, b) next to the first room of the level that is not fixed and takes a
// new room in direction dir; returns it or null (EAX = level; stack dir, a, b; ret 0xC). Raw retail 32-bit layout; maze env.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

extern "C"
uint32_t d2common_105c0_port(uint32_t pLevel, uint32_t nDir, uint32_t nA, uint32_t nB, const D2CommonMazeEnv* e)
{
	for (uint32_t r = D(pLevel + 0x10); r; r = D(r + 0x24))
	{
		if (Fixed(r))
			continue;
		const uint32_t t = NewRoom(e, r);
		if (!e->link(nDir, r, t))
		{
			e->free_room(t);
			continue;
		}
		e->adjust(t, r, nDir);
		e->adjust(r, t, (nDir - 2) & 3);
		Enlist(r, t);
		e->pick(r, 1);
		const uint32_t pDef = D(t + 0x20);
		D(pDef + 4) = nB;
		const uint32_t f = D(pDef + 0xC) | 2;
		D(pDef) = nA;
		D(pDef + 0xC) = f;
		return t;
	}
	return 0;
}
