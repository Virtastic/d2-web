// D2Common implementation batch: twelve UNMAPPED rows translated from the pinned 1.13c decompiles
// (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Probe shape: r[0..3] are the arguments in
// descriptor order, r[4] the result. Pointer results are normalised to a byte offset from the
// table base (0 = null/out of range), matching the other d2common table getters.
// Prior art: D2MOO 1.10f (D2Seed.h, DATATBLS_GetSkillDescTxtRecord, UNITS_GetCharStatsTxtRecord);
// the 1.13c bodies at these RVAs are the same arithmetic over the same D2DataTablesStrc layout.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "../d2moo/compat/datatbls_bridge.h"
#include "../tables/shared.h"

static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "skills.txt dwParam");

namespace
{
	inline bool TablesLoaded()
	{
		return d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang());
	}

	// 0x10B0/0x1100: one LCG step, then the limited roll. Both bodies are identical; they differ
	// only in the register carrying nMax (EAX at 0x10B0, ESI at 0x1100), which the probe models
	// as r[1] either way. nMax < 1 returns 0 without rolling.
	uint32_t RollLimited(uint32_t* pSeed, uint32_t nMax)
	{
		if (static_cast<int32_t>(nMax) < 1)
		{
			return 0;
		}

		const uint64_t lSeed = static_cast<uint64_t>(pSeed[0]) * 0x6AC690C5ull + pSeed[1];
		pSeed[0] = static_cast<uint32_t>(lSeed);
		pSeed[1] = static_cast<uint32_t>(lSeed >> 32);

		if ((nMax & (nMax - 1)) != 0)
		{
			return static_cast<uint32_t>(lSeed) % nMax;
		}

		return (nMax - 1) & static_cast<uint32_t>(lSeed);
	}

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
// 1.13c D2Common+0x10B0 (SEED_RollLimitedRandomNumber): r[0] = pSeed, r[1] = nMax, r[4] = roll;
// the seed pair is advanced in place.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_10b0(uint32_t* r)
{
	r[4] = RollLimited(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[0])), r[1]);
}

EMSCRIPTEN_KEEPALIVE __attribute__((weak)) void d2_fn_d2common_1100(uint32_t* r);
// 1.13c D2Common+0x1100: the second copy of the same body, nMax arriving in ESI instead of EAX.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1100(uint32_t* r)
{
	r[4] = RollLimited(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[0])), r[1]);
}

// 1.13c D2Common+0x1180: r[0] = pSeed (ECX), r[1] = nMin (EDI), r[2] = nMax (EAX).
// nMax <= nMin returns nMin without rolling; otherwise one LCG step gives nMin + roll over
// (nMax - nMin + 1). The range is unsigned, so a wrapped count is rejected by its sign bit.
void D2Common_1180_RollRange(uint32_t* r)
{
	uint32_t* pSeed = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[0]));
	const int32_t nMin = static_cast<int32_t>(r[1]);
	const int32_t nMax = static_cast<int32_t>(r[2]);
	if (nMax <= nMin)
	{
		r[4] = static_cast<uint32_t>(nMin);
		return;
	}

	const uint32_t nRange = static_cast<uint32_t>(nMax - nMin);
	const uint32_t nCount = nRange + 1;
	if (static_cast<int32_t>(nCount) < 1)
	{
		r[4] = static_cast<uint32_t>(nMin);
		return;
	}

	const uint64_t lSeed = static_cast<uint64_t>(pSeed[0]) * 0x6AC690C5ull + pSeed[1];
	pSeed[0] = static_cast<uint32_t>(lSeed);
	pSeed[1] = static_cast<uint32_t>(lSeed >> 32);

	if ((nCount & nRange) != 0)
	{
		r[4] = static_cast<uint32_t>(nMin) + static_cast<uint32_t>(lSeed) % nCount;
	}
	else
	{
		r[4] = static_cast<uint32_t>(nMin) + (nRange & static_cast<uint32_t>(lSeed));
	}
}

// 0x1280, 0x12D0, 0x1400, 0x1470 and 0x1500 are probed in src/core/fn/d2common_<rva>.cpp (one definition each).

// 1.13c D2Common+0x1550: skills.txt dwParam[0] (+0x148) of nSkillId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1550(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	// The record pins sgptDataTables to blob 1: its +0xB98 holds the
	// address of the skills rows (blob 0, retail stride 0x23C = ours), +0xBA0 the count. Run the rule on that
	// table, then put the loaded one back.
	D2SkillsTxt* pSaved = sgptDataTables->pSkillsTxt;
	const int32_t nSavedCount = sgptDataTables->nSkillsTxtRecordCount;
	if (r[13] >= 2)
	{
		const uint8_t* pDt = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(r[16]));
		uint32_t nRows = 0;
		int32_t nCount = 0;
		std::memcpy(&nRows, pDt + 0xB98, 4);
		std::memcpy(&nCount, pDt + 0xBA0, 4);
		sgptDataTables->pSkillsTxt = reinterpret_cast<D2SkillsTxt*>(static_cast<uintptr_t>(nRows));
		sgptDataTables->nSkillsTxtRecordCount = nCount;
	}
	r[4] = static_cast<uint32_t>(SkillParam(static_cast<int32_t>(r[0]), 0));
	sgptDataTables->pSkillsTxt = pSaved;
	sgptDataTables->nSkillsTxtRecordCount = nSavedCount;
}

// 1.13c D2Common+0x1580: skills.txt dwParam[1] (+0x14C) of nSkillId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1580(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = static_cast<uint32_t>(SkillParam(static_cast<int32_t>(r[0]), 1));
}

// 1.13c D2Common+0x15B0: skills.txt dwParam[2] (+0x150) of nSkillId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_15b0(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = static_cast<uint32_t>(SkillParam(static_cast<int32_t>(r[0]), 2));
}

// 1.13c D2Common+0x15E0: skills.txt dwParam[3] (+0x154) of nSkillId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_15e0(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = static_cast<uint32_t>(SkillParam(static_cast<int32_t>(r[0]), 3));
}
}
