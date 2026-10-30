// src/core/d2common/d2common_batch_2f7a0.cpp -- twelve 1.13c D2Common.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Image base 0x6FD50000.
//
// Retail records are opaque here and are read by their retail byte offsets (rd8/rd16/rd32), with
// retail addresses travelling as 32-bit integers in the style of the other d2common batches.
// Callees src/ does not implement are function-pointer seams on D2CommonBatch2F7A0Env, handed over
// after the retail arguments; a missing seam contributes zero. Register-only operands the decompile
// drops (in_EAX / unaff_EDI / unaff_BL) are named seam parameters. The paths that end in _exit(-1)
// are modelled as plain returns, so the exported body never faults.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "Drlg/D2DrlgActivate.h"
#include "Units/Units.h"

// Implemented in d2common_batch_1fb80.cpp; called by 0x51FF0.
int32_t D2Common_1FB80_GetWieldType(D2UnitStrc* pPlayer, D2InventoryStrc* pInventory);

namespace
{
	inline const uint8_t* P(uint32_t a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }

	inline uint8_t  rd8 (const void* p, uint32_t o) { uint8_t  v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 1); return v; }
	inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 2); return v; }
	inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
	inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 2); }

	// The same reads from a retail 32-bit address.
	inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8(P(a), o); }
	inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16(P(a), o); }
	inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32(P(a), o); }
	inline void     wr16(uint32_t a, uint32_t o, uint16_t v) { wr16(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), o, v); }
}

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2CommonBatch2F7A0Env {
	// 0x22EC0(class id, value): items.txt row +0xC0 == value.
	int32_t  (*sub22EC0)(int32_t nClassId, int32_t nValue);
	// 0x4F930(0): the alternate branch of 0x51FF0.
	int32_t  (*sub4F930)(int32_t nZero);
	// 0x38F60(unit, 0x18): a weapon's stat total.
	int32_t  (*sub38F60)(uint32_t pUnit, int32_t nStatId);
	// 0x38B70(unit, 0x16): the unit's stat total.
	int32_t  (*sub38B70)(uint32_t pUnit, int32_t nStatId);
	// 0x51BF0(unit, dwDmgSymPerCalc, skill id, skill level): the skill formula evaluator.
	int32_t  (*sub51BF0)(uint32_t pUnit, uint32_t nCalc, int32_t nSkillId, int32_t nSkillLevel);
	// 0x382F0(buffer, 0xCC, 0x40): stat pairs of the list in its register argument; answers the count.
	int32_t  (*sub382F0)(void* pBuffer, uint32_t nStatId, int32_t nMax, uint32_t pStatList);
	// 0x7C090: retail reads its record from EDI and a byte from BL; both are explicit here.
	void     (*sub7C090)(uint32_t pRecord, uint32_t nBL);
	// 0x99BF8 (PTR_FUN_6fde9bf8): the callback the decrement path dispatches.
	void     (*cb99BF8)();
	// 0x12C00(a, b, c, 0x80000, file, line, format, operands): the profiler's log line. The trailing
	// varargs are register-only in the decompile; the record itself is the one recovered operand.
	int32_t  (*sub12C00)(int32_t nA, int32_t nB, int32_t nC, uint32_t nFlags, const char* szFile,
	                     int32_t nLine, const char* szFormat, const void* pSlot);
	// kernel32 QueryPerformanceFrequency. Retail keeps the answer in a local it never reads.
	void     (*queryPerformanceFrequency)(void* pFrequency);
	// 0x6FDE9EC8: 23 profiling records of 0x78 bytes each (D2Common_4A450's slots).
	const uint8_t* perfRecords;
};

// 1.13c D2Common+0x2F7A0 (#10289)
// A type-0 unit's record is unit+0x14; its left skill id (+0x74) and left skill flags (+0x7C) go to the
// two out pointers. Anything else stores 0 and 0xFFFFFFFF. A null record is the retail fatal 0x1449,
// which never returns, so nothing is stored.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_sub_2F7A0(uint32_t pUnit, uint32_t* pSkillId, uint32_t* pFlags)
{
	if (pUnit != 0 && rd32(pUnit, 0) == 0) {
		const uint32_t pPlayer = rd32(pUnit, 0x14);
		if (pPlayer == 0) return;
		*pSkillId = rd32(pPlayer, 0x74);
		*pFlags   = rd32(pPlayer, 0x7C);
		return;
	}
	*pSkillId = 0;
	*pFlags   = 0xFFFFFFFFu;
}

