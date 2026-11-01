// 1.13c D2Common+0x0E1A0 (#10075) as a register-convention wrapper: EAX = pPath is forwarded with the
// tile pointer and the packed (nLo, nHi) coordinates to the repositioner at 0xDDC0, then the
// path's +0x28 dword is cleared and 1 returned. Argument 1 is unused. The 0xDDC0 body is its own row;
// this seam keeps the retail argument layout.
#include <cstdint>
#include <emscripten.h>

struct D2CommonE1A0Env
{
	void (*reposition)(uint32_t pPath, uint32_t pTile, uint32_t nCoords);
};

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2common_e1a0_port(uint32_t pPath, uint32_t unused1, uint32_t pTile,
                                                uint16_t nLo, uint16_t nHi, const D2CommonE1A0Env* pEnv)
{
	(void)unused1;
	if (!pEnv || !pEnv->reposition)
		return 0;
	pEnv->reposition(pPath, pTile, (static_cast<uint32_t>(nHi) << 16) | nLo);
	*reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pPath) + 0x28) = 0;
	return 1;
}
