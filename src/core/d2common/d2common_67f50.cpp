// 1.13c D2Common+0x67F50 (RoomTile.cpp) -- EAX = room, EDI = memory pool, plain ret: free the room's three tile arrays.
// t = room+0x54 (tile block); when t is set, the dwords at t+0x14, t+0x1C, t+0x24 that are non-null each go to Fog 10046
// (pool, pointer, file, line 0x592 / 0x593 / 0x594, 0). Nothing is written back; no result. Addresses are 32-bit.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
}

// 1.13c D2Common+0x67F50
extern "C" void D2Common_67F50_FreeRoomTileArrays(uint32_t room, uint32_t pool,
    void (*freePool)(uint32_t pool, uint32_t ptr, uint32_t line))
{
    const uint32_t t = rd32(room + 0x54);
    if (!t) return;
    for (uint32_t i = 0; i < 3; ++i) {
        const uint32_t p = rd32(t + 0x14 + 8 * i);
        if (p) freePool(pool, p, 0x592 + i);
    }
}
