#include "Drlg/D2DrlgOutWild.h"
#include <test_seam.h>

#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgGrid.h"
#include "Drlg/D2DrlgOutdoors.h"
#include "Drlg/D2DrlgOutPlace.h"
#include "Drlg/D2DrlgMaze.h"
#include "D2Seed.h"
#include <DataTbls/LevelsIds.h>

// The 1.13c wilderness steps the level initializer runs (src/core/d2common/d2common_batch_41070.cpp)
void D2Common_7EF60(D2DrlgLevelStrc* pLevel);
void D2Common_7EE50(D2DrlgLevelStrc* pLevel);
void D2Common_7F180(D2DrlgLevelStrc* pLevel);

//D2Common.0x6FD84CA0
void __fastcall DRLGOUTWILD_GetBridgeCoords(D2DrlgLevelStrc* pLevel, int* pX, int* pY)
{
	int nX = pLevel->pOutdoors->nGridWidth / 2 - 1;

	for (int nY = 1; nY < pLevel->pOutdoors->nGridWidth - 1; ++nY)
	{
		if (DRLGGRID_GetGridEntry(&pLevel->pOutdoors->pGrid[0], nX, nY) == 28 
			&& DRLGOUTDOORS_GetPackedGrid2Info(pLevel->pOutdoors, nX, nY).nPickedFile == 1)
		{
			*pX = nX;
			*pY = nY;
			return;
		}
	}

	*pX = -1;
	*pY = -1;
}

// 1.13c D2Common+0x7F3A0 (step order and level-id gates diffed against it; the vertex loop is the body of D2Common+0x7E8C0,
// rewritten here to its retail form with the two 1.10f vertex predicates inlined as retail does)
//D2Common.0x6FD84D30
void __fastcall DRLGOUTWILD_InitAct1OutdoorLevel(D2DrlgLevelStrc* pLevel)
{
	int nX = 0;
	int nY = 0;
	BOOL bAdded = FALSE;
	BOOL bBreak = FALSE;

	int v21 = 0;
	bool v22; // zf@53

	if (pLevel->nLevelId != LEVEL_BLOODMOOR && pLevel->nLevelId != LEVEL_COLDPLAINS && pLevel->nLevelId != LEVEL_BURIALGROUNDS)
	{
		D2DrlgVertexStrc* pVertex = pLevel->pOutdoors->pVertex;
		D2DrlgVertexStrc* pPreviousVertex = pVertex;

		for (D2DrlgVertexStrc* i = pVertex->pNext; i != pVertex; i = i->pNext)
		{
			pPreviousVertex = i;
		}

		bBreak = FALSE;
		do
		{
			D2DrlgVertexStrc* pNextVertex = pVertex->pNext;
			if (pVertex->nPosX < pNextVertex->nPosX && pPreviousVertex->nPosY > pVertex->nPosY && !(pVertex->dwFlags & 1)
				&& !(pPreviousVertex->dwFlags & 1) || pVertex->nPosY > pNextVertex->nPosY 
				&& pPreviousVertex->nPosX > pVertex->nPosX && !(pVertex->dwFlags & 1) && !(pPreviousVertex->dwFlags & 1))
			{
				D2DrlgVertexStrc* pFirstVertex = pVertex;
				D2DrlgVertexStrc* pSpecialVertex = NULL;
				do
				{
					if (pVertex == pLevel->pOutdoors->pVertex)
					{
						bBreak = TRUE;
					}

					D2DrlgVertexStrc* pNext = pVertex->pNext;
					if (pVertex->nPosY < pNext->nPosY || pNext->nPosX < pVertex->nPosX || (pVertex->dwFlags & 1) || (pNext->dwFlags & 1))
					{
						break;
					}

					if ((pVertex->nPosX < pNext->nPosX && pNext->nPosY < pNext->pNext->nPosY && !(pNext->dwFlags & 1))
						|| (pNext->nPosY < pVertex->nPosY && pNext->nPosX < pNext->pNext->nPosX && !(pNext->dwFlags & 1)))
					{
						pSpecialVertex = pVertex;
					}

					pVertex = pNext;
				}
				while (pVertex != pFirstVertex);

				if (pSpecialVertex)
				{
					for (D2DrlgVertexStrc* j = pFirstVertex; j != pSpecialVertex; j = j->pNext)
					{
						j->nDirection = 1;
					}

					pSpecialVertex->nDirection = 1;

					pLevel->pOutdoors->dwFlags |= 0x20;
				}
			}

			pPreviousVertex = pVertex;
			pVertex = pVertex->pNext;
		}
		while (!bBreak && pVertex != pLevel->pOutdoors->pVertex);
	}

	DRLGOUTPLACE_SetOutGridLinkFlags(pLevel);
	DRLGOUTPLACE_PlaceAct1245OutdoorBorders(pLevel);

	if (pLevel->nLevelId >= LEVEL_BLOODMOOR && pLevel->nLevelId <= LEVEL_TAMOEHIGHLAND)
	{
		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_CLIFFS, LVLPREST_ACT1_WILD_BORDER_1);

		// river, cliff caves and the entrance placement (the farm is skipped inside)
		D2Common_7EF60(pLevel);

		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_MIDDLE, LVLPREST_ACT1_WILD_BORDER_1);
		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_CORNER, LVLPREST_ACT1_WILD_BORDER_1);
		D2Common_7EE50(pLevel);
		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_BORDER, LVLPREST_ACT1_WILD_BORDER_1);
		DRLGOUTDOORS_SpawnAct1DirtPaths(pLevel);
	}

	if (pLevel->nLevelId == LEVEL_MOOMOOFARM)
	{
		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_CLIFFS, LVLPREST_ACT1_WILD_BORDER_1);
		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_MIDDLE, LVLPREST_ACT1_WILD_BORDER_1);
		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_CORNER, LVLPREST_ACT1_WILD_BORDER_1);
		DRLGOUTDOORS_AddAct124SecondaryBorder(pLevel, LVLSUB_ACT1_BORDER_BORDER, LVLPREST_ACT1_WILD_BORDER_1);
	}

	if (pLevel->nLevelId >= LEVEL_COLDPLAINS && pLevel->nLevelId <= LEVEL_BLACKMARSH)
	{
		DRLGOUTDOORS_SpawnAct12Waypoint(pLevel);
	}

	if (pLevel->nLevelId >= LEVEL_BLOODMOOR && pLevel->nLevelId <= LEVEL_TAMOEHIGHLAND)
	{
		DRLGOUTDOORS_SpawnAct12Shrines(pLevel, 5);
	}

	D2Common_7F180(pLevel);
}

