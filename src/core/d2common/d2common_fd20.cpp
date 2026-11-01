// 1.13c D2Common+0x0FD20: grow three rooms around the level's first room, turning the direction each time, then stamp the
// first room's preset for level kinds 0x11 / 0x22 (stdcall level; ret 4). Raw retail 32-bit layout; callees are the maze env.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

namespace { const uint32_t kPreset[4][2] = { { 0x1BF, 0x433 }, { 0x1BC, 0x435 }, { 0x1BE, 0x434 }, { 0x1BD, 0x432 } }; }

extern "C"
void d2common_fd20_port(uint32_t pLevel, const D2CommonMazeEnv* e)
{
	const uint32_t r0 = D(pLevel + 0x10);
	int32_t nDir = static_cast<int32_t>(Lcg(pLevel + 0x1C4) & 3);
	for (int i = 0; i < 3; ++i)
	{
		const uint32_t t = NewRoom(e, r0);
		if (!e->link(static_cast<uint32_t>(nDir), r0, t))
			e->free_room(t);
		else
		{
			e->adjust(t, r0, static_cast<uint32_t>(nDir));
			e->adjust(r0, t, static_cast<uint32_t>(nDir - 2) & 3);
			e->scatter(t);
			Enlist(r0, t);
			e->pick(r0, 1);
			e->pick(t, 1);
		}
		nDir = NextDir(nDir);
	}
	const uint32_t nKind = D(pLevel + 0x1C0);
	if (nKind != 0x11 && nKind != 0x22)
		return;
	const uint32_t pDef = D(r0 + 0x20);
	D(pDef) = kPreset[nDir][nKind == 0x11 ? 0 : 1];
	const uint32_t f = D(pDef + 0xC) | 2;
	D(pDef + 4) = 0xFFFFFFFFu;
	D(pDef + 0xC) = f;
}
