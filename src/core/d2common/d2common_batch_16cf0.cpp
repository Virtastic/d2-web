// Prior art used: D2MOO DrlgOutRoom / DrlgDrlgGrid / DrlgDrlgVer.
// Retail's ESI points directly at the outdoor data, not its owning room.
#include "Drlg/D2DrlgOutRoom.h"
#include "Drlg/D2DrlgDrlgVer.h"

extern "C" {
#define W __attribute__((weak))
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
}

// Retail never frees a grid here. The caller frees the rows first, so each grid's row pointer must
// already be clear (retail asserts DrlgGrid.cpp:0x218); the pair is then zeroed and the vertices go.
static void d2_drlg_grid_clear_pair(D2DrlgGridStrc* pGrid)
{
    if (pGrid->pCellsRowOffsets)
    {
        Ordinal_10046("..\\Source\\D2Common\\DRLG\\DrlgGrid.cpp", 0x218, 0);
    }
    pGrid->pCellsFlags = NULL;
    pGrid->pCellsRowOffsets = NULL;
}

// 1.13c D2Common+0x4A4D0
extern "C" void d2_fn_d2common_4a4d0(D2DrlgOutdoorRoomStrc* outdoor, void* mempool)
{
    d2_drlg_grid_clear_pair(&outdoor->pTileTypeGrid);
    d2_drlg_grid_clear_pair(&outdoor->pWallGrid);
    d2_drlg_grid_clear_pair(&outdoor->pFloorGrid);
    d2_drlg_grid_clear_pair(&outdoor->pDirtPathGrid);
    DRLGVER_FreeVertices(mempool, &outdoor->pVertex);
}

// Retail returns immediately; this is an intentional no-op entry point.
// 1.13c D2Common+0x22FC0
extern "C" void d2_fn_d2common_22fc0()
{
}
