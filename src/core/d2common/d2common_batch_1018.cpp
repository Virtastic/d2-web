#include "../d2moo/include/Drlg/D2DrlgDrlg.h"
#include "../d2moo/include/Drlg/D2DrlgDrlgVer.h"
#include "../d2moo/include/Drlg/D2DrlgPreset.h"
#include "../d2moo/include/Drlg/D2DrlgOutJung.h"
#include "../d2moo/include/Drlg/D2DrlgOutdoors.h"
#include "../d2moo/include/Drlg/D2DrlgOutDesr.h"
#include "../d2moo/include/Drlg/D2DrlgOutSiege.h"
#include "../d2moo/include/Drlg/D2DrlgOutPlace.h"
#include "../d2moo/include/Drlg/D2DrlgOutWild.h"
#include "../d2moo/compat/Fog.h"

// Prior art: D2MOO DRLG, checked against each cached 1.13c routine.
// Retail extracts several helpers that D2MOO inlines; the outdoor initializer
// aliases retain those helpers in their original order. Stage A only, no parity verdict.

// 1.13c D2Common+0x1B6E0
void __fastcall d2c_1b6e0_create_vertices(void* pool, D2DrlgVertexStrc** vertices,
	D2DrlgCoordStrc* coord, uint8_t direction, D2DrlgOrthStrc* roomData)
{
	DRLGVER_CreateVertices(pool, vertices, coord, direction, roomData);
}

// 1.13c D2Common+0x42070
void __fastcall d2c_42070_normalize_and_merge_vertices(void* pool,
	D2DrlgVertexStrc** vertices)
{
	D2DrlgVertexStrc* vertex = *vertices;
	do
	{
		vertex->nPosX = (vertex->nPosX + ((vertex->nPosX >> 31) & 7)) >> 3;
		vertex->nPosY = (vertex->nPosY + ((vertex->nPosY >> 31) & 7)) >> 3;
		vertex = vertex->pNext;
	}
	while (vertex != *vertices);

	vertex = *vertices;
	do
	{
		D2DrlgVertexStrc* next = vertex->pNext;
		if (vertex->nPosX == next->nPosX && vertex->nPosY == next->nPosY)
		{
			if (next == *vertices)
				*vertices = vertex;
			vertex->pNext = next->pNext;
			vertex->dwFlags |= next->dwFlags;
			vertex->nDirection = next->nDirection;
			D2_FREE_POOL(pool, next);
		}
		vertex = vertex->pNext;
	}
	while (vertex != *vertices);
}

// 1.13c D2Common+0x43190
void __fastcall d2c_43190_build_outdoor_rooms(D2DrlgLevelStrc* level)
{
	uint32_t mask = 0;
	switch (level->nLevelType)
	{
	case 2: mask = 0x44103; break;
	case 16: case 22: case 27: case 28: mask = 1; break;
	case 21: mask = 4; break;
	case 30: case 31: mask = 0x11; break;
	}
	auto* outdoors = level->pOutdoors;
	int tileY = level->nPosY;
	for (int y = 0; y < outdoors->nGridHeight; ++y, tileY += 8)
	{
		int tileX = level->nPosX;
		for (int x = 0; x < outdoors->nGridWidth; ++x, tileX += 8)
		{
			const int flags = DRLGGRID_GetGridEntry(&outdoors->pGrid[1], x, y);
			const uint32_t packed = DRLGGRID_GetGridEntry(&outdoors->pGrid[2], x, y);
			if (packed & 0x200)
			{
				const int preset = DRLGGRID_GetGridEntry(&outdoors->pGrid[0], x, y);
				if (preset)
				{
					D2DrlgCoordStrc coords = {};
					coords.nPosX = tileX;
					coords.nPosY = tileY;
					auto* map = DRLGPRESET_AllocDrlgMap(level, preset, &coords, &level->pSeed);
					DRLGPRESET_SetPickedFileInDrlgMap(map, (packed >> 16) & 15);
					DRLGPRESET_BuildArea(level, map, flags, 0);
				}
			}
			else if (!(packed & 0x100))
			{
				DRLGOUTPLACE_CreateOutdoorRoomEx(level, tileX, tileY, 8, 8, flags,
					packed, DRLGGRID_GetGridEntry(&outdoors->pGrid[3], x, y), mask);
			}
		}
	}
}

// 1.13c D2Common+0x41CA0
void __fastcall d2c_41ca0_init_act5_outdoors(D2DrlgLevelStrc* level)
{
	DRLGOUTSIEGE_InitAct5OutdoorLevel(level);
}

// 1.13c D2Common+0x7F3A0
void __fastcall d2c_7f3a0_init_act1_outdoors(D2DrlgLevelStrc* level)
{
	DRLGOUTWILD_InitAct1OutdoorLevel(level);
}

// 1.13c D2Common+0x854C0
void __fastcall d2c_854c0_build_jungle(D2DrlgLevelStrc* level)
{
	DRLGOUTJUNG_BuildJungle(level);
}

// 1.13c D2Common+0x85E20
void __fastcall d2c_85e20_init_act2_outdoors(D2DrlgLevelStrc* level)
{
	DRLGOUTDESR_InitAct2OutdoorLevel(level);
}

// 1.13c D2Common+0x861A0
void __fastcall d2c_861a0_init_act4_outdoors(D2DrlgLevelStrc* level)
{
	DRLGOUTDOORS_InitAct4OutdoorLevel(level);
}
