// Stage A. Prior art used: the 1.10f source DRLG warp/orth/vertex/grid routines and
// ITEMS_SetDefenseOrDamage; checked against each cached 1.13c decompile.
// Native links use pointers; retail coordinates, IDs and arithmetic remain 32-bit.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgDrlgGrid.h"
#include "Drlg/D2DrlgDrlgVer.h"
#include "D2Items.h"


// stand-alone 32-bit layout twin of D2Common 0x7BE50 (the proven row runs DRLGACTIVATE_RoomExIdentifyRealStatus) (ECX = room): same body as 0x7BC20. A status byte (+0x44) >= 4 or a zero ref count
// at +0xC+status*2 picks the first status 0..3 with a non-zero count (4 when none), relinks through
// 0x7BB70 and stores it at +0x44. Retail room records are read by their 32-bit offsets.
extern "C" void D2Common_7BB70_RelinkStatus(intptr_t pRoom, uint8_t nStatus);
extern "C" void D2Common_7BE50_NormaliseStatus(intptr_t pRoom)
{
    uint8_t* r = reinterpret_cast<uint8_t*>(pRoom);
    const uint8_t cur = r[0x44];
    uint16_t c[4];
    std::memcpy(c, r + 0xC, sizeof c);
    if (cur < 4 && c[cur] != 0) return;
    uint8_t s = 0;
    do {
        if (c[s] != 0) break;
        ++s;
    } while (s < 4);
    D2Common_7BB70_RelinkStatus(pRoom, s);
    r[0x44] = s;
}

// D2Common+0x29F30 is d2common_29f30.cpp; the play path reaches it through d2common_29f30_env.cpp.
// D2Common+0x67F50 moved to d2common_67f50.cpp.




// D2Common+0x13F30 is sub_6FD83970 in game/DrlgOutPlace.cpp; the probe runs it (stand-alone copy removed).

// The 1.13c +0x37020 body is sub_6FD75DE0 in DrlgDrlgGrid.cpp; this is only the extern "C" entry the probe calls.
extern "C" void d2common_37020_port(D2DrlgGridStrc* grid, D2DrlgVertexStrc* vertex,
    int32_t flag, FlagOperation operation, BOOL includeNext)
{
    sub_6FD75DE0(grid, vertex, flag, operation, includeNext);
}

// The 1.13c +0x36CD0 body is sub_6FD75F60 in DrlgDrlgGrid.cpp. Callers hold retail images (a vertex whose end is the
// 32-bit pointer at +0x10, a grid of two 32-bit words {cells, rows}, a {x,y,w,h} clip rect); this builds the native
// views over the same memory, plots, and leaves every write in the caller's cells.
extern "C" void d2common_36cd0_port(const void* pLine, const void* pRect, const void* pGrid, int nValue, int nSel, int nThick)
{
    const int32_t* l = (const int32_t*)pLine;
    const uint32_t* g = (const uint32_t*)pGrid;
    D2DrlgGridStrc grid{};
    grid.pCellsFlags = (int32_t*)(uintptr_t)g[0];
    grid.pCellsRowOffsets = (int32_t*)(uintptr_t)g[1];
    const int32_t* e = (const int32_t*)(uintptr_t)(uint32_t)l[4];
    D2DrlgVertexStrc next{};
    next.nPosX = e[0]; next.nPosY = e[1];
    D2DrlgVertexStrc cur{};
    cur.nPosX = l[0]; cur.nPosY = l[1];
    cur.pNext = &next;
    D2DrlgCoordStrc rect;
    std::memcpy(&rect, pRect, sizeof rect);
    sub_6FD75F60(&grid, &cur, &rect, nValue, (FlagOperation)nSel, nThick);
}