// 1.13c D2Common+0x18E30 (#10290)
// ObjMode table lookup: the unsigned index is bounded by the table count, then row bGetMode 0 gives
// pObjType and 1 gives pObjMode (any other mode answers null, as does an out-of-range index). This is
// the body of the vendored DATATBLS_GetObjModeTypeTxtRecord, whose signed bound the unsigned test
// makes equivalent; its null-row fatal path is an assert there and a null here.
extern "C"
EMSCRIPTEN_KEEPALIVE D2ObjModeTypeTxt* d2_fn_d2common_18E30(uint32_t nIndex, int32_t bGetMode)
{
	if (nIndex >= (uint32_t)sgptDataTables->pObjModeDataTables.nObjModeTypeTxtRecordCount) return nullptr;
	return DATATBLS_GetObjModeTypeTxtRecord((int32_t)nIndex, bGetMode);
}

// 1.13c D2Common+0x24C10 (#10291)
// Item units (type 4) answer 0x22EC0(items.txt class id at +0x04, value): whether the row's +0xC0 dword
// equals the value. Every other unit answers false.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_24C10(uint32_t pUnit, int32_t nValue,
                                                  const D2CommonBatch2F7A0Env* env)
{
	if (pUnit != 0 && rd32(pUnit, 0) == 4 && env->sub22EC0 != nullptr) {
		return env->sub22EC0((int32_t)rd32(pUnit, 4), nValue);
	}
	return 0;
}

// 1.13c D2Common+0x2ECF0 (#10296, D2GetRunesBIN)
// Object units (type 2) answer their object record, unit+0x14 -> +0x00. A null unit (fatal 0xE3C) and
// every other type (fatal 0xE43) return 0; a null record faults in retail and returns 0 here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_2ECF0(uint32_t pUnit)
{
	if (pUnit == 0 || rd32(pUnit, 0) != 2) return 0;
	const uint32_t pObject = rd32(pUnit, 0x14);
	return pObject != 0 ? rd32(pObject, 0) : 0;
}

// 1.13c D2Common+0x51FF0 (#10297)
// The max-damage skill roll of the 0x6BAA0 damage builder, same body as the vendored 1.10f max branch:
// an out-of-range skill id or a missing table answers 2; bit 0x10 of the row's +0x05 flags byte answers
// 0x4F930(0); with nSrcDam (+0x1A5) and nScaleSource both set, the source damage is the weapon's +0x18
// total when 0x1FB80 reports wield type 2, else the unit's +0x16 total, scaled by nSrcDam and divided
// by 128; then dwMaxDam (+0x1AC) and the level bonus over dwMaxLvlDam (+0x1C4, level in the dropped
// EAX, taken from this function's level argument) are added; a dwDmgSymPerCalc (+0x1D8) other than
// -1 that the formula evaluator accepts adds ApplyRatio(damage, formula, 100); the result is shifted
// left by nToHitShift (+0x1A4) mod 32.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_51FF0(uint32_t pUnit, int32_t nSkillId, int32_t nSkillLevel,
                                                  int32_t nScaleSource, const D2CommonBatch2F7A0Env* env)
{
	if (pUnit == 0) return 2;                       // retail faults in 0x31B90; not observable
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount ||
	    sgptDataTables->pSkillsTxt == nullptr) {
		return 2;
	}

	const D2SkillsTxt* pRow = &sgptDataTables->pSkillsTxt[nSkillId];
	if (((uint8_t)rd32(gdwBitMasks, 16) & rd8(pRow, 5)) != 0) {
		return env->sub4F930 != nullptr ? env->sub4F930(0) : 0;
	}

	int32_t nSource = 0;
	if (rd8(pRow, 0x1A5) != 0 && nScaleSource != 0) {
		D2UnitStrc* pUnitNative = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
		D2UnitStrc* pWeapon = D2Common_10434(pUnitNative, 0);
		if (pWeapon == nullptr) {
			nSource = env->sub38B70 != nullptr ? env->sub38B70(pUnit, 0x16) : 0;
		}
		else {
			D2InventoryStrc* pInventory = reinterpret_cast<D2InventoryStrc*>(static_cast<uintptr_t>(rd32(pUnit, 0x60)));
			const int32_t nWield = D2Common_1FB80_GetWieldType(pUnitNative, pInventory);
			nSource = nWield == 2
				? (env->sub38F60 != nullptr ? env->sub38F60((uint32_t)(uintptr_t)pWeapon, 0x18) : 0)
				: (env->sub38B70 != nullptr ? env->sub38B70(pUnit, 0x16) : 0);
		}

		const int32_t nProduct = (int32_t)((uint32_t)rd8(pRow, 0x1A5) * (uint32_t)nSource);
		nSource = nProduct / 128;                    // retail's sign-corrected arithmetic shift by 7
	}

	int32_t nDamage = nSource
		+ SKILLS_CalculateDamageBonusByLevel(nSkillLevel, (int*)pRow->dwMaxLvlDam)
		+ (int32_t)pRow->dwMaxDam;

	if (pRow->dwDmgSymPerCalc != 0xFFFFFFFFu) {
		const int32_t nFormula = env->sub51BF0 != nullptr
			? env->sub51BF0(pUnit, pRow->dwDmgSymPerCalc, nSkillId, nSkillLevel)
			: 0;
		if (nFormula != 0) {
			nDamage += DATATBLS_ApplyRatio(nDamage, nFormula, 100);
		}
	}

	return (int32_t)((uint32_t)nDamage << (pRow->nToHitShift & 0x1F));
}

