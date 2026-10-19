// Prior art used: D2MOO DrlgOutRoom / DrlgDrlgGrid / DrlgDrlgVer.
// Retail's ESI points directly at the outdoor data, not its owning room.
#include "Drlg/D2DrlgOutRoom.h"

// 1.13c D2Common+0x4A4D0
extern "C" void d2_fn_d2common_4a4d0(D2DrlgOutdoorRoomStrc* outdoor, void* mempool)
{
    DRLGGRID_FreeGrid(mempool, &outdoor->pTileTypeGrid);
    DRLGGRID_FreeGrid(mempool, &outdoor->pWallGrid);
    DRLGGRID_FreeGrid(mempool, &outdoor->pFloorGrid);
    DRLGGRID_FreeGrid(mempool, &outdoor->pDirtPathGrid);
    DRLGVER_FreeVertices(mempool, &outdoor->pVertex);
}

// Retail returns immediately; this is an intentional no-op entry point.
// 1.13c D2Common+0x22FC0
extern "C" void d2_fn_d2common_22fc0()
{
}
