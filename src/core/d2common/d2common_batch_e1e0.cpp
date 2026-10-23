// 1.13c D2Common implementation batch: +0xE1E0.
// Thin wrapper: hand the room, an extra pointer and a packed 32-bit coordinate pair to the tile
// updater at 0xDF30 and, on success, clear the room's +0x28 dword.
#include <cstdint>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// The callee at 0xDF30 receives the room pointer in EAX, the extra pointer and the packed
// coordinates on the stack. That layout is carried by the seam signature.
struct D2CommonE1E0Env
{
	int32_t (*update_tile)(uint32_t pRoom, uint32_t pArg, uint32_t nCoords);
};

// 1.13c D2Common+0xE1E0 (#10533): update one room tile and reset the room's pending dword.
// param_2 is unused by the body. The two 16-bit coordinates pack as (nHi << 16) | nLo.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t D2Common_RoomUpdateTileAndClearPending(uint32_t pRoom, uint32_t unused2, uint32_t pArg,
                                                 uint16_t nLo, uint16_t nHi, const D2CommonE1E0Env* pEnv)
{
	(void)unused2;
	if (!pEnv || !pEnv->update_tile)
		return 0;

	const uint32_t nCoords = (static_cast<uint32_t>(nHi) << 16) | nLo;
	if (!pEnv->update_tile(pRoom, pArg, nCoords))
		return 0;

	W(P(pRoom), 0x28) = 0;
	return 1;
}