// 1.13c D2Common+0x82E00 (#10299)
// Overlay record id lookup: a non-negative id below the overlay count answers the record's nTrans
// (+0x7C); everything else answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2common_82E00(int32_t nOverlayId)
{
	if (nOverlayId >= 0 && nOverlayId < sgptDataTables->nOverlayTxtRecordCount &&
	    sgptDataTables->pOverlayTxt != nullptr) {
		return rd8(&sgptDataTables->pOverlayTxt[nOverlayId], 0x7C);
	}
	return 0;
}

// 1.13c D2Common+0x4A3D0 (#10300)
// The profiler dump: a QueryPerformanceFrequency whose answer retail never reads, one configured log
// line (ProfCore.cpp line 0x4E), then one log line per profiling record (line 0x54) over the 23
// records of 0x78 bytes at 0x6FDE9EC8. The records' name/time/count operands are varargs the
// decompile does not recover; the record is handed to the seam, and the frequency line gets null.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4A3D0(const D2CommonBatch2F7A0Env* env)
{
	static const char kFile[] = "..\\Source\\D2Common\\Logging\\ProfCore.cpp";
	static const char kFrequency[] = "PROFILING DUMP FREQUENCY %d:\n";
	static const char kSlot[] = "FUNCTION: %30s, TIME %12d, COUNT %5d";

	uint32_t frequency[2] = { 0, 0 };                // LARGE_INTEGER local_8
	if (env->queryPerformanceFrequency != nullptr) {
		env->queryPerformanceFrequency(frequency);
	}

	if (env->sub12C00 != nullptr) {
		env->sub12C00(1, 2, 1, 0x80000, kFile, 0x4E, kFrequency, nullptr);
	}
	if (env->perfRecords == nullptr || env->sub12C00 == nullptr) {
		return;
	}
	for (uint32_t i = 0; i < 23; ++i) {
		env->sub12C00(1, 2, 1, 0x80000, kFile, 0x54, kSlot, env->perfRecords + i * 0x78);
	}
}

// 1.13c D2Common+0x24FF0 (#10607)
// Stat-array scan: with an out pointer set, *out starts at 1; a unit whose +0x5C stat list exists and
// whose +0x10 flag dword is negative takes 0x382F0(buffer, 0xCC, 0x40) (the stat id arrives in its
// register operand, handed over as the stat list), which fills up to 0x40 eight-byte pairs and answers
// the count. A count below 1 answers 0. Otherwise, if the out pointer is set, each pair's low byte is
// compared with the low byte of its dword's bits 8.. and the first pair where the low byte is smaller
// stores 0 and answers 1; the whole scan answers 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t D2Common_24FF0_StatScan(uint32_t pUnit, uint32_t* pOut,
                                                  const D2CommonBatch2F7A0Env* env)
{
	if (pOut != nullptr) *pOut = 1;

	const uint32_t pStatList = pUnit != 0 ? rd32(pUnit, 0x5C) : 0;
	if (pStatList == 0 || (int32_t)rd32(pStatList, 0x10) >= 0) return 0;

	uint8_t buffer[0x200] = { 0 };
	const int32_t nCount = env->sub382F0 != nullptr
		? env->sub382F0(buffer, 0xCC, 0x40, pStatList)
		: 0;
	if (nCount < 1) return 0;

	if (pOut != nullptr) {
		for (int32_t i = 0; i < nCount; ++i) {
			const uint32_t v = rd32(buffer, (uint32_t)(4 + i * 8));
			if ((int32_t)(v & 0xFF) < ((int32_t)v >> 8)) {
				*pOut = 0;
				return 1;
			}
		}
	}
	return 1;
}

// 1.13c D2Common+0x3B0A0 (#10613)
// The +0x08 dword of the argument; a null argument is retail's fatal 0x634, which never returns.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3B0A0(uint32_t pRecord)
{
	if (pRecord == 0) return 0;
	return rd32(pRecord, 8);
}

