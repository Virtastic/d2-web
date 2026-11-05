// 1.13c D2Common+0x54A00 (ordinal 11089, stdcall, ret 0x18): per-level monster stat block.
// monstats row (stride 0x1A8) by id, monlvl row (stride 0x78) by level clamped to count - 1; difficulty clamped 0..2; the game
// type selects the expansion column (+3 dwords). Each flag bit fills its slots of the 14-int out block: raw monstats shorts when
// the row's NORATIO flag (low byte of +0x0C against gdwBitMasks[2]) is set, else ApplyRatio(monstats, monlvl, 100).
// 1.10f -> 1.13c: every monstats value is read SIGNED (movsx); the vendored struct declares them uint16_t, which zero-extends.
// Only the low nine flag bits are tested (byte 0 and bit 0 of byte 1).
#include <cstdint>
#include <cstring>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "DataTbls/MonsterTbls.h"

static_assert(sizeof(D2MonStatsTxt) == 0x1A8, "monstats stride");
static_assert(sizeof(D2MonLvlTxt) == 0x78, "monlvl stride");

namespace {
inline int32_t S16(const uint8_t* p, int32_t o) { int16_t v; std::memcpy(&v, p + o, 2); return v; }
inline int32_t D32(const uint8_t* p, int32_t o) { int32_t v; std::memcpy(&v, p + o, 4); return v; }
}

extern "C" int32_t D2Common_54A00_MonStatsByLevel(int32_t nMonsterId, int32_t nGameType, int32_t nDifficulty, int32_t nLevel,
                                                   uint32_t nFlags, int32_t* pOut) {   // 1.13c D2Common+0x54A00
	if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount) return 0;
	const uint8_t* ms = reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonStatsTxt[nMonsterId]);
	if (!ms) return 0;
	const int32_t nLvlCount = sgptDataTables->nMonLvlTxtRecordCount;
	if (nLevel >= nLvlCount - 1) nLevel = nLvlCount - 1;
	if (nLevel < 0 || nLevel >= nLvlCount) return 0;
	const uint8_t* ml = reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonLvlTxt[nLevel]);
	if (!ml) return 0;
	const int32_t d = nDifficulty < 1 ? 0 : (nDifficulty > 1 ? 2 : nDifficulty);
	const int32_t col = d + (nGameType != 0 ? 3 : 0);
	const bool raw = (ms[0x0C] & static_cast<uint8_t>(gdwBitMasks[MONSTATSFLAGINDEX_NORATIO])) != 0;
	// monlvl columns: AC +0x00, TH +0x18, HP +0x30, DM +0x48, XP +0x60 (three normal then three expansion dwords each)
	auto lv = [&](int32_t base) { return D32(ml, base + col * 4); };
	auto st = [&](int32_t off) { return S16(ms, off + d * 2); };
	auto ratio = [&](int32_t base, int32_t off) { return DATATBLS_ApplyRatio(lv(base), st(off), 100); };
	if (nFlags & 0x1) {
		if (raw) { pOut[0] = st(0xB0); pOut[1] = st(0xB6); }
		else { pOut[0] = ratio(0x30, 0xB0); pOut[1] = ratio(0x30, 0xB6); }
	}
	if (nFlags & 0x2) pOut[2] = raw ? st(0xBC) : ratio(0x00, 0xBC);
	if (nFlags & 0x4) pOut[4] = raw ? st(0xD4) : ratio(0x60, 0xD4);
	// attack slots: TH, min, max -> out[3], out[a], out[a + 1]
	static const int32_t kAtk[3][4] = { { 0x08, 0xC2, 0xDA, 5 }, { 0x10, 0xC8, 0xE6, 7 }, { 0x20, 0xCE, 0xF2, 9 } };
	for (const auto& a : kAtk) {
		if (!(nFlags & static_cast<uint32_t>(a[0]))) continue;
		if (raw) { pOut[3] = st(a[1]); pOut[a[3]] = st(a[2]); pOut[a[3] + 1] = st(a[2] + 6); }
		else { pOut[3] = ratio(0x18, a[1]); pOut[a[3]] = ratio(0x48, a[2]); pOut[a[3] + 1] = ratio(0x48, a[2] + 6); }
	}
	// elemental slots 0..2 (min +0x10E, max +0x120, duration +0x132, 6 bytes per slot): raw lands in out[9..10], scaled in out[11..12]
	for (int32_t k = 0; k < 3; ++k) {
		if (!(nFlags & (0x40u << k))) continue;
		const int32_t o = k * 6;
		if (raw) { pOut[9] = st(0x10E + o); pOut[10] = st(0x120 + o); }
		else { pOut[11] = ratio(0x48, 0x10E + o); pOut[12] = ratio(0x48, 0x120 + o); }
		pOut[13] = st(0x132 + o);
	}
	return 1;
}
