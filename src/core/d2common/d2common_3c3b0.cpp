// 1.13c D2Common+0x3C3B0: stdcall(room, unit), ret 8.
// Null room is fatal 0x487 and a zero unit id is fatal 0x488. 0x3B3A0 validates the room's unit array
// (a zero result is fatal 0x489). The count at room+0x78 is searched over the array at room+0x48; a miss is
// fatal 0x490. The found slot takes the last entry, the count drops by one, and 0x3BDD0 refreshes the room's
// client list. The refresh comes in through the updateClientList argument so a probe can pin it.
#include <cstdint>
#include <cstring>
#include "d2common_fatal.h"

extern "C" uint32_t D2Common_3B3A0_ValidateUnitIds(uint32_t room);

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
void W32(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
}

extern "C" uint32_t D2Common_3C3B0_RemoveRoomUnit(uint32_t pRoom, uint32_t nUnitId, uint32_t (*updateClientList)(uint32_t pRoom))
{
	if (!pRoom) d2common_fatal(0x487);
	if (!nUnitId) d2common_fatal(0x488);
	if (!D2Common_3B3A0_ValidateUnitIds(pRoom)) d2common_fatal(0x489);

	const uint32_t count = R32(pRoom + 0x78);
	const uint32_t arr = R32(pRoom + 0x48);
	uint32_t i = 0;
	while (i < count && R32(arr + 4 * i) != nUnitId) ++i;
	if (i >= count) d2common_fatal(0x490);

	W32(arr + 4 * i, R32(arr + 4 * (count - 1)));
	W32(pRoom + 0x78, count - 1);
	// retail leaves the refresh's EAX as the return value (tail of the function)
	return updateClientList(pRoom);
}
