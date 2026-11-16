// 1.13c D2Common implementation batch: +0x302E0, +0x2E2C0, +0x2DDB0 (the UNMAPPED callee of
// +0x2E2C0, taken with it), +0x2DF80, +0x120F0, +0x121D0. Only +0x2DDB0 is kept here: the other
// five were already landed on main in src/core/d2common/d2common_batch_24e60.cpp, and defining
// them twice breaks the wasm link. +0x38B00 is the vendored
// STATLIST_GetBaseStat_6FDB6340 and is marked there (src/core/d2moo/src/D2StatList.cpp).
// The +0x2DDB0 body is src/core/d2common/d2common_2ddb0.cpp (env form, shared with its FN probe); this file
// binds the env to the real callees. DRLG level records and their rooms are reached through the 1.13c
// 32-bit offsets (the vendored 1.10f structs reorder those fields). A callee that is still an UNMAPPED
// row is an env-seam function pointer (null until the environment installs it).
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Dungeon.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Units/UnitFinds.h"
#include "Units/Units.h"

#include "d2common_2ddb0.h"

// Env seams for callees that are still-UNMAPPED rows. Signature follows the 1.13c calls (the DRLG
// pointer travels in a register the decompiler lost); a null pointer means the environment has not
// installed the implementation yet.
extern "C"
{
	// 0x44CE0: eight per-difficulty level ids for a level definition.
	int32_t* (*g_d2common_fn_44CE0)(uint32_t pDrlg, int32_t nLevelId) = nullptr;

	EMSCRIPTEN_KEEPALIVE int32_t d2_batch_d2common_2ddb0(uint32_t pLevel);
}

namespace
{
	uint32_t Rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }
	uint32_t Rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
	void Wr32(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)) = v; }
	uint32_t VisIds(uint32_t pDrlg, int32_t nLevelId)
	{
		return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_d2common_fn_44CE0(pDrlg, nLevelId)));
	}
	uint32_t AllocLevel(uint32_t pDrlg, int32_t nLevelId)
	{
		return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(
			DRLG_AllocLevel(reinterpret_cast<D2DrlgStrc*>(static_cast<uintptr_t>(pDrlg)), nLevelId)));
	}
	void FreePool(uint32_t pPool, uint32_t p, uint32_t)
	{
		D2_FREE_POOL(reinterpret_cast<void*>(static_cast<uintptr_t>(pPool)), reinterpret_cast<void*>(static_cast<uintptr_t>(p)));
	}
}

// 1.13c D2Common+0x2DDB0: returns 1 when the level's room set can be discarded (see d2common_2ddb0.cpp).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_batch_d2common_2ddb0(uint32_t pLevel)
{
	if (!pLevel || !g_d2common_fn_44CE0)
	{
		return 0;
	}
	const D2Common2DDB0Env env{ Rd32, Rd8, Wr32, VisIds, AllocLevel, FreePool };
	return D2Common_2DDB0_LevelDiscard(pLevel, &env);
}
