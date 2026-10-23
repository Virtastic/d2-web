// D2Common implementation batch: twelve UNMAPPED rows translated from the pinned 1.13c decompiles
// (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Probe shape: r[0..3] are the arguments in
// descriptor order, r[4] the result. Pointer results are normalised to a byte offset from the
// table base (0 = null/out of range), matching the other d2common table getters.
// Prior art: D2MOO 1.10f (D2Seed.h, DATATBLS_GetSkillDescTxtRecord, UNITS_GetCharStatsTxtRecord);
// the 1.13c bodies at these RVAs are the same arithmetic over the same D2DataTablesStrc layout.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "../d2moo/compat/datatbls_bridge.h"
#include "../tables/shared.h"

static_assert(sizeof(D2CharStatsTxt) == 0xC4, "charstats row size");
static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "skills.txt dwParam");
static_assert(offsetof(D2SkillsTxt, wSkillDesc) == 0x194, "skills.txt wSkillDesc");
static_assert(offsetof(D2MonStatsTxt, wMonStatsEx) == 0x18, "monstats.txt wMonStatsEx");
static_assert(offsetof(D2MonStats2Txt, nFlags) == 0x04, "monstats2.txt nFlags");
static_assert(offsetof(D2MonStats2Txt, nModeFlags) == 0xF0, "monstats2.txt nModeFlags");

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

	// 0x1400/0x1470: bit nBit of the MonStats2 dword at byte offset nFieldOffset (0xF0 nModeFlags,
	// 0x04 nFlags) for the monster row nMonsterId. Out-of-range ids and a missing table give 0.
	// ponytail: the null-table guards are the probe's own; retail only bounds the ids.
	uint32_t MonStats2Bit(int32_t nMonsterId, uint32_t nBit, int32_t nFieldOffset)
	{
		if (!sgptDataTables->pMonStatsTxt || nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
		{
			return 0;
		}

		const int32_t nMonStats2 = sgptDataTables->pMonStatsTxt[nMonsterId].wMonStatsEx;
		if (!sgptDataTables->pMonStats2Txt || nMonStats2 < 0 || nMonStats2 >= sgptDataTables->nMonStats2TxtRecordCount)
		{
			return 0;
		}

		const uint8_t* pFlags = reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonStats2Txt[nMonStats2]);
		return pFlags[nFieldOffset + (static_cast<int32_t>(nBit) >> 3)] & gdwBitMasks[nBit & 7];
	}

	// 0x1500: skills.txt row -> wSkillDesc -> skilldesc.txt row, as a byte offset (0x120 rows).
	uint32_t SkillDescOffset(int32_t nSkillId)
	{
		if (!sgptDataTables->pSkillsTxt || nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
		{
			return 0;
		}

		const int16_t nSkillDesc = static_cast<int16_t>(sgptDataTables->pSkillsTxt[nSkillId].wSkillDesc);
		if (!sgptDataTables->pSkillDescTxt || nSkillDesc < 0 || nSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
		{
			return 0;
		}

		return static_cast<uint32_t>(nSkillDesc) * 0x120;
	}

	// 0x1550..0x15E0: skills.txt dwParam[0..3] of the skill row nSkillId.
	int32_t SkillParam(int32_t nSkillId, int nParam)
	{
		if (!sgptDataTables->pSkillsTxt || nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
		{
			return 0;
		}

		return sgptDataTables->pSkillsTxt[nSkillId].dwParam[nParam];
	}
}

extern "C" {
// 1.13c D2Common+0x10B0 (SEED_RollLimitedRandomNumber): r[0] = pSeed, r[1] = nMax, r[4] = roll;
// the seed pair is advanced in place.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_10b0(uint32_t* r)
{
	r[4] = RollLimited(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[0])), r[1]);
}

// 1.13c D2Common+0x1100: the second copy of the same body, nMax arriving in ESI instead of EAX.
EMSCRIPTEN_KEEPALIVE __attribute__((weak)) void d2_fn_d2common_1100(uint32_t* r)
{
	r[4] = RollLimited(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[0])), r[1]);
}

// 1.13c D2Common+0x1180: r[0] = pSeed (ECX), r[1] = nMin (EDI), r[2] = nMax (EAX).
// nMax <= nMin returns nMin without rolling; otherwise one LCG step gives nMin + roll over
// (nMax - nMin + 1). The range is unsigned, so a wrapped count is rejected by its sign bit.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1180(uint32_t* r)
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

// 1.13c D2Common+0x1280: empty body (`return;`), no result.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1280(uint32_t* r)
{
	(void)r;
}

// 1.13c D2Common+0x12D0: charstats row nClassId (0xC4 each) as a byte offset from the table base.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_12d0(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	const int32_t nClassId = static_cast<int32_t>(r[0]);
	if (!sgptDataTables->pCharStatsTxt || nClassId < 0 || nClassId >= sgptDataTables->nCharStatsTxtRecordCount)
	{
		r[4] = 0;
		return;
	}
	r[4] = static_cast<uint32_t>(nClassId) * 0xC4;
}

// 1.13c D2Common+0x1400: MonStats2 nModeFlags (+0xF0) bit nBit of monster row nMonsterId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1400(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = MonStats2Bit(static_cast<int32_t>(r[0]), r[1], 0xF0);
}

// 1.13c D2Common+0x1470: MonStats2 nFlags (+0x04, dwFlags) bit nBit of monster row nMonsterId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1470(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = MonStats2Bit(static_cast<int32_t>(r[0]), r[1], 0x04);
}

// 1.13c D2Common+0x1500: skills.txt wSkillDesc (+0x194) of nSkillId -> skilldesc row offset (0x120).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1500(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = SkillDescOffset(static_cast<int32_t>(r[0]));
}

// 1.13c D2Common+0x1550: skills.txt dwParam[0] (+0x148) of nSkillId.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1550(uint32_t* r)
{
	if (!TablesLoaded()) { r[4] = 0x7EADBEEF; return; }
	r[4] = static_cast<uint32_t>(SkillParam(static_cast<int32_t>(r[0]), 0));
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
