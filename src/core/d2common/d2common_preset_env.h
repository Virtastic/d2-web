// Shared callee env for the 1.13c D2Common level preset builders 0x10CA0-0x12450 (raw retail 32-bit layout; each port is its own file).
// Level ctx: +0x10 first room, +0x14 settings, +0x1B4 outer ctx, +0x1C4/+0x1C8 LCG (low, high), +0x1D0 level id, +0x1C..+0x28 bounds.
// Outer ctx: +0x94 / +0x484 level ids, +0x47C first record, +0x480 flag byte; records chain at +0x1AC with the id at +0x1D0.
// Room: +0x20 definition {+0 preset id, +4 sub id, +0xC flags}, +0x24 next room, +0x34/+0x38 x, y, +0x3C/+0x40 w, h.
// The callees keep their retail registers as explicit arguments.
#pragma once
#include <cstdint>
#include "d2common_maze_env.h"

// 191 x {key, a, b, dir} from rva 0x960D8 (d2common_preset_tables.cpp)
extern "C" const int32_t d2common_preset_records[191][4];

struct D2CommonPresetEnv
{
	void (*f106b0)(const int32_t* pRecEDX, uint32_t pLevel, int32_t* pCounter);   // 0x106B0: EDX = record, stack level, counter (may be NULL)
	uint32_t (*f000)(uint32_t pRoom, uint32_t nDir, uint32_t bScatter);            // 0xF000: stdcall
	void (*pick)(uint32_t pRoomEBX, uint32_t nReset);                              // 0xE490: EBX = room, stack 1
	void (*f44f20)(uint32_t pRecEAX, uint32_t pRoom, uint32_t nDir, uint32_t nZero); // 0x44F20: EAX = record, stack room, dir, 0
	uint32_t (*f2d9b0)(uint32_t pOuterECX, uint32_t nIdEDX);                       // 0x2D9B0: find or create the record of that id
	void (*f10010)(uint32_t pLevelEAX, uint32_t nPreset, uint32_t pSkip);          // 0x10010: EAX = level, stack preset, skip room
	uint32_t (*f10730)(uint32_t pLevelECX);                                        // 0x10730 (table slot 1)
	uint32_t (*f10820)(uint32_t pLevelECX);                                        // 0x10820 (table slot 0)
	uint32_t (*f10910)(uint32_t pLevelECX);                                        // 0x10910 (table slot 2)
	uint32_t (*f10a00)(uint32_t pLevelEAX);                                        // 0x10A00
};

namespace d2common_preset
{
	using d2common_maze::D;
	// the record at a retail rva inside the table
	inline const int32_t* Rec(uint32_t nRva) { return d2common_preset_records[(nRva - 0x960D8u) / 16u]; }
	// a record base plus counter * 16, the way retail indexes (no bound check)
	inline const int32_t* At(uint32_t nBase, int32_t n) { return Rec(nBase + static_cast<uint32_t>(n) * 16u); }
	inline void Step(const D2CommonPresetEnv* e, uint32_t pLevel, uint32_t nBase, int32_t* pn) { e->f106b0(At(nBase, *pn), pLevel, pn); }
	// the LCG step every handler opens with: returns the new low word
	inline uint32_t Lcg(uint32_t pLevel) { return d2common_maze::Lcg(pLevel + 0x1C4); }
	inline int32_t Sdiv2(int32_t v) { return (v - (v >> 31)) >> 1; }   // C's signed v / 2
	// 0x2D3C0 (pure): bounds of the level's rooms -> ctx +0x1C..+0x28
	inline void Bounds(uint32_t pLevel)
	{
		uint32_t r = D(pLevel + 0x10);
		int32_t x0 = (int32_t)D(r + 0x34), y0 = (int32_t)D(r + 0x38);
		int32_t x1 = (int32_t)D(r + 0x3C) + x0, y1 = (int32_t)D(r + 0x40) + y0;
		for (; r; r = D(r + 0x24))
		{
			const int32_t rx = (int32_t)D(r + 0x34), ry = (int32_t)D(r + 0x38);
			if (x1 < (int32_t)D(r + 0x3C) + rx) x1 = (int32_t)D(r + 0x3C) + rx;
			if (rx < x0) x0 = rx;
			if (y1 < (int32_t)D(r + 0x40) + ry) y1 = (int32_t)D(r + 0x40) + ry;
			if (ry < y0) y0 = ry;
		}
		D(pLevel + 0x1C) = (uint32_t)x0;
		D(pLevel + 0x20) = (uint32_t)y0;
		D(pLevel + 0x24) = (uint32_t)(x1 - x0);
		D(pLevel + 0x28) = (uint32_t)(y1 - y0);
	}
	// the preset tag a room's definition gets: id, sub id, flag bit 1
	inline void Tag(uint32_t pRoom, uint32_t nId, uint32_t nSub)
	{
		const uint32_t d = D(pRoom + 0x20);
		D(d + 0xC) |= 2;
		D(d) = nId;
		D(d + 4) = nSub;
	}
	// find the record of that id in the outer ctx's chain, else ask 0x2D9B0
	inline uint32_t FindRec(const D2CommonPresetEnv* e, uint32_t pOuter, uint32_t nId)
	{
		for (uint32_t s = D(pOuter + 0x47C); s; s = D(s + 0x1AC))
			if (D(s + 0x1D0) == nId)
				return s;
		return e->f2d9b0(pOuter, nId);
	}
}
