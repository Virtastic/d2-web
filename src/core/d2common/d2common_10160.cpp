// 1.13c D2Common+0x10160: grow the level's maze to its room count (settings +4 indexed by the difficulty byte at +0x450 of the
// level's owner, x3 when the level is the owner's +0x94 id, x2 when it is its +0x484 id): pick a room by the level LCG (a mask for
// a power-of-two count, else the remainder), roll its own LCG for the direction and try a new room there (stdcall level; ret 4).
// Raw retail 32-bit layout; callees are the maze env.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

extern "C"
void d2common_10160_port(uint32_t pLevel, const D2CommonMazeEnv* e)
{
	const uint32_t pOwner = D(pLevel + 0x1B4);
	const uint32_t nDiff = *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(pOwner + 0x450));
	int32_t nTarget = static_cast<int32_t>(D(D(pLevel + 0x14) + 4 + nDiff * 4));
	const uint32_t nId = D(pLevel + 0x1D0);
	if (nId == D(pOwner + 0x94))
		nTarget *= 3;
	if (nId == D(D(pLevel + 0x1B4) + 0x484))
		nTarget *= 2;
	while (static_cast<int32_t>(D(pLevel + 8)) < nTarget)
	{
		const uint32_t n = D(pLevel + 8);
		uint32_t nIdx = 0;
		if (static_cast<int32_t>(n) > 0)
		{
			const uint32_t v = Lcg(pLevel + 0x1C4);
			nIdx = (n & (n - 1)) ? v % n : v & (n - 1);
		}
		uint32_t r = D(pLevel + 0x10);
		for (; nIdx; --nIdx)
			r = D(r + 0x24);
		const uint32_t nDir = Lcg(r + 0x14) & 3;
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
		e->scatter(t);
		Enlist(r, t);
		e->pick(r, 1);
		e->pick(t, 1);
	}
}