// 1.13c D2Common+0x7E9F0 (diffed; identical)
//D2Common.0x6FD85060
BOOL __fastcall DRLGOUTWILD_TestSpawnRiver(D2DrlgLevelStrc* pLevel, int nX)
{
	for (int nY = 0; nY < pLevel->pOutdoors->nGridHeight; ++nY)
	{
		if (DRLGOUTDOORS_GetPackedGrid2Info(pLevel->pOutdoors, nX, nY).bHasDirection 
			|| DRLGOUTDOORS_GetPackedGrid2Info(pLevel->pOutdoors, nX + 1, nY).bHasDirection)
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 1.13c D2Common+0x7ECC0 (the per-row loop body; retail inlines it there; the 12-entry table was read from the DLL)
void DRLGOUTWILD_SpawnRiverPreset(D2DrlgLevelStrc* pLevel, int nX, int nY, bool bLowerRiver)
{
	static const struct {
		int nUpperFile;
		int nLowerFile;
	} stru_6FDD0CA0[] =
	{
		{ 2, 2 },
		{ 0, 3 },
		{ 1, 1 },
		{ 3, 0 },
		{ 0, 2 },
		{ 0, 1 },
		{ 1, 0 },
		{ 2, 0 },
		{ 2, 3 },
		{ 1, 3 },
		{ 3, 1 },
		{ 3, 2 }
	};
	int nPickedFile = 0;

	D2DrlgOutdoorPackedGrid2InfoStrc tPackedInfo = DRLGOUTDOORS_GetPackedGrid2Info(pLevel->pOutdoors, nX + int(bLowerRiver), nY);
	int nFlags2 = DRLGGRID_GetGridEntry(&pLevel->pOutdoors->pGrid[0], nX + int(bLowerRiver), nY);
	if (nFlags2)
	{
		if (nFlags2 != LVLPREST_ACT1_WILD_BORDER_4 || tPackedInfo.nPickedFile != 3)
		{

			const int nIdx = nFlags2 - LVLPREST_ACT1_WILD_BORDER_1;
			D2_ASSERT(nIdx < ARRAY_SIZE(stru_6FDD0CA0));
			nPickedFile = bLowerRiver ? stru_6FDD0CA0[nIdx].nLowerFile : stru_6FDD0CA0[nIdx].nUpperFile;
		}
		else
		{
			nPickedFile = 3;
		}
	}
	else
	{
		if (tPackedInfo.nUnkb08)
		{
			nPickedFile = 0;
		}
		else
		{
			nPickedFile = 3;
		}
	}

	DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(pLevel, nX + int(bLowerRiver), nY, bLowerRiver ? LVLPREST_ACT1_RIVER_LOWER : LVLPREST_ACT1_RIVER_UPPER, nPickedFile, FALSE);

}
// 1.13c D2Common+0x7ECC0, with the bridge placement of D2Common+0x7EB40 (diffed branch by branch)
//D2Common.0x6FD850B0
void __fastcall DRLGOUTWILD_SpawnRiver(D2DrlgLevelStrc* pLevel, int nX)
{
	if (D2Seam<&DRLGOUTWILD_SpawnRiver>::hook) { D2Seam<&DRLGOUTWILD_SpawnRiver>::hook(pLevel, nX); return; }
	for (int nY = 0; nY < pLevel->pOutdoors->nGridHeight; ++nY)
	{
		DRLGOUTWILD_SpawnRiverPreset(pLevel, nX, nY, false);
		DRLGOUTWILD_SpawnRiverPreset(pLevel, nX, nY, true);
	}

	if (pLevel->pOutdoors->dwFlags & (OUTDOOR_RIVER|OUTDOOR_BRIDGE))
	{
		const int nRowRange = pLevel->pOutdoors->nGridHeight - 2;
		// retail draws only when the range is positive
		int nRand = nRowRange > 0 ? SEED_RollLimitedRandomNumber(&pLevel->pSeed, nRowRange) : 0;

		for (int i = 0; i < nRowRange; ++i)
		{
			int nY = (nRand + i) % nRowRange + 1;

			if (DRLGOUTDOORS_TestGridCellSpawnValid(pLevel, nX - 1, nY) && ((pLevel->pOutdoors->dwFlags & OUTDOOR_BRIDGE) != 0 || DRLGOUTDOORS_TestGridCellSpawnValid(pLevel, nX + 2, nY)))
			{
				if (DRLGOUTDOORS_GetPackedGrid2Info(pLevel->pOutdoors, nX    , nY).nPickedFile == 3 && 
					DRLGOUTDOORS_GetPackedGrid2Info(pLevel->pOutdoors, nX + 1, nY).nPickedFile == 3)
				{
					DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(pLevel, nX    , nY, LVLPREST_ACT1_BRIDGE, 1, 0);
					DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(pLevel, nX + 1, nY, LVLPREST_ACT1_BRIDGE, (pLevel->pOutdoors->dwFlags & OUTDOOR_BRIDGE) != 0 ? 3 : 2, 0);
					return;
				}
			}
		}
	}
}

// 1.13c D2Common+0x7EF60 (the per-cell body of its cave scan loop; retail inlines it: grid 16 -> preset 0x19, 17 -> 0x18, then flag 0x40)
//D2Common.0x6FD85390
BOOL __fastcall DRLGOUTWILD_SpawnCliffCaves(D2DrlgLevelStrc* pLevel, int nX, int nY)
{
	switch (DRLGGRID_GetGridEntry(pLevel->pOutdoors->pGrid, nX, nY))
	{
	case 16:
		DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(pLevel, nX, nY, LVLPREST_ACT1_WILD_CLIFF_CAVE_LEFT, -1, 0);
		pLevel->pOutdoors->dwFlags |= 0x40;
		return TRUE;

	case 17:
		DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(pLevel, nX, nY, LVLPREST_ACT1_WILD_CLIFF_CAVE_RIGHT, -1, 0);
		pLevel->pOutdoors->dwFlags |= 0x40;
		return TRUE;

	default:
		return FALSE;
	}
}


// 1.13c D2Common+0x7EDC0 (diffed; identical: same seed draws, same preset ids)
//D2Common.0x6FD85920
void __fastcall DRLGOUTWILD_SpawnCottage(D2DrlgLevelStrc* pLevel, int nLvlPrestId, int a3)
{
	if (SEED_RollRandomNumber(&pLevel->pSeed) & 3)
	{
		DRLGOUTDOORS_SpawnRandomOutdoorDS1(pLevel, nLvlPrestId, -1);

		if (a3 && SEED_RollRandomNumber(&pLevel->pSeed) & 1)
		{
			DRLGOUTDOORS_SpawnRandomOutdoorDS1(pLevel, LVLPREST_ACT1_COTTAGES_3, -1);
		}
	}
	else
	{
		DRLGOUTDOORS_SpawnRandomOutdoorDS1(pLevel, nLvlPrestId, -1);
		DRLGOUTDOORS_SpawnRandomOutdoorDS1(pLevel, nLvlPrestId, -1);
	}
}
