// 1.13c D2Common+0x0DF30 port interface.
#pragma once
#include <cstdint>

struct D2CommonDF30Env
{
	void (*walk_base)(uint32_t pTiles, uint32_t nX, uint32_t nY, uint32_t pBase, uint32_t nMode);
	void (*walk_tile)(uint32_t pTile, uint32_t nX, uint32_t nY, uint32_t pBase, uint32_t nMode);
	void (*refresh_room)(uint32_t pTiles, uint32_t nX, uint32_t nY, uint32_t nState, uint32_t nMode);
	uint16_t (*get_tile_state)(uint32_t pTile, uint32_t nX, uint32_t nY, uint32_t nState, uint32_t nFlags);
	void (*set_tile_state)(uint32_t pTile, uint32_t nX, uint32_t nY, uint32_t nState, uint32_t nMode);
	uint32_t (*get_room)(uint32_t pTile, uint32_t nLowX, uint32_t nHighY);   // 0x1330: EAX = pTile, EBX = low coordinate, EDI = high coordinate
	void (*fatal)(uint32_t nMessage);
	void (*finish_update)(uint32_t pContext);                // 0xDC80: EAX = the path
	void (*recache_room)(uint32_t pContext, uint32_t nXFixed, uint32_t nYFixed, uint32_t pTile);   // 0xDA40: ESI = the path
};

extern "C" int32_t d2common_df30_port(uint32_t pContext, uint32_t pTile, uint32_t nCoords, const D2CommonDF30Env* env);
