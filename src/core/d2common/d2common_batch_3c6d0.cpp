// D2Common 1.13c batch: DoNothing916 (RVA 0x3C6D0).
#include <cstdint>
#include <emscripten.h>

// 0x819C0 has C++ linkage in d2common_batch_45aa0.cpp; same signature here.
__attribute__((weak)) uint32_t d2_fn_d2common_819c0(int32_t nSubY, uint32_t pRoom, int32_t nSubX);

namespace
{
// Retail FUN_6fd51330 (RVA 0x1330): no-argument accessor returning a record whose +0x10 is a room.
// Host-installed seam; null leaves the function inert.
uint32_t (*env_3c6d0_GetRecord)(void) = nullptr;
}

extern "C" {
// 1.13c D2Common+0x3C6D0: look up a room tile at (x/5, y/5); result discarded.
EMSCRIPTEN_KEEPALIVE void D2Common_DoNothing916(uint32_t, int32_t nX, int32_t nY)
{
	const uint32_t pRec = env_3c6d0_GetRecord ? env_3c6d0_GetRecord() : 0;
	if (pRec == 0 || !d2_fn_d2common_819c0)
		return;
	// record+0x10 is a 32-bit retail pointer
	const uint32_t pRoom = *reinterpret_cast<const uint32_t*>(static_cast<intptr_t>(pRec) + 0x10);
	d2_fn_d2common_819c0(nY / 5, pRoom, nX / 5);
}
}
