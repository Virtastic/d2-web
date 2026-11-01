// 1.13c D2Common+0x102F0: build a ring of rooms from the level's first room: count-1 steps in direction 1, then 0, then 3, then
// count-2 steps in direction 2, and close the ring back to the first room (EAX = level; stack count; ret 4). A step that does not
// fit leaves the chain at null (retail then reads through it). Raw retail 32-bit layout; callees are the maze env.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

extern "C"
void d2common_102f0_port(uint32_t pLevel, int32_t nCount, const D2CommonMazeEnv* e)
{
	const uint32_t pFirst = D(pLevel + 0x10);
	uint32_t cur = pFirst;
	static const uint32_t kDir[4] = { 1, 0, 3, 2 };
	for (int p = 0; p < 4; ++p)
	{
		const uint32_t nDir = kDir[p];
		for (int32_t i = p < 3 ? nCount - 1 : nCount - 2; i > 0; --i)
		{
			uint32_t t = NewRoom(e, cur);
			if (!e->link(nDir, cur, t))
			{
				e->free_room(t);
				t = 0;
			}
			else
			{
				e->adjust(t, cur, nDir);
				e->adjust(cur, t, (nDir + 2) & 3);
				e->scatter(t);
				Enlist(cur, t);
				e->pick(cur, 1);
				e->pick(t, 1);
			}
			cur = t;
		}
	}
	e->adjust(pFirst, cur, 2);
	e->adjust(cur, pFirst, 0);
	e->pick(cur, 1);
	e->pick(pFirst, 1);
}
