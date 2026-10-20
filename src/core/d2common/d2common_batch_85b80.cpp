// 1.13c D2Common batch 85b80: +0x85B80, +0x85BE0, +0x85C50. Each hands a table of preset ids to
// 0x85A80 (DRLGOUTDESR_PlacePresetVariants; level arrives in a register the decompile hides), so
// the callee is a seam, as in D2Common_85CF0_ProcessTwoIds.
#include <cstdint>
#include <emscripten.h>

typedef void (*D2CommonPlaceIdsFn)(const uint32_t* pIds, int32_t nCount, int32_t nFlag);

// 1.13c D2Common+0x85B80: {0x191,0x192,0x196,0x197,0x193} flag 0, then {0x188,0x189} flag 1.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85B80_PlaceIds(D2CommonPlaceIdsFn fn)
{
	const uint32_t a[5] = {0x191, 0x192, 0x196, 0x197, 0x193};
	const uint32_t b[2] = {0x188, 0x189};
	fn(a, 5, 0);
	fn(b, 2, 1);
}

// 1.13c D2Common+0x85BE0: {0x18B,400,0x18E,0x194,0x195} flag 0, then {0x19B} flag 1 -- twice, as retail does.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85BE0_PlaceIds(D2CommonPlaceIdsFn fn)
{
	const uint32_t a[5] = {0x18B, 400, 0x18E, 0x194, 0x195};
	const uint32_t b[1] = {0x19B};
	fn(a, 5, 0);
	fn(b, 1, 1);
	fn(b, 1, 1);
}

// 1.13c D2Common+0x85C50: {0x19D,0x198,0x199,0x19A} flag 0.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85C50_PlaceIds(D2CommonPlaceIdsFn fn)
{
	const uint32_t a[4] = {0x19D, 0x198, 0x199, 0x19A};
	fn(a, 4, 0);
}
