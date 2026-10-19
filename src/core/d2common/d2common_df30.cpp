// 1.13c D2Common+0x0DF30.
// The tile walkers below remain seams: they are separate D2Common entry points and
// retain their retail argument order at this boundary.
#include <cstdint>
#include <emscripten.h>

struct D2CommonDF30Env
{
	void (*walk_base)(uint32_t pTiles, uint32_t nX, uint32_t nY, uint32_t pBase, uint32_t nMode);
	void (*walk_tile)(uint32_t pTile, uint32_t nX, uint32_t nY, uint32_t pBase, uint32_t nMode);
	void (*refresh_room)(uint32_t pTiles, uint32_t nX, uint32_t nY, uint32_t nState, uint32_t nMode);
	uint16_t (*get_tile_state)(uint32_t pTile, uint32_t nX, uint32_t nY, uint32_t nState, uint32_t nFlags);
	void (*set_tile_state)(uint32_t pTile, uint32_t nX, uint32_t nY, uint32_t nState, uint32_t nMode);
	void (*set_world_position)(uint32_t nXFixed, uint32_t nYFixed, uint32_t pTile);
	uint32_t (*get_current_unit)(void);
	void (*fatal)(uint32_t nMessage);
	void (*finish_update)(void);
};

namespace
{
	inline uint8_t* P(uint32_t address)
	{
		return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(address));
	}

	inline uint32_t R32(const uint8_t* base, uint32_t offset)
	{
		return *reinterpret_cast<const uint32_t*>(base + offset);
	}

	inline void W32(uint8_t* base, uint32_t offset, uint32_t value)
	{
		*reinterpret_cast<uint32_t*>(base + offset) = value;
	}

	inline uint16_t R16(const uint8_t* base, uint32_t offset)
	{
		return *reinterpret_cast<const uint16_t*>(base + offset);
	}

	inline void W16(uint8_t* base, uint32_t offset, uint16_t value)
	{
		*reinterpret_cast<uint16_t*>(base + offset) = value;
	}
}

// Models the register-context function body. pContext is the retail EAX context,
// pTile and nCoords are the two stack arguments. All pointers stored in retail state
// remain 32-bit values; callbacks resolve the corresponding engine entry points.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_df30(
	uint32_t pContext, uint32_t pTile, uint32_t nCoords, const D2CommonDF30Env* env)
{
	if (!pContext || !env)
		return 0;
	uint8_t* const context = P(pContext);
	const uint32_t pTiles = R32(context, 0x1C);
	const uint32_t nX = R16(context, 0x02);
	const uint32_t nY = R16(context, 0x06);
	const uint32_t nLowX = nCoords & 0xFFFF;
	const uint32_t nHighY = nCoords >> 16;
	const uint32_t pMode = R32(context, 0x4C);

	const uint32_t pType = R32(context, 0x30);
	const bool use_base_walk = pType == 0 || R32(P(pType), 0) != 3;
	if (use_base_walk)
	{
		const uint32_t pBase = R32(context, 0x48);
		if (nCoords == 0)
		{
			if (env->walk_base)
				env->walk_base(pTiles, nX, nY, pBase, pMode);
		}
		else if (pTiles != 0)
		{
			if (env->walk_base)
				env->walk_base(pTiles, nX, nY, pBase, pMode);
			if (env->walk_tile)
				env->walk_tile(pTile, nLowX, nHighY, pBase, pMode);
		}
	}
	else if (nCoords == 0)
	{
		if (env->refresh_room)
			env->refresh_room(pTiles, nX, nY, R32(context, 0x44), pMode);
		W16(context, 0x54, 0);
	}
	else
	{
		const uint32_t nCurrentCoords = (R16(context, 0x06) << 16) | R16(context, 0x02);
		uint32_t nFlags = R32(context, 0x34);
		if (nCoords == nCurrentCoords)
			nFlags &= ~8u;
		else
			nFlags |= 8u;
		W32(context, 0x34, nFlags);
		const uint32_t nState = R32(context, 0x44);
		const uint32_t nTileFlags = R32(context, 0x50);
		if (pTiles != 0 && env->refresh_room)
			env->refresh_room(pTiles, nX, nY, nState, pMode);
		const uint16_t nTileState = env->get_tile_state
			? env->get_tile_state(pTile, nLowX, nHighY, nState, nTileFlags) : 0;
		if (env->set_tile_state)
			env->set_tile_state(pTile, nLowX, nHighY, nState, pMode);
		W16(context, 0x54, nTileState);
		W32(context, 0x1D4, 1);
		W32(context, 0x1D8, nCoords);
	}

	if (pTiles != pTile)
		W32(context, 0x34, R32(context, 0x34) | 1u);
	if (env->set_world_position)
		env->set_world_position(nCoords * 0x10000u + 0x8000u,
			 nHighY * 0x10000u + 0x8000u, pTile);
	if (nCoords != 0 && env->get_current_unit && env->get_current_unit() == 0)
	{
		if (env->fatal)
			env->fatal(0x489);
		return 0;
	}
	if (env->finish_update)
		env->finish_update();
	return 1;
}
