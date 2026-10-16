// 1.13c D2Common+0x4B8F0 and +0x4B970: the two small collision-mask helpers D2MOO inlines.
#include <cstdint>
#include "D2Collision.h"
#include "D2Dungeon.h"
// 1.13c D2Common+0x4B8F0: OR nMask into the 3x3 box centred on (nX, nY), spilling into neighbouring rooms.
void D2Common_4B8F0_SetMaskBox3x3(D2ActiveRoomStrc* pRoom, int nX, int nY, uint16_t nMask)
{
	D2BoundingBoxStrc box = { nX - 1, nY - 1, nX + 1, nY + 1 };
	COLLISION_SetCollisionMaskForBoundingBoxRecursively(pRoom, &box, nMask);
}
// 1.13c D2Common+0x4B970: OR nMask into the five-cell cross at (nX, nY): x-1, x, x+1 on the row, then y-1, y+1 on the column.
void D2Common_4B970_SetMaskCross(D2ActiveRoomStrc* pRoom, int nX, int nY, uint16_t nMask)
{
	COLLISION_SetCollisionMask(pRoom, nX - 1, nY, nMask);
	COLLISION_SetCollisionMask(pRoom, nX, nY, nMask);
	COLLISION_SetCollisionMask(pRoom, nX + 1, nY, nMask);
	COLLISION_SetCollisionMask(pRoom, nX, nY - 1, nMask);
	COLLISION_SetCollisionMask(pRoom, nX, nY + 1, nMask);
}
// 1.13c D2Common+0x4B560 / +0x4B5F0 / +0x4B6B0: 5x5 stamps straight into a room's collision mask (grid-relative indices, no room walk,
// no upper clamp; negative x/y clamp to 0 on the low side).
#include <D2CMP.h>
template <class F> static void stamp5x5(D2RoomCollisionGridStrc* g, int nX, int nY, F f)
{
	if (!g || !g->pCollisionMask) return;
	const int x0 = nX > 0 ? nX : 0, y0 = nY > 0 ? nY : 0, w = g->pRoomCoords.nSubtileWidth;
	for (int y = y0; y < nY + 5; ++y)
		for (int x = x0; x < nX + 5; ++x) f(g->pCollisionMask[y * w + x], nX, nY, x, y);
}
// 1.13c D2Common+0x4B560: OR nMask into the 5x5 block at (nX, nY).
void D2Common_4B560_OrMask5x5(D2RoomCollisionGridStrc* g, int nX, int nY, uint16_t nMask)
{
	stamp5x5(g, nX, nY, [&](uint16_t& m, int, int, int, int) { m |= nMask; });
}
// 1.13c D2Common+0x4B5F0: overwrite the 5x5 block with the tile's flag bytes (row order reversed: tile row = nY + 4 - y).
void D2Common_4B5F0_SetTileFlags5x5(D2RoomCollisionGridStrc* g, int nX, int nY, D2TileLibraryEntryStrc* pTile)
{
	if (!g || !g->pCollisionMask) return;
	const uint8_t* f = D2CMP_10085_GetTileFlagArray(pTile);
	stamp5x5(g, nX, nY, [&](uint16_t& m, int, int, int x, int y) { m = f[(nY + 4 - y) * 5 + (x - nX)]; });
}
// 1.13c D2Common+0x4B6B0: clear the tile's flag bits from the 5x5 block.
void D2Common_4B6B0_ClearTileFlags5x5(D2RoomCollisionGridStrc* g, int nX, int nY, D2TileLibraryEntryStrc* pTile)
{
	if (!g || !g->pCollisionMask) return;
	const uint8_t* f = D2CMP_10085_GetTileFlagArray(pTile);
	stamp5x5(g, nX, nY, [&](uint16_t& m, int, int, int x, int y) { m &= ~(uint16_t)f[(nY + 4 - y) * 5 + (x - nX)]; });
}
// 1.13c D2Common+0x4B780: OR the tile's flag bits into the 5x5 block.
void D2Common_4B780_OrTileFlags5x5(D2RoomCollisionGridStrc* g, int nX, int nY, D2TileLibraryEntryStrc* pTile)
{
	if (!g || !g->pCollisionMask) return;
	const uint8_t* f = D2CMP_10085_GetTileFlagArray(pTile);
	stamp5x5(g, nX, nY, [&](uint16_t& m, int, int, int x, int y) { m |= (uint16_t)f[(nY + 4 - y) * 5 + (x - nX)]; });
}
// 1.13c D2Common+0x4CA20: stamp a tile's flags into the collision grid of the room holding (grid tile origin + (nDX,nDY)) * 5:
// clear pTile1's bits, then OR pTile2's when non-null. Needs the room's own grid; the target room via COLLISION_GetRoomBySubTileCoordinates (+0x1330).
void D2Common_4CA20_ClearThenOrTile(D2ActiveRoomStrc* pRoom, int nDX, int nDY, D2TileLibraryEntryStrc* pTile1, D2TileLibraryEntryStrc* pTile2)
{
	if (!pRoom || !pRoom->pCollisionGrid) return;
	const int x = (pRoom->pCollisionGrid->pRoomCoords.nTileXPos + nDX) * 5, y = (pRoom->pCollisionGrid->pRoomCoords.nTileYPos + nDY) * 5;
	D2ActiveRoomStrc* r2 = COLLISION_GetRoomBySubTileCoordinates(pRoom, x, y);
	if (!r2 || !r2->pCollisionGrid) return;
	D2RoomCollisionGridStrc* g = r2->pCollisionGrid;
	D2Common_4B6B0_ClearTileFlags5x5(g, x - g->pRoomCoords.nSubtileX, y - g->pRoomCoords.nSubtileY, pTile1);
	if (pTile2) D2Common_4B780_OrTileFlags5x5(g, x - g->pRoomCoords.nSubtileX, y - g->pRoomCoords.nSubtileY, pTile2);
}
// 1.13c D2Common+0x4CA90: for each of nCount entries (dx, dy, pTile, nFlags) whose tile cell (origin + (dx,dy)) * 5 lies inside pGrid:
// bFlag 0 -> OR the tile's flags (4B780), else assign them (4B5F0); then OR 0x10/1/4 for entry flag bits 2/0x40/0x80 (4B560).
struct D2Coll4CA90Entry { int nDX, nDY; uint8_t nFlags; D2TileLibraryEntryStrc* pTile; };
void D2Common_4CA90_StampEntries(D2RoomCollisionGridStrc* g, const D2Coll4CA90Entry* e, int nCount, int nTileX, int nTileY, int bFlag)
{
	for (int i = 0; i < nCount; ++i) {
		const int x = (e[i].nDX + nTileX) * 5, y = (nTileY + e[i].nDY) * 5;
		const D2DrlgCoordsStrc& c = g->pRoomCoords;
		if (!(c.nSubtileX <= x && x < c.nSubtileWidth + c.nSubtileX && c.nSubtileY <= y && y < c.nSubtileHeight + c.nSubtileY)) continue;
		const int rx = x - c.nSubtileX, ry = y - c.nSubtileY;
		if (!bFlag) D2Common_4B780_OrTileFlags5x5(g, rx, ry, e[i].pTile); else D2Common_4B5F0_SetTileFlags5x5(g, rx, ry, e[i].pTile);
		uint16_t m = 0;
		if (e[i].nFlags & 2) m = 0x10;
		if (e[i].nFlags & 0x40) m |= 1;
		if (e[i].nFlags & 0x80) m |= 4;
		if (m) D2Common_4B560_OrMask5x5(g, rx, ry, m);
	}
}
