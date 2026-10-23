// 1.13c D2Common Stage A batch: rows 0x2A480, 0x344F0, 0x460B0, 0x1D710, 0x30880, 0x71610, 0x50EA0.
// Translated from the pinned 1.13c decompiles. Fatal paths that end in _exit(-1) are modelled as
// returns, as in the rest of src/core/d2common. A row whose body is carried by still-UNMAPPED
// callees is a Stage A stub whose comment names what is unproven.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "Units/Units.h"

#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"

extern "C" uint32_t d2_fn_d2common_24610(uint32_t pUnit);
extern "C" {
__attribute__((weak)) int32_t d2_sub_38b70_get(uint32_t pUnit, int32_t nStat);
__attribute__((weak)) void d2_sub_3a740_set(uint32_t pUnit, int32_t nStat, int32_t nValue);
}

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// +0x2A480 (#10992): item socket count. Carried by 0x24610 (stub), 0x38B70 and 0x3A740 (weak seams). Shape: only for unit
// type 4 with item data (unit+0x14); the base cap is ItemsTxt[nClassId].nInvWidth (byte +0x10F) *
// nInvHeight (+0x110), capped at 6, then by the item-data quality dword (+0x00): 4 -> 4, 6 -> 2,
// 8/9 -> 3, 5/7 -> 1 when stat 0xC2 (STAT_ITEM_NUMSOCKETS) is below 1; finally the low byte of 0x24610
// and the caller's count (at least 1) cap it. A positive result sets IFLAG_SOCKETED (0x800) at item
// data +0x18 and writes stat 0xC2.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2a480(uint32_t pUnit, uint32_t nMax) { // 1.13c D2Common+0x2A480
	uint8_t* u = P(pUnit);
	if (!u || W(u, 0) != 4) return;
	const uint8_t* t = reinterpret_cast<const uint8_t*>(DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(W(u, 4))));
	uint32_t nCap = t ? uint32_t(t[0x10F]) * t[0x110] : 0;
	if (nCap == 0) return;
	uint32_t n = nCap < 6 ? nCap : 6;
	int32_t nMaxI = static_cast<int32_t>(nMax);
	uint8_t* d = P(W(u, 0x14));
	switch (d ? W(d, 0) : 2u)
	{
	case 4: if (n > 3) n = 4; break;
	case 5: case 7:
		nMaxI = d2_sub_38b70_get(pUnit, 0xC2);
		if (nMaxI < 1) { nMaxI = static_cast<int32_t>(nMax); if (n) n = 1; }
		break;
	case 6: if (n > 1) n = 2; break;
	case 8: case 9: if (n > 2) n = 3; break;
	}
	const uint32_t q = d2_fn_d2common_24610(pUnit) & 0xFF;
	if (q <= n) n = q;
	if (nMaxI < 2) nMaxI = 1;
	if (nMaxI < static_cast<int32_t>(n)) n = static_cast<uint32_t>(nMaxI);
	if (static_cast<int32_t>(n) > 0)
	{
		if (d) W(d, 0x18) |= 0x800;
		d2_sub_3a740_set(pUnit, 0xC2, static_cast<int32_t>(n));
	}
}

// +0x344F0 (#10993): forwards the state mask at data-tables +0xDC (fStateMasks[4]) and the unit to 0x34080.
extern "C" void d2_fn_d2common_34080(const uint32_t* mask, D2UnitStrc* unit);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_344f0(uint32_t pArg) { // 1.13c D2Common+0x344f0
	d2_fn_d2common_34080(sgptDataTables->fStateMasks[4], reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pArg)));
}

// +0x460B0 (#10996): per-mode emit loop. Stub: every side effect is a still-UNMAPPED callee -- 0x22640
// and 0x221C0 (item format) pick the element, 0x45AA0 emits, 0x458C0 selects the slot, 0x1800/0x1830
// gate modes 3/4 -- with iteration counts 3 for modes 0/2/5, 2 for mode 1, 12 for mode 3 and 2/9/10
// for mode 4. Unknown modes fatal 0xAF.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_460b0(uint32_t nMode, uint32_t nArg2, uint32_t pArg3, uint32_t pArg4, uint32_t nArg5) { // 1.13c D2Common+0x460b0
	(void)nMode;
	(void)nArg2;
	(void)pArg3;
	(void)pArg4;
	(void)nArg5;
}

