// 1.13c D2Common+0x10010: around every room of the level but one, try a new room in each of the directions 0..7 and stamp each
// that fits with the given preset (EAX = level; stack preset, room to skip; ret 8). The room list is first copied into a Fog pool
// array (0x9222 / 0x9228 on the level's pool, Maze.cpp lines 0x271 / 0x288). Raw retail 32-bit layout; callees are the maze env.
// The decompile models the pool alloc as not popping its stack words and so reads the skip room as the pool; the disassembly
// compares each room with the second stack argument.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

extern "C"
void d2common_10010_port(uint32_t pLevel, uint32_t nPreset, uint32_t pSkip, const D2CommonMazeEnv* e)
{
	const uint32_t pPool = D(D(pLevel + 0x1B4) + 0x478);
	const uint32_t pArr = e->fog_alloc(pPool, D(pLevel + 8) << 2);
	const int32_t n = static_cast<int32_t>(D(pLevel + 8));
	uint32_t r = D(pLevel + 0x10);
	for (int32_t i = 0; i < n; ++i)
	{
		D(pArr + 4 * i) = r;
		r = D(r + 0x24);
	}
	for (int32_t i = 0; i < n; ++i)
	{
		if (D(pArr + 4 * i) == pSkip)
			continue;
		for (uint32_t nDir = 0; nDir < 8; ++nDir)
		{
			const uint32_t pRoom = D(pArr + 4 * i);
			const uint32_t t = NewRoom(e, pRoom);
			if (!e->link(nDir, pRoom, t))
			{
				e->free_room(t);
				continue;
			}
			e->adjust(t, pRoom, nDir);
			e->adjust(pRoom, t, (nDir - 2) & 3);
			Enlist(pRoom, t);
			const uint32_t pDef = D(t + 0x20);
			D(pDef) = nPreset;
			const uint32_t f = D(pDef + 0xC) | 2;
			D(pDef + 4) = 0xFFFFFFFFu;
			D(pDef + 0xC) = f;
		}
	}
	e->fog_free(pPool, pArr);
}
