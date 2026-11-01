// 1.13c D2Common+0x10AF0: roll the first room's LCG for one of four layouts {preset, dir, sub} x 2, place both rooms next to it
// (0xF000, no scatter), re-pick the first room and stamp each placed room, then ring every room with preset 0x344 (0x10010)
// (stdcall level; ret 4). Raw retail 32-bit layout; maze env. Retail's k*2 >= 7 fatal branch cannot be taken after &3.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

namespace
{
	const uint32_t kLayout[4][6] = {
		{ 0x41E, 1, 0, 0x41D, 3, 1 }, { 0x41E, 1, 1, 0x41D, 3, 0 }, { 0x41F, 0, 1, 0x420, 2, 0 }, { 0x41F, 0, 0, 0x420, 2, 1 } };
}

extern "C"
void d2common_10af0_port(uint32_t pLevel, const D2CommonMazeEnv* e)
{
	const uint32_t r0 = D(pLevel + 0x10);
	const uint32_t* p = kLayout[Lcg(r0 + 0x14) & 3];
	for (int i = 0; i < 2; ++i, p += 3)
	{
		const uint32_t t = e->f000(r0, p[1], 0);
		if (!t)
			continue;
		e->pick(r0, 1);
		const uint32_t pDef = D(t + 0x20);
		D(pDef) = p[0];
		const uint32_t f = D(pDef + 0xC) | 2;
		D(pDef + 4) = p[2];
		D(pDef + 0xC) = f;
	}
	e->f10010(pLevel, 0x344, 0);
}
