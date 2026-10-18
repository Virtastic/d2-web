// 1.13c D2Common implementation batch: +0x311B0, +0x4E110, +0x3BF00, +0x4E030, +0x22B20,
// +0x38850, +0x132A0. Bodies follow the cached 1.13c decompile; pointer fields are read at the
// retail (32-bit) offsets, as in the other d2common batch files. Retail's fatal paths
// (Fog message + _exit(-1)) are modelled as returns. Rows whose decompile hides register-passed
// arguments behind an untyped call are marked stubs naming the unproven part.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/MonsterTbls.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t R(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
}

// +0x311B0: monster (unit type 1) in animation mode 12 whose MonStats2 record has
// mask bit 0xC clear in flags byte +6 and mask bit 0x1C set in flags byte +4. The decompile's
// calls to 0x13B0 and 0x33D60 carry their arguments in registers (no arguments printed); the
// class-id operand and the owner operand are therefore unproven, so the body is a marked stub.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_fn_d2common_311b0(uint32_t pUnit) {   // 1.13c D2Common+0x311B0
	(void)pUnit;
	return false;
}

// +0x4E110: store nValue at +0x10 of the record; a null record is retail's fatal
// path and becomes a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4e110(uint32_t pRecord, uint32_t nValue) {   // 1.13c D2Common+0x4E110
	if (!pRecord)
		return;
	*reinterpret_cast<uint32_t*>(P(pRecord) + 0x10) = nValue;
}

// +0x3BF00: true when the byte at (pointer+0x10)->0x44 is at least 3.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_fn_d2common_3bf00(uint32_t pRecord) {   // 1.13c D2Common+0x3BF00
	if (!pRecord)
		return false;
	const uint32_t pInner = R(P(pRecord), 0x10);
	if (!pInner)
		return false;
	return P(pInner)[0x44] >= 3;
}

// +0x4E030: the dword at +0x18 of the record; a null record is retail's fatal path
// and answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4e030(uint32_t pRecord) {   // 1.13c D2Common+0x4E030
	if (!pRecord)
		return 0;
	return R(P(pRecord), 0x18);
}

// +0x22B20: dword at +0xEC of monstats.txt record nIndex. The two globals retail
// reads (count, base) are the same table this engine reaches through sgptDataTables; an
// out-of-range index is retail's fatal path and answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_22b20(uint32_t nIndex) {   // 1.13c D2Common+0x22B20
	static_assert(sizeof(D2MonStatsTxt) == 0x1A8, "monstats.txt record stride");
	static_assert(offsetof(D2MonStatsTxt, nA2MaxD) == 0xEC, "monstats.txt +0xEC");

	const uint32_t nCount = static_cast<uint32_t>(sgptDataTables->nMonStatsTxtRecordCount);
	if (!sgptDataTables->pMonStatsTxt || nIndex >= nCount)
		return 0;

	const uint8_t* pRecord = reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonStatsTxt[nIndex]);
	return R(pRecord, 0xEC);
}

// +0x38850: walks the stat array at record+0x24 (count at +0x54) and hands each
// found stat's dword at +4, or 0, to the callback with the caller's context. The decompile hides
// the register-passed arguments (EAX = stats array, EDI = queried id) of the 0x382B0 lookup and
// the loop index is the only visible candidate, so the body is a marked stub.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_38850(uint32_t pRecord, uint32_t pContext, void (*pCallback)(uint32_t, uint32_t)) {   // 1.13c D2Common+0x38850
	(void)pRecord;
	(void)pContext;
	(void)pCallback;
}

// +0x132A0 GetGlobalBeltRecord: copies the 0x108-byte record at
// (nRow * 7 + nIndex) of the global belt table to pOut. The table base is retail's global
// DAT_6fdf13dc and arrives through the env seam; a zero base with index 0 is retail's fatal path
// and becomes a no-op.
struct D2Common132A0Env
{
	const uint8_t* pBeltRecords;													//0x00
};

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_132a0(uint32_t nIndex, uint32_t nRow, uint32_t* pOut, const D2Common132A0Env* pEnv) {   // 1.13c D2Common+0x132A0
	const uint8_t* pBase = pEnv ? pEnv->pBeltRecords : nullptr;
	if (!pBase && nIndex == 0)
		return;
	if (!pOut)
		return;

	const uint8_t* pSource = pBase + (nRow * 7 + nIndex) * 0x108u;
	for (int i = 0; i < 0x42; ++i)
		pOut[i] = R(pSource, i * 4);
}
