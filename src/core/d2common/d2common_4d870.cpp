// 1.13c D2Common+0x4D870 (#10078): stdcall forwarder over 0x4D140 (FindFreeSpot, 9 stdcall words); the callee is injected so a
// replay probe can record what it is handed.
#include <cstdint>
struct D2ActiveRoomStrc;
struct D2CoordStrc;
// 0x4D140 (COLLISION_GetFreeCoordinatesImpl): room, spawn point, field point, unit size, mask, field mask, neighbours, max distance, step.
typedef D2ActiveRoomStrc* (*D2Common4D140Fn)(D2ActiveRoomStrc*, D2CoordStrc*, D2CoordStrc*, int32_t, uint32_t, uint32_t, int32_t, int32_t, int32_t);
extern "C" D2ActiveRoomStrc* D2Common_4D870_Forward9_Seam(D2Common4D140Fn f, D2ActiveRoomStrc* pRoom, D2CoordStrc* pSpawn,
                                                         int32_t nSize, uint32_t nMask, int32_t bNeighbours, int32_t nMaxDistance)
{
    return f(pRoom, pSpawn, nullptr, nSize, nMask, nMask, bNeighbours, nMaxDistance, 1);
}
