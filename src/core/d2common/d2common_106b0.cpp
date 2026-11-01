// 1.13c D2Common+0x106B0: apply a preset record {key, a, b, dir}: restamp the first non-fixed room whose preset is key with (a, b),
// else place a new room with it (0x105C0); then step the caller's direction counter, if any, mod 4 (EDX = record; stack level,
// counter; ret 8). Raw retail 32-bit layout; maze env.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

extern "C"
void d2common_106b0_port(const int32_t* pRec, uint32_t pLevel, uint32_t pCounter, const D2CommonMazeEnv* e)
{
	const uint32_t nKey = static_cast<uint32_t>(pRec[0]), nA = static_cast<uint32_t>(pRec[1]), nB = static_cast<uint32_t>(pRec[2]);
	bool bFound = false;
	for (uint32_t r = D(pLevel + 0x10); r; r = D(r + 0x24))
	{
		if (Fixed(r) || D(D(r + 0x20)) != nKey)
			continue;
		const uint32_t pDef = D(r + 0x20);
		const uint32_t f = D(pDef + 0xC) | 2;
		D(pDef) = nA;
		D(pDef + 4) = nB;
		D(pDef + 0xC) = f;
		bFound = true;
		break;
	}
	if (!bFound)
		e->f105c0(pLevel, static_cast<uint32_t>(pRec[3]), nA, nB);
	if (pCounter)
		D(pCounter) = static_cast<uint32_t>(NextDir(static_cast<int32_t>(D(pCounter))));
}
