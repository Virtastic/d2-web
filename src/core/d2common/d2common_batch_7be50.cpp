// Stage A. Prior art used: D2MOO DRLG warp/orth/vertex/grid routines and
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

static int32_t add32(int32_t a, int32_t b) { return int32_t(uint32_t(a) + uint32_t(b)); }
static int32_t mul32(int32_t a, int32_t b) { return int32_t(uint32_t(a) * uint32_t(b)); }

// 1.13c D2Common+0x7BE50 (ECX = room): same body as 0x7BC20. A status byte (+0x44) >= 4 or a zero ref count
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

// 1.13c D2Common+0x29F30
extern "C" void d2_fn_d2common_29f30(D2UnitStrc* item, int32_t stat)
{
    // ESI holds item; the switch selector is preserved from the caller.
    ITEMS_SetDefenseOrDamage(item, stat);
}

// 1.13c D2Common+0x67F50
// EAX is room, EDI is the caller's pool. Fog 10046 takes pool in ECX,
// allocation in EDX, then file/line/flags on the stack. This helper does not
// reset pointers/counters or free links/grid (the larger D2MOO helper does).
extern "C" void d2_fn_d2common_67f50(D2DrlgRoomStrc* room, void* pool,
    void (*freePool)(void*, void*, const char*, int, int) = FOG_FreePool)
{
    if (auto* grid = room->pTileGrid) {
        constexpr auto file = "..\\Source\\D2Common\\DRLG\\RoomTile.cpp";
        if (grid->pTiles.pWallTiles) freePool(pool, grid->pTiles.pWallTiles, file, 0x592, 0);
        if (grid->pTiles.pFloorTiles) freePool(pool, grid->pTiles.pFloorTiles, file, 0x593, 0);
        if (grid->pTiles.pRoofTiles) freePool(pool, grid->pTiles.pRoofTiles, file, 0x594, 0);
    }
}



extern "C" void D2Common_44B20(D2DrlgOrthStrc** head, D2DrlgOrthStrc* node, bool (*before)(const D2DrlgOrthStrc*, const D2DrlgOrthStrc*));
extern "C" bool D2Common_44B20_Before(const D2DrlgOrthStrc* a, const D2DrlgOrthStrc* b);

struct D2CommonPlacement13c { int32_t x, y, width, height, direction; };
static_assert(offsetof(D2CommonPlacement13c, direction) == 0x10);
// 1.13c D2Common+0x13F30
extern "C" void D2Common_13F30_Placement(const int32_t* origin, int32_t direction,
    D2CommonPlacement13c* out, int32_t width, int32_t height)
{
    int32_t dx = 0, dy = 0;
    switch (direction) {
    case 0: dy = int32_t(0u - uint32_t(height)); break;
    case 1: case 2: {
        int32_t hi = int32_t((int64_t(height) * 0x55555555LL) >> 32);
        dy = int32_t(uint32_t(hi) - uint32_t(height)) >> 1;
        dy = add32(dy, -(dy >> 31));
        dx = direction == 1 ? int32_t(0u - uint32_t(width)) : width;
        break;
    }
    case 3: case 4: {
        int32_t twice = mul32(height, -2);
        dy = add32(twice / 3, twice >> 31);
        dy = add32(dy, -(dy >> 31));
        dx = direction == 3 ? int32_t(0u - uint32_t(width)) : width;
        break;
    }
    }
    out->x = add32(origin[0], dx); out->y = add32(origin[1], dy);
    out->width = width; out->height = height; out->direction = direction;
}

// 1.13c D2Common+0x37020
extern "C" void d2common_37020_port(D2DrlgGridStrc* grid, D2DrlgVertexStrc* vertex,
    int32_t flag, FlagOperation operation, BOOL includeNext)
{
    sub_6FD75DE0(grid, vertex, flag, operation, includeNext);
}

