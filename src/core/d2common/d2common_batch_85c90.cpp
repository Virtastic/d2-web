// 1.13c D2Common batch: +0x85C90, +0x85D20, +0x85D90, +0x85FD0. Seams for UNMAPPED callees.
#include <cstdint>
#include <emscripten.h>

typedef void (*D2CommonProcessIdsFn)(const uint32_t* pIds, int32_t nCount, int32_t nPayload);

// 1.13c D2Common+0x85C90: 0x85A80 over {0x19B,0x18F,0x18E,0x193} (payload 0), then over {0x18B}
// (payload 1) twice.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85C90_ProcessIds(D2CommonProcessIdsFn fnProcess)
{
	const uint32_t aA[4] = {0x19B, 0x18F, 0x18E, 0x193};
	const uint32_t aB[1] = {0x18B};
	fnProcess(aA, 4, 0);
	fnProcess(aB, 1, 1);
	fnProcess(aB, 1, 1);
}

// 1.13c D2Common+0x85D20: 0x85A80 over {0x18B,0x19B,0x190,0x18E} (payload 0), then over the tail
// {0x194,0x195,0x196,0x197} (payload 1).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85D20_ProcessIds(D2CommonProcessIdsFn fnProcess)
{
	const uint32_t aIds[8] = {0x18B, 0x19B, 0x190, 0x18E, 0x194, 0x195, 0x196, 0x197};
	fnProcess(aIds, 4, 0);
	fnProcess(aIds + 4, 4, 1);
}

// 1.13c D2Common+0x85D90: 0x85A80 over seven ids, payload 0.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85D90_ProcessIds(D2CommonProcessIdsFn fnProcess)
{
	const uint32_t aIds[7] = {0x18B, 0x19B, 0x191, 0x192, 0x18F, 0x18E, 0x193};
	fnProcess(aIds, 7, 0);
}

// 1.13c D2Common+0x85FD0: walk a 5x5 grid (index i: column i%5, row i/5, step 3 cells) and call
// 0x42A70 with the table entry for i, flag -1 and 0. The context register is passed through
// untouched; the 25-entry table (retail data at 0x9B748) is supplied by the caller.
typedef void (*D2CommonGridPlaceFn)(void* pCtx, int32_t nX, int32_t nY, uint32_t nValue, int32_t nFlag, int32_t nZero);

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85FD0_GridWalk(D2CommonGridPlaceFn fnPlace, void* pCtx,
	const uint32_t* pTable25)
{
	for (uint32_t i = 0; i < 25; ++i)
		fnPlace(pCtx, int32_t((i % 5) * 3), int32_t((i / 5) * 3), pTable25[i], -1, 0);
}
