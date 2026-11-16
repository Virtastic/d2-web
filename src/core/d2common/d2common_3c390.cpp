// 1.13c D2Common+0x3C390 (#10057, DUNGEON_IsRoomInTown): stdcall(pRoom), ret 4. A null room answers 0 (the argument left in
// EAX); otherwise the room's level id (room +0x10 -> room-ex +0x58 -> level +0x1D0, 32-bit retail layout read through raw
// dwords) goes in EAX to DRLG_IsTownLevel (0x2D320: 1 for levels 1, 0x28, 0x4B, 0x67, 0x6D), whose answer is returned.
#include <cstdint>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"

namespace { uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a + o)), 4); return v; } }

// 1.13c D2Common+0x3C390
extern "C" uint32_t D2Common_3C390_IsRoomInTown(uint32_t pRoom)
{
    if (!pRoom) return 0;
    return static_cast<uint32_t>(DRLG_IsTownLevel(static_cast<int32_t>(rd32(rd32(rd32(pRoom, 0x10), 0x58), 0x1D0))));
}
