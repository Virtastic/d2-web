// 1.13c D2Common Stage A batch: rows 0x2A480, 0x344F0, 0x460B0, 0x1D710, 0x30880, 0x71610, 0x50EA0.
// Translated from the pinned 1.13c decompiles. Fatal paths that end in _exit(-1) are modelled as
// returns, as in the rest of src/core/d2common. A row whose body is carried by still-UNMAPPED
// callees is a Stage A stub whose comment names what is unproven.
#include <cstdint>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// +0x2A480 (#10992): item socket count. Stub: the body is carried by the still-UNMAPPED 0x24610
// (socket-count helper), 0x38B70 (stat getter) and 0x3A740 (stat setter). Proven shape: only for unit
// type 4 with item data (unit+0x14); the base cap is ItemsTxt[nClassId].nInvWidth (byte +0x10F) *
// nInvHeight (+0x110), capped at 6, then by the item-data quality dword (+0x00): 4 -> 4, 6 -> 2,
// 8/9 -> 3, 5/7 -> 1 when stat 0xC2 (STAT_ITEM_NUMSOCKETS) is below 1; finally the low byte of 0x24610
// and the caller's count (at least 1) cap it. A positive result sets IFLAG_SOCKETED (0x800) at item
// data +0x18 and writes stat 0xC2.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2a480(uint32_t pUnit, uint32_t nMax) { // 1.13c D2Common+0x2A480
	(void)pUnit;
	(void)nMax;
}

// +0x344F0 (#10993): forwards the data-tables pointer at +0xDC and the argument to 0x34080.
// Stub: 0x34080 is UNMAPPED; nothing of the call is written.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_344f0(uint32_t pArg) { // 1.13c D2Common+0x344F0
	(void)pArg;
}

// +0x460B0 (#10996): per-mode emit loop. Stub: every side effect is a still-UNMAPPED callee -- 0x22640
// and 0x221C0 (item format) pick the element, 0x45AA0 emits, 0x458C0 selects the slot, 0x1800/0x1830
// gate modes 3/4 -- with iteration counts 3 for modes 0/2/5, 2 for mode 1, 12 for mode 3 and 2/9/10
// for mode 4. Unknown modes fatal 0xAF.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_460b0(uint32_t nMode, uint32_t nArg2, uint32_t pArg3, uint32_t pArg4, uint32_t nArg5) { // 1.13c D2Common+0x460B0
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

// +0x50EA0 (#11008): clamp a value. Stub: the value is 0x4E480's dword +0x28, plus 0x4FCB0 when its
// dword +0x34 is -1, clamped to at least 0 and then to the cap at *DAT_6fdf0b50; a zero from 0x4E480
// yields 0. Both callees and the cap global are UNMAPPED.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_50ea0(int32_t nArg) { // 1.13c D2Common+0x50EA0
	(void)nArg;
	return 0;
}
