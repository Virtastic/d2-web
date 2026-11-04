// D2Common 1.13c: the room-cell lookup at a world position (RVA 0x3C6D0).
#include <cstdint>
#include <emscripten.h>

// 0x819C0 lives in d2common_batch_45aa0.cpp under the _batch name (the FN probe owns the plain one).
extern "C" __attribute__((weak)) uint32_t d2_fn_d2common_819c0_batch(int32_t nSubY, uint32_t pRoom, int32_t nSubX);

// Seams for the two callees. room_at is 0x1330 (EAX = room, EBX = x, EDI = y: the room that holds the
// position, or 0); cell is 0x819C0 (ECX = y / 5, EDX = the found room's +0x10 word, stack = x / 5).
// A null cell seam falls back to the batch port of 0x819C0; a null room_at answers 0.
struct D2Common3C6D0Env
{
	uint32_t (*room_at)(uint32_t pRoom, int32_t nX, int32_t nY);
	uint32_t (*cell)(int32_t nSubY, uint32_t pRoomEx, int32_t nSubX);
};

extern "C" {
// 1.13c D2Common+0x3C6D0 (#10916): stdcall (room, x, y), ret 0xC. No room at the position answers 0;
// otherwise the answer is the cell lookup at the subtile (x / 5, y / 5), both divisions truncating.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_DoNothing916(uint32_t pRoom, int32_t nX, int32_t nY, const D2Common3C6D0Env* pEnv)
{
	const uint32_t pFound = pEnv && pEnv->room_at ? pEnv->room_at(pRoom, nX, nY) : 0;
	if (pFound == 0)
		return 0;
	// the found room's +0x10 is a 32-bit retail pointer
	const uint32_t pRoomEx = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pFound) + 0x10);
	if (pEnv->cell)
		return pEnv->cell(nY / 5, pRoomEx, nX / 5);
	return d2_fn_d2common_819c0_batch ? d2_fn_d2common_819c0_batch(nY / 5, pRoomEx, nX / 5) : 0;
}
}
