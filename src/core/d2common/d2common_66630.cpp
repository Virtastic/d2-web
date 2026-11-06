// Room membership test for a unit: the unit's tile must lie inside the room's box, and the unit must not
// already be on the room's unit list unless the caller allows it. Retail passes the room in ESI and the
// unit in EDI (one stack word, ret 4); here they are explicit. The room-coords fill and the fatal exit are seams.
#include <cstdint>
#include <cstring>

struct D2RoomUnitCheckSeam {
    void (*pfnRoomCoords)(uint32_t pRoom, uint32_t pCoords);   // fills an 8-dword block (result unused)
    void (*pfnFatal)(uint32_t nLine);                          // never returns in retail
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t R16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
}

// 1.13c D2Common+0x66630
extern "C" int32_t D2Common_66630_RoomUnitCheck(const D2RoomUnitCheckSeam* s, uint32_t pRoom, uint32_t pUnit, int32_t bInList)
{
    uint32_t aCoords[8];
    s->pfnRoomCoords(pRoom, (uint32_t)(uintptr_t)aCoords);
    const int32_t nType = (int32_t)R32(pUnit);
    const uint32_t pPath = R32(pUnit + 0x2c);
    const bool bDynamic = nType == 2 || (nType > 3 && nType <= 5);
    int32_t x, y;
    if (bDynamic) { x = (int32_t)R32(pPath + 0xc); y = (int32_t)R32(pPath + 0x10); }
    else { x = pPath ? (int32_t)R16(pPath + 2) : 0; y = pPath ? (int32_t)R16(pPath + 6) : 0; }
    uint32_t nLine = 0x2e;
    if (pRoom) {
        const int32_t rx = (int32_t)R32(pRoom + 0x4c), ry = (int32_t)R32(pRoom + 0x50);
        if (x >= rx && x < rx + (int32_t)R32(pRoom + 0x54) && y >= ry && y < ry + (int32_t)R32(pRoom + 0x58)) {
            uint32_t u = R32(pRoom + 0x74);
            for (; u; u = R32(u + 0xe8)) if (u == pUnit) break;
            if (!u) return 1;
            if (!bInList) return 0;
            nLine = 0x36;
        }
    }
    s->pfnFatal(nLine);
    return -1;
}
