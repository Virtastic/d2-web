// 1.13c D2Common+0x12450 (stdcall, ret 8): non-zero when the second unit's room list holds the first unit's room.
// The room of a unit (0x2FE10) is [[unit+0x2C]] for types 2, 4, 5 and [[unit+0x2C]+0x1C] otherwise; the list and count (0x3BA20) are room +0 and +0x24.
// Null units are retail's fatal exit and are not modelled.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

static uint32_t RoomOf(uint32_t pUnit)
{
	const uint32_t t = D(pUnit);
	const uint32_t p = D(pUnit + 0x2C);
	if (t == 2 || (t >= 4 && t <= 5))
		return D(p);
	return p ? D(p + 0x1C) : 0;
}

extern "C"
uint32_t d2common_12450_port(uint32_t pUnitA, uint32_t pUnitB) // 1.13c D2Common+0x12450
{
	if (!pUnitA || !pUnitB)
		return 0;
	const uint32_t pListRoom = RoomOf(pUnitB);
	const uint32_t pRoom = RoomOf(pUnitA);
	if (!pListRoom || !pRoom)
		return 0;
	const int32_t n = static_cast<int32_t>(D(pListRoom + 0x24));
	const uint32_t pList = D(pListRoom);
	for (int32_t i = 0; i < n; ++i)
		if (D(pList + static_cast<uint32_t>(i) * 4) == pRoom)
			return 1;
	return 0;
}
