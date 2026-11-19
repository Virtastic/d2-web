// D2Common implementation batch: twelve UNMAPPED rows translated from the pinned 1.13c decompiles
// (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Probe shape: r[0..3] are the arguments in
// descriptor order, r[4] the result. Pointer results are normalised to a byte offset from the
// table base (0 = null/out of range), matching the other d2common table getters.
// Prior art: 1.10f (D2Seed.h, DATATBLS_GetSkillDescTxtRecord, UNITS_GetCharStatsTxtRecord);
// the 1.13c bodies at these RVAs are the same arithmetic over the same D2DataTablesStrc layout.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2Seed.h"
#include "DataTbls/SkillsTbls.h"
#include "../compat/datatbls_bridge.h"
#include "../tables/shared.h"

static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "skills.txt dwParam");

namespace
{
	inline bool TablesLoaded()
	{
		return d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang());
	}

	// 0x10B0/0x1100/0x1180 run the game bodies SEED_RollLimitedRandomNumber and SEED_RollRange (D2Seed.h).

	// 0x1550..0x15E0: skills.txt dwParam[0..3] now run the game-tree bodies in SkillsTbls.cpp.
	int32_t SkillParam(int32_t nSkillId, int nParam)
	{
		switch (nParam)
		{
		case 0: return DATATBLS_GetSkillParam0(nSkillId);
		case 1: return DATATBLS_GetSkillParam1(nSkillId);
		case 2: return DATATBLS_GetSkillParam2(nSkillId);
		default: return DATATBLS_GetSkillParam3(nSkillId);
		}
	}
}

extern "C" {
// Register-shape adapters for the seam callers (0x335F0, 0x465B0); the bodies are the game ones in D2Seed.h.
// r[0] = pSeed, r[1] = nMax, r[4] = roll (the 0x10B0 probe is src/core/fn/d2common_10b0.cpp).
EMSCRIPTEN_KEEPALIVE __attribute__((weak)) void d2_fn_d2common_1100(uint32_t* r);
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1100(uint32_t* r)
{
	r[4] = SEED_RollLimitedRandomNumber(reinterpret_cast<D2SeedStrc*>(static_cast<uintptr_t>(r[0])), static_cast<int32_t>(r[1]));
}

// r[0] = pSeed (ECX), r[1] = nMin (EDI), r[2] = nMax (EAX).
void D2Common_1180_RollRange(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(SEED_RollRange(reinterpret_cast<D2SeedStrc*>(static_cast<uintptr_t>(r[0])),
		static_cast<int32_t>(r[1]), static_cast<int32_t>(r[2])));
}

// 0x1280, 0x12D0, 0x1400, 0x1470 and 0x1500 are probed in src/core/fn/d2common_<rva>.cpp (one definition each).

// 0x1550, 0x1580 and 0x15B0 are probed in src/core/fn/d2common_<rva>.cpp; D2Skills.cpp calls the game bodies.

// 1.13c D2Common+0x15E0: skills.txt dwParam[3] (+0x154) of nSkillId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_15e0(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = static_cast<uint32_t>(SkillParam(static_cast<int32_t>(r[0]), 3));
}
}
