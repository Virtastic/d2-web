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
#include "Units/Units.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t R(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
}

// +0x311B0: monster (unit type 1) in animation mode 12 (+0x10) whose MonStats2 record (by class id
// at unit+4) has (byte)gdwBitMasks[12] clear in byte +6 and (byte)gdwBitMasks[28] set in byte +4,
// and for which the 0x33D60 state-mask test (eax = unit) is false. gdwBitMasks[i] is 1<<i, so both
// byte-truncated masks are 0: the second test never holds and retail always returns false. The
// literal truncation is kept; the tail call is therefore unreachable.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_fn_d2common_311b0(uint32_t pUnit) {   // 1.13c D2Common+0x311B0
	if (!pUnit || R(P(pUnit), 0) != 1 || R(P(pUnit), 0x10) != 0xC)
		return false;
	const D2MonStats2Txt* rec = UNITS_GetMonStats2TxtRecordFromMonsterId(static_cast<int>(R(P(pUnit), 4)));
	if (!rec)
		return false;
	const uint8_t* b = reinterpret_cast<const uint8_t*>(rec);
	return (static_cast<uint8_t>(1u << 12) & b[6]) == 0 && (static_cast<uint8_t>(1u << 28) & b[4]) != 0 && false;
}

// +0x4E110: store nValue at +0x10 of the record; a null record is retail's fatal
// path and becomes a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4e110(uint32_t pRecord, uint32_t nValue) {   // 1.13c D2Common+0x4e110
	if (!pRecord)
		return;
	*reinterpret_cast<uint32_t*>(P(pRecord) + 0x10) = nValue;
}

// +0x3BF00: true when the byte at (pointer+0x10)->0x44 is at least 3.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_fn_d2common_3bf00(uint32_t pRecord) {   // 1.13c D2Common+0x3bf00
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

// +0x38850: when the unit's stat-list record (pRecord+0x5C) has flags (+0x10) < 0, for each
// id 0..count(+0x54) looks it up by binary search in the stat array {ptr +0x24, u16 count +0x28,
// 8-byte entries keyed by dword 0} (0x382B0) and calls pCallback(entry dword +4 or 0, pContext).
// Runs only if pRecord != 0 and (pContext != 0 or pCallback == 0). The id fed to 0x382B0 is a
// register argument the decompile hides; the loop index is the only candidate.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_38850(uint32_t pRecord, uint32_t pContext, void (*pCallback)(uint32_t, uint32_t)) {   // 1.13c D2Common+0x38850
	if (!pRecord || (!pContext && pCallback))
		return;
	const uint8_t* r = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(*reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pRecord) + 0x5C)));
	if (!r || *reinterpret_cast<const int32_t*>(r + 0x10) >= 0)
		return;
	const int16_t n = *reinterpret_cast<const int16_t*>(r + 0x54);
	const uint8_t* arr = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(*reinterpret_cast<const uint32_t*>(r + 0x24)));
	const int16_t cnt = *reinterpret_cast<const int16_t*>(r + 0x28);
	for (int32_t i = 0; i < n; ++i) {
		int32_t lo = 0, hi = cnt - 1, f = -1;
		while (lo <= hi) {
			const int32_t m = (lo + hi) / 2;
			const uint32_t k = *reinterpret_cast<const uint32_t*>(arr + m * 8);
			if (k == static_cast<uint32_t>(i)) { f = m; break; }
			if (k < static_cast<uint32_t>(i)) lo = m + 1; else hi = m - 1;
		}
		pCallback(f < 0 ? 0 : *reinterpret_cast<const uint32_t*>(arr + f * 8 + 4), pContext);
	}
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
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_132a0(uint32_t nIndex, uint32_t nRow, uint32_t* pOut, const D2Common132A0Env* pEnv) {   // 1.13c D2Common+0x132a0
	const uint8_t* pBase = pEnv ? pEnv->pBeltRecords : nullptr;
	if (!pBase && nIndex == 0)
		return;
	if (!pOut)
		return;

	const uint8_t* pSource = pBase + (nRow * 7 + nIndex) * 0x108u;
	for (int i = 0; i < 0x42; ++i)
		pOut[i] = R(pSource, i * 4);
}