// +0x1D710 (#10998): MisStream unit teardown. Stub: for unit type 3 with data at unit+0x14 whose dword
// +0x30 is a stream, the body is two Fog ordinal-10046 calls at MisStream.cpp:0x3B/0x3C (the second
// unconditional) whose first two, register-only arguments (memory pool, freed pointer) the decompile
// does not carry.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1d710(uint32_t pUnit) { // 1.13c D2Common+0x1D710
	(void)pUnit;
}

// +0x30880 (#11002): clear a quest record's waypoint block. The pointer at param+0x14 addresses three
// 12-byte entries: dwords at +0x10/+0x14/+0x18 and a uint16 waypoint state pointer at +0x1C/+0x20/+0x24.
// A state that is neither 0x102, 0 nor 0x101 fatals 0x113; 0/0x101 become 0x102. The block pointer is
// then cleared. A null param fatals 0xF6B and a null block 0xF6D. The QuestRecord.cpp:0x60/0x61 and
// Waypoint.cpp:0x121 Fog ordinal-10046 calls are not modelled: their first two arguments are
// register-only in the decompile.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_30880(uint32_t nMemPool, uint32_t pData) { // 1.13c D2Common+0x30880
	(void)nMemPool;
	uint8_t* p = P(pData);
	if (!p) return;                                          // retail fatal 0xF6B
	uint8_t* pBlock = P(W(p, 0x14));
	if (!pBlock) return;                                     // retail fatal 0xF6D

	for (int n = 0; n < 3; ++n)                              // dwords 0x10..0x18, states 0x1C..0x24
	{
		uint8_t* pState = P(W(pBlock, 0x1C + n * 4));
		const uint16_t nState = *reinterpret_cast<uint16_t*>(pState);
		if (nState != 0x102)
		{
			if (nState != 0 && nState != 0x101) return;      // retail fatal 0x113
			*reinterpret_cast<uint16_t*>(pState) = 0x102;
		}
	}
	W(p, 0x14) = 0;
}

// +0x71610 (#11007, D2TestQuestState): return the quest-state record for nQuest -- the table base at
// DAT_6fdefd94 plus nQuest * 0x70 -- when nQuest is not -1 and is below the record count at
// DAT_6fdefd90, else null. Stub: the quest-state table is not modelled in the engine, so it returns
// null; retail fatals 0x4F2/0x4F3 when the table or the entry is missing.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_71610(int32_t nQuest) { // 1.13c D2Common+0x71610
	(void)nQuest;
	return 0;
}

extern "C" uint32_t D2Common_4E480_FindSkillNode(uint32_t, int32_t);
extern "C" int32_t d2_fn_d2common_4fcb0(D2UnitStrc*, D2SkillStrc*);
// +0x50EA0 (#11008): clamped skill level. param_1 is the unit (0 -> 0); the skill id arrives in ESI
// (unmodelled, passed as nSkill). Node = 0x4E480(unit, skill); none -> 0. Level = node+0x28, plus
// 0x4FCB0 when node+0x34 is -1; floored at 0, then capped at *DAT_6fdf0b50 (passed as pCap).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_50ea0(D2UnitStrc* pUnit, int32_t nSkill, const int32_t* pCap) { // 1.13c D2Common+0x50EA0
	if (!pUnit) return 0;
	const uint32_t node = D2Common_4E480_FindSkillNode(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pUnit)), nSkill);
	if (!node) return 0;
	int32_t v = *reinterpret_cast<int32_t*>(static_cast<uintptr_t>(node) + 0x28);
	if (*reinterpret_cast<int32_t*>(static_cast<uintptr_t>(node) + 0x34) == -1)
		v += d2_fn_d2common_4fcb0(pUnit, reinterpret_cast<D2SkillStrc*>(node));
	if (v < 0) v = 0;
	return v < *pCap ? v : *pCap;
}
