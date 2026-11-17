// 1.13c D2Common+0x4D7E0 (#10654): stdcall forwarder over 0x4D140 (FindFreeSpot, 9 stdcall words); the callee is injected so a
// replay probe can record what it is handed.
#include <cstdint>
struct D2ActiveRoomStrc;
struct D2CoordStrc;
// 0x4D140 (COLLISION_GetFreeCoordinatesImpl): room, spawn point, field point, unit size, mask, field mask, neighbours, max distance, step.
typedef D2ActiveRoomStrc* (*D2Common4D140Fn)(D2ActiveRoomStrc*, D2CoordStrc*, D2CoordStrc*, int32_t, uint32_t, uint32_t, int32_t, int32_t, int32_t);
extern "C" D2ActiveRoomStrc* D2Common_4D7E0_FindFreeSpotR50_Seam(D2Common4D140Fn f, D2ActiveRoomStrc* pRoom, D2CoordStrc* pSpawn, D2CoordStrc* pField,
                                                                int32_t nSize, uint32_t nMask, uint32_t nFieldMask, int32_t bNeighbours)
{
    return f(pRoom, pSpawn, pField, nSize, nMask, nFieldMask, bNeighbours, 0x32, 1);
}