// 1.13c D2Common+0x22380 (#10617)
// Item hit class: item units (type 4) answer items.txt nHitClass (+0x13C) of their class id (+0x04).
// A null unit (fatal 0x1190), another type (fatal 0x1191) and a missing row (fatal 0x1193) answer 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t D2Common_22380_ItemHitClass(uint32_t pUnit)
{
	static_assert(offsetof(D2ItemsTxt, nHitClass) == 0x13C, "items.txt nHitClass");
	if (pUnit == 0 || rd32(pUnit, 0) != 4) return 0;
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord((int32_t)rd32(pUnit, 4));
	return pRow != nullptr ? pRow->nHitClass : 0;
}

// 1.13c D2Common+0x31EA0 (#10621)
// Player attack rating: 5 * dexterity + to-hit - 0x23. A null unit or one that is not a type-0 player
// is retail's fatal 0x107F. The +0x5C stat list is only read when it exists: with at least 20
// ItemStatCost rows the to-hit total (stat 19) is read, with at least 3 the dexterity total (stat 2).
// The class id (+0x04) then adds charstats[].dwToHitFactor (+0x3C) when it is in range. The totals go
// through the vendored STATLIST_GetTotalStat, the body of the 1.13c 0x38A80 (list in EBX, packed
// layer-0 id = stat id << 16 in EDI, ItemStatCost row on the stack).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t D2Common_PlayerAttackRating(uint32_t pUnit)
{
	static_assert(offsetof(D2CharStatsTxt, dwToHitFactor) == 0x3C, "charstats dwToHitFactor");
	if (pUnit == 0 || rd32(pUnit, 0) != 0) return 0;

	const uint32_t pStatList = rd32(pUnit, 0x5C);
	int32_t nToHit = 0;
	int32_t nDexterity = 0;
	if (pStatList != 0 && sgptDataTables->pItemStatCostTxt != nullptr) {
		D2StatListStrc* pList = reinterpret_cast<D2StatListStrc*>(static_cast<uintptr_t>(pStatList));
		if (sgptDataTables->nItemStatCostTxtRecordCount >= 20) {
			nToHit = STATLIST_GetTotalStat_6FDB63E0(
				pList,
				D2SLayerStatIdStrc::MakeFromStatId(STAT_TOHIT).nPackedValue,
				&sgptDataTables->pItemStatCostTxt[STAT_TOHIT]);
		}
		if (sgptDataTables->nItemStatCostTxtRecordCount >= 3) {
			nDexterity = STATLIST_GetTotalStat_6FDB63E0(
				pList,
				D2SLayerStatIdStrc::MakeFromStatId(STAT_DEXTERITY).nPackedValue,
				&sgptDataTables->pItemStatCostTxt[STAT_DEXTERITY]);
		}
	}

	int32_t nValue = nDexterity * 5 + nToHit - 0x23;

	const int32_t nClass = (int32_t)rd32(pUnit, 4);
	if (nClass >= 0 && nClass < sgptDataTables->nCharStatsTxtRecordCount &&
	    sgptDataTables->pCharStatsTxt != nullptr) {
		nValue += (int32_t)sgptDataTables->pCharStatsTxt[nClass].dwToHitFactor;
	}
	return nValue;
}

// 1.13c D2Common+0x3CC40 (#10622)
// Room-record propagation: both arguments' +0x10 record pointers are compared. When they differ,
// a non-null second record goes to 0x7C090 (retail reads it from EDI; BL, its second operand, has no
// visible producer on this path and is handed over as 0); then a non-null first record whose +0x0C
// word is non-zero is decremented, the 0x99BF8 callback runs, and the record is propagated with
// status 1 through the vendored DRLGACTIVATE_RoomExPropagateUnsetStatus (1.13c 0x7BB20).
extern "C"
EMSCRIPTEN_KEEPALIVE void DRLGACTIVATE_RoomUnsetAndPropagateStatus(uint32_t pFirst, uint32_t pSecond,
                                               const D2CommonBatch2F7A0Env* env)
{
	const uint32_t pA = pFirst != 0 ? rd32(pFirst, 0x10) : 0;
	const uint32_t pB = pSecond != 0 ? rd32(pSecond, 0x10) : 0;
	if (pA == pB) return;

	if (pB != 0 && env->sub7C090 != nullptr) {
		env->sub7C090(pB, 0);
	}
	if (pA != 0 && rd16(pA, 0xC) != 0) {
		wr16(pA, 0xC, (uint16_t)(rd16(pA, 0xC) - 1));
		if (env->cb99BF8 != nullptr) {
			env->cb99BF8();
		}
		DRLGACTIVATE_RoomExPropagateUnsetStatus(
			reinterpret_cast<D2DrlgRoomStrc*>(static_cast<uintptr_t>(pA)), 1);
	}
}
