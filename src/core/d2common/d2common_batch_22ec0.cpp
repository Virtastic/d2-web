// 1.13c D2Common Stage A implementation batch: 0x22EC0, 0x4DF20, 0x23360, 0x51870, 0x52100,
// 0x12C00, 0x22B70, 0x2EBB0, 0x4DEE0, 0x6CB20, 0x1AA00, 0x3B440.
// Bodies are translated from the pinned 1.13c decompiles (build-wasm/ghidra-d2common/decomp).
// Records keep retail's 32-bit layout: fields are read at their retail byte offsets, and helpers
// that have no recovered body yet are reached through a per-function env seam (null seam = the
// retail call is fatal or unavailable, so the caller returns the function's neutral value).
// Retail's fatal asserts (_exit) are modelled as the function's neutral return.
#include <cstdarg>
#include <cstddef>
#include <cstdint>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2Monsters.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/LevelsTbls.h"
#include "DataTbls/MonsterTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"

static_assert(sizeof(D2ItemsTxt) == 0x1A8, "items.txt record stride");
static_assert(offsetof(D2ItemsTxt, dwWeapClass) == 0xC0, "items.txt dwWeapClass");
static_assert(offsetof(D2ItemsTxt, dwMaxStack) == 0xE8, "items.txt dwMaxStack");
static_assert(offsetof(D2ItemsTxt, wType) == 0x11E, "items.txt wType");
static_assert(sizeof(D2ItemTypesTxt) == 0xE4, "itemtypes.txt record stride");
static_assert(offsetof(D2ItemTypesTxt, nNormal) == 0x16, "itemtypes.txt nNormal");
static_assert(sizeof(D2MonStatsTxt) == 0x1A8, "monstats.txt record stride");
static_assert(offsetof(D2MonStatsTxt, nBaseId) == 0x02, "monstats.txt nBaseId");
static_assert(sizeof(D2SkillsTxt) == 0x23C, "skills.txt record stride");
static_assert(offsetof(D2SkillsTxt, nToHitShift) == 0x1A4, "skills.txt nToHitShift");
static_assert(offsetof(D2SkillsTxt, nSrcDam) == 0x1A5, "skills.txt nSrcDam");
static_assert(offsetof(D2SkillsTxt, dwMinDam) == 0x1A8, "skills.txt dwMinDam");
static_assert(offsetof(D2SkillsTxt, dwMinLvlDam) == 0x1B0, "skills.txt dwMinLvlDam");
static_assert(offsetof(D2SkillsTxt, dwDmgSymPerCalc) == 0x1D8, "skills.txt dwDmgSymPerCalc");
static_assert(sizeof(D2LevelDefBin) == 0x9C, "leveldefs.bin record stride");
static_assert(offsetof(D2ItemDataTbl, pItemsTxt) == 0x04, "items.txt table pointer");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline const uint8_t* PC(uint32_t a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline uint32_t RdU32(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
	inline int32_t RdI32(const uint8_t* b, int o) { return *reinterpret_cast<const int32_t*>(b + o); }
	inline int16_t RdI16(const uint8_t* b, int o) { return *reinterpret_cast<const int16_t*>(b + o); }
	inline uint16_t RdU16(const uint8_t* b, int o) { return *reinterpret_cast<const uint16_t*>(b + o); }
	inline uint8_t RdU8(const uint8_t* b, int o) { return *(b + o); }
}

// ---------------------------------------------------------------------------------------------
// 0x22EC0: items.txt dwWeapClass of nItemId equals nWeapClass. Out-of-range is retail's fatal
// assert (0x1ef / 0x8ec); it returns the neutral false here.
// 1.13c D2Common+0x22EC0 (#10553)
bool D2Common_22EC0_CheckWeaponClassByItemId(uint32_t nItemId, int32_t nWeapClass)
{
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nItemId));
	if (!pRow)
		return false;
	return pRow->dwWeapClass == static_cast<uint32_t>(nWeapClass);
}

// ---------------------------------------------------------------------------------------------
// 0x22B70: items.txt dwMaxStack of nItemId. Out-of-range is retail's fatal assert (0x1ef/0xd50).
// 1.13c D2Common+0x22B70 (#10743)
extern "C" void D2Common_1B0D_Exit(int32_t);
uint32_t D2Common_22B70_GetMaxStack(uint32_t nItemId)
{
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nItemId));
	if (!pRow)
		D2Common_1B0D_Exit(-1);
	return pRow->dwMaxStack;
}

// ---------------------------------------------------------------------------------------------
// 0x23360: itemtypes.txt nNormal for an item unit's wType[0]. Non-item units, an invalid item id
// (retail assert 0x67e in the inlined 0x230F0) and out-of-range types all yield 1.
// 1.13c D2Common+0x23360 (#10556)
uint8_t D2Common_23360_CanBeNormal(uint32_t pUnit)
{
	if (!pUnit || RdU32(PC(pUnit), 0x00) != 4)
		return 1;

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(RdU32(PC(pUnit), 0x04)));
	if (!pItemsTxtRecord)
		return 1;

	const int32_t nType = pItemsTxtRecord->wType[0];
	if (nType < 0 || nType >= sgptDataTables->nItemTypesTxtRecordCount)
		return 1;
	if (!sgptDataTables->pItemTypesTxt)
		return 1;
	return sgptDataTables->pItemTypesTxt[nType].nNormal;
}

// ---------------------------------------------------------------------------------------------
// 0x4DF20 / 0x4DEE0: store a dword into the block at +0x1C / +0x20. Retail asserts on a null block.
// 1.13c D2Common+0x4DF20 (#10554)
void D2Common_4DF20_SetField1C(uint32_t pBlock, uint32_t dwValue)
{
	if (!pBlock)
		return;
	W(P(pBlock), 0x1C) = dwValue;
}

// 1.13c D2Common+0x4DEE0 (#10748)
void D2Common_4DEE0_SetField20(uint32_t pBlock, uint32_t dwValue)
{
	if (!pBlock)
		return;
	W(P(pBlock), 0x20) = dwValue;
}

// ---------------------------------------------------------------------------------------------
// 0x3B440: flag bit 1 of the dword at +0x34. Retail asserts on a null block (0x42f).
// 1.13c D2Common+0x3b440 (#10752)
uint32_t D2Common_3B440_GetFlag2(uint32_t pBlock)
{
	if (!pBlock)
		return 0;
	return RdU32(PC(pBlock), 0x34) & 2;
}

// ---------------------------------------------------------------------------------------------
// 0x2EBB0: retail's unit-field validator. Null block (0xfa5), null field +0x14 (0xfa7) and a
// non-zero first dword (0xfa8) are all fatal asserts; only "block set, +0x14 set, type 0" passes.
// The assert machinery has no wasm equivalent, so every path returns void.
// 1.13c D2Common+0x2ebb0 (#10746)
void D2Common_2EBB0_ValidateUnitFields(uint32_t pBlock)
{
	if (!pBlock)
		return;
	if (RdU32(PC(pBlock), 0x14) == 0)
		return;
	if (RdU32(PC(pBlock), 0x00) == 0)
		return;
}

// ---------------------------------------------------------------------------------------------
// 0x6CB20: leveldefs.bin row for a level id, null out of range.
// 1.13c D2Common+0x6CB20 (#10749)
D2LevelDefBin* D2Common_6CB20_GetLevelDefRecord(int32_t nLevelId)
{
	return DATATBLS_GetLevelDefRecord(nLevelId);
}

// ---------------------------------------------------------------------------------------------
// 0x12C00: variadic log line. Retail guards on a global log FILE*, takes the log critical section,
// calls the formatter at 0x12AF0 and leaves the section; the formatter itself re-checks its own
// enable flag. Both Windows-side pieces (lock, FILE*) live behind the seam.
struct D2Common12C00Env
{
	uint32_t* pLogFile;		//0x00 global DAT_6fdf1470: non-null = logging enabled
	void (*enter)(void);		//0x04 enter the log critical section
	void (*leave)(void);		//0x08 leave the log critical section
	void (*write_v)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const char*, va_list); //0x0C 0x12AF0
};

// 1.13c D2Common+0x12C00 (#10570)
extern "C"
void D2Common_12C00_LogV(const D2Common12C00Env* pEnv, uint32_t a1, uint32_t a2,
                                    uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6,
                                    const char* pFormat, ...)
{
	if (!pEnv || !pEnv->pLogFile || *pEnv->pLogFile == 0)
		return;

	if (pEnv->enter)
		pEnv->enter();

	va_list args;
	va_start(args, pFormat);
	if (pEnv->write_v)
		pEnv->write_v(a1, a2, a3, a4, a5, a6, pFormat, args);
	va_end(args);

	if (pEnv->leave)
		pEnv->leave();
}

// ---------------------------------------------------------------------------------------------
// 0x51870: placement search around a unit. Unit coordinates are read exactly like 0x1050/0x1080:
// unit types 2/4/5 keep a dword at path+0xC / +0x10, everything else a word at path+0x2 / +0x6.
namespace
{
	inline uint32_t UnitCoordX(const int32_t* pUnit)
	{
		const int32_t nType = pUnit[0];
		const uint32_t pData = static_cast<uint32_t>(pUnit[0xB]);
		if (nType == 2 || nType == 4 || nType == 5)
			return RdU32(PC(pData), 0x0C);
		return pData ? RdU16(PC(pData), 0x02) : 0;
	}

	inline uint32_t UnitCoordY(const int32_t* pUnit)
	{
		const int32_t nType = pUnit[0];
		const uint32_t pData = static_cast<uint32_t>(pUnit[0xB]);
		if (nType == 2 || nType == 4 || nType == 5)
			return RdU32(PC(pData), 0x10);
		return pData ? RdU16(PC(pData), 0x06) : 0;
	}
}

struct D2Common51870Env
{
	bool (*is_in_melee_range)(int32_t*, int32_t*, int32_t);						//0x00 0x326C0
	int32_t (*distance_to_coords)(int32_t*, uint32_t, uint32_t);				//0x04 0x7F5E0
	uint32_t (*room_test)(int32_t*, uint32_t, uint32_t, uint32_t, uint32_t);	//0x08 0x4CE90
	int32_t (*unit_size_x)(int32_t*);											//0x0C 0x30F80
	int32_t* (*place)(int32_t*, uint32_t*, uint32_t*, int32_t, uint32_t, uint32_t, int32_t, uint32_t, int32_t); //0x10 0x4D140
	bool (*finalize)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t*, uint32_t); //0x14 0x4E750
	bool (*end)(void);															//0x18 0x4F120
};

// 1.13c D2Common+0x51870 (#10566)
bool D2Common_51870_FindPlacement(int32_t* pUnit, int32_t* pTarget, uint32_t* pOutX, uint32_t* pOutY,
                                  const D2Common51870Env* pEnv)
{
	if (!pEnv || !pUnit)
		return false;

	if (pTarget && pEnv->is_in_melee_range && pEnv->is_in_melee_range(pUnit, pTarget, 0))
		return false;

	const uint32_t nUnitX = UnitCoordX(pUnit);
	const uint32_t nUnitY = UnitCoordY(pUnit);

	uint32_t nTargetX;
	uint32_t nTargetY;
	if (!pTarget)
	{
		nTargetX = RdU16(PC(static_cast<uint32_t>(pUnit[0xB])), 0x10);
		nTargetY = RdU16(PC(static_cast<uint32_t>(pUnit[0xB])), 0x12);
	}
	else
	{
		nTargetX = static_cast<uint32_t>(pTarget[0]);
		if (nTargetX == 2 || (nTargetX > 3 && nTargetX < 6))
		{
			uint32_t pData = static_cast<uint32_t>(pTarget[0xB]);
			nTargetX = RdU32(PC(pData), 0x0C);
			nTargetY = RdU32(PC(pData), 0x10);
		}
		else
		{
			const uint32_t pData = static_cast<uint32_t>(pTarget[0xB]);
			nTargetX = pData ? RdU16(PC(pData), 0x02) : 0;
			nTargetY = pData ? RdU16(PC(pData), 0x06) : 0;
		}
	}

	uint32_t nNewX = nTargetX;
	const int32_t nScale = pEnv->distance_to_coords ? pEnv->distance_to_coords(pUnit, nTargetX, nTargetY) : 0;
	if (nScale == 0)
		return false;

	uint32_t nNewY = nTargetY;
	nNewX = static_cast<uint32_t>(static_cast<int32_t>(nTargetX - nUnitX) * 2 / nScale + static_cast<int32_t>(nTargetX));
	nNewY = static_cast<uint32_t>(static_cast<int32_t>(nTargetY - nUnitY) * 2 / nScale + static_cast<int32_t>(nTargetY));

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(reinterpret_cast<D2UnitStrc*>(pUnit));
	if (!pRoom)
		return false;

	if (!pEnv->room_test)
		return false;
	const uint32_t nCollision = pEnv->room_test(reinterpret_cast<int32_t*>(pRoom), nNewX, nNewY,
	                                            RdU32(PC(static_cast<uint32_t>(pUnit[0xB])), 0x48), 0x1C09);
	int32_t* pEntry = nullptr;
	if (nCollision != 0)
	{
		const int32_t nSizeX = pEnv->unit_size_x ? pEnv->unit_size_x(reinterpret_cast<int32_t*>(pRoom)) : 0;
		// retail seeds the in/out point with the TARGET coords (not the extrapolated ones)
		nNewX = nTargetX;
		nNewY = nTargetY;
		pEntry = pEnv->place ? pEnv->place(reinterpret_cast<int32_t*>(pRoom), &nNewX, &nNewY, nSizeX,
		                                   0x1C09, 0x1C09, 0, 0x32, 1)
		                     : nullptr;
		if (!pEntry)
			return false;
	}

	const uint32_t nLastX = UnitCoordX(pUnit);
	const uint32_t nLastY = UnitCoordY(pUnit);
	if (!pEnv->finalize || !pEnv->finalize(nLastX, nLastY, nNewX, nNewY, pEntry, 0x804))
		return false;

	*pOutX = nNewX;
	*pOutY = nNewY;
	return pEnv->end ? pEnv->end() : false;
}

// ---------------------------------------------------------------------------------------------
// 0x52100: minimum physical damage of a skill (D2MOO's SKILLS_GetMinPhysDamage body matches the
// 1.13c row arithmetic field for field). Unrecovered helper rows are behind the seam.
struct D2Common52100Env
{
	int32_t (*kick_damage)(int32_t);												//0x00 0x4F930
	uint32_t (*get_item)(uint32_t, int32_t);										//0x04 0x31B90
	int32_t (*wield_type)(uint32_t, uint32_t);										//0x08 0x1FB80
	int32_t (*get_stat)(uint32_t, int32_t);											//0x0C 0x38B70
	int32_t (*min_damage)(uint32_t, int32_t);										//0x10 0x38ED0
	int32_t (*damage_bonus_by_level)(int32_t, const int32_t*);						//0x14 0x4DDB0
	int32_t (*evaluate_formula)(uint32_t, uint32_t, int32_t, uint32_t);				//0x18 0xA1BF0
};

// 1.13c D2Common+0x52100 (#10567)
int32_t D2Common_52100_SkillsGetMinPhysDamage(uint32_t pUnit, int32_t nSkillId, uint32_t nSkillLevel,
                                              int32_t bUseWeapon, const D2Common52100Env* pEnv)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
		return 1;
	if (!sgptDataTables->pSkillsTxt)
		return 1;
	const uint8_t* pRow = reinterpret_cast<const uint8_t*>(&sgptDataTables->pSkillsTxt[nSkillId]);

	if (RdU8(pRow, 0x05) & gdwBitMasks[4])
		return pEnv && pEnv->kick_damage ? pEnv->kick_damage(1) : 1;

	int32_t nDamage = 0;
	if (RdU8(pRow, 0x1A5) != 0 && bUseWeapon != 0 && pEnv)
	{
		const uint32_t pItem = pEnv->get_item ? pEnv->get_item(pUnit, 0) : 0;
		if (!pItem)
		{
			nDamage = pEnv->get_stat ? pEnv->get_stat(pUnit, 0x15) : 0;
		}
		else
		{
			const uint32_t pInventory = RdU32(PC(pUnit), 0x60);
			if ((pEnv->wield_type ? pEnv->wield_type(pUnit, pInventory) : 0) == 2)
				nDamage = pEnv->min_damage ? pEnv->min_damage(pItem, 1) : 0;
			else
				nDamage = pEnv->get_stat ? pEnv->get_stat(pUnit, 0x15) : 0;
		}
		nDamage = static_cast<int32_t>(RdU8(pRow, 0x1A5)) * nDamage / 128;
	}

	const int32_t* pMinLvlDam = reinterpret_cast<const int32_t*>(pRow + 0x1B0);
	nDamage += RdI32(pRow, 0x1A8);
	nDamage += pEnv && pEnv->damage_bonus_by_level ? pEnv->damage_bonus_by_level(static_cast<int32_t>(nSkillLevel), pMinLvlDam) : 0;

	const uint32_t nFormula = RdU32(pRow, 0x1D8);
	if (nFormula != 0xFFFFFFFF && pEnv && pEnv->evaluate_formula)
	{
		const int32_t nBonus = pEnv->evaluate_formula(pUnit, nFormula, nSkillId, nSkillLevel);
		if (nBonus != 0)
			nDamage += DATATBLS_ApplyRatio(nBonus, nDamage, 100);
	}

	return nDamage << (RdU8(pRow, 0x1A4) & 0x1F);
}

// ---------------------------------------------------------------------------------------------
// 0x1AA00: minion spawn info (base id -> id / x / y / spawn mode). The 1.13c body is the same
// switch as D2MOO's MONSTERS_GetMinionSpawnInfo; ids are the monster ids the row validates.
// Retail's incoming-EBX chain id is modelled as the local nChainId, set only by the rows that
// visibly call DATATBLS_GetMonsterChainInfo. Unrecovered helpers 0x10B0 and the fatal assert are
// behind the seam; 0x1930/0x1A770/0x2FE10/0x53850/0x665C0 are the vendored 1.13c rows.
struct D2Common1AA00Env
{
	uint32_t (*seed_roll)(uint32_t, uint32_t);	//0x00 0x10B0 SEED_RollLimitedRandomNumber
	void (*fatal)(int32_t);						//0x04 retail assert + _exit
};

// 1.13c D2Common+0x1aa00 (#10751)
void D2Common_1AA00_MonstersGetMinionSpawnInfo(uint32_t pMonster, int32_t* pId, uint32_t* pX,
                                               uint32_t* pY, uint32_t* pSpawnMode, uint32_t nDifficulty,
                                               int32_t (*pfSpawnClassCallback)(uint32_t),
                                               const D2Common1AA00Env* pEnv)
{
	int32_t* pUnit = reinterpret_cast<int32_t*>(pMonster);
	int32_t nBaseId = -1;
	if (pUnit && pUnit[0] == 1)
	{
		const int32_t nClassId = pUnit[1];
		nBaseId = nClassId;
		if (nClassId >= 0 && nClassId < sgptDataTables->nMonStatsTxtRecordCount)
		{
			const uint8_t* pRow = reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonStatsTxt[nClassId]);
			if (pRow)
				nBaseId = RdI16(pRow, 0x02);
		}
	}
	if (nBaseId < 0 || nBaseId >= sgptDataTables->nMonStatsTxtRecordCount)
		nBaseId = -1;

	int32_t nChainId = 0;

	switch (nBaseId)
	{
	case 0x0CE: // crownest1
		DATATBLS_GetMonsterChainInfo(pUnit ? pUnit[1] : -1, nullptr, &nChainId);
		*pId = MONSTERS_GetClassIdFromMonsterChain(MONSTERS_ValidateMonsterId(0x0F), nChainId);
		*pX = UnitCoordX(pUnit);
		*pY = UnitCoordY(pUnit) + 3;
		*pSpawnMode = 1;
		break;

	case 0x0E4: // sarcophagus
		*pId = D2Common_11063(UNITS_GetRoom(reinterpret_cast<D2UnitStrc*>(pUnit)), MONSTERS_ValidateMonsterId(0x60));
		*pX = UnitCoordX(pUnit);
		*pY = UnitCoordY(pUnit) + 2;
		*pSpawnMode = 1;
		break;

	case 0x10B: // bloodraven
		*pId = MONSTERS_ValidateMonsterId(0x06);
		*pX = RdU16(PC(static_cast<uint32_t>(pUnit[0xB])), 0x10);
		*pY = RdU16(PC(static_cast<uint32_t>(pUnit[0xB])), 0x12);
		*pSpawnMode = 8;
		break;

	case 0x11C: // maggotqueen1
		DATATBLS_GetMonsterChainInfo(pUnit ? pUnit[1] : -1, nullptr, &nChainId);
		*pId = MONSTERS_GetClassIdFromMonsterChain(MONSTERS_ValidateMonsterId(0x44), nChainId);
		*pX = UnitCoordX(pUnit) + 8;
		*pY = UnitCoordY(pUnit);
		*pSpawnMode = 8;
		break;

	case 0x12A: // vilemother1
		DATATBLS_GetMonsterChainInfo(pUnit ? pUnit[1] : -1, nullptr, &nChainId);
		*pId = MONSTERS_GetClassIdFromMonsterChain(MONSTERS_ValidateMonsterId(0x12D), nChainId);
		*pX = RdU16(PC(static_cast<uint32_t>(pUnit[0xB])), 0x10);
		*pY = RdU16(PC(static_cast<uint32_t>(pUnit[0xB])), 0x12);
		*pSpawnMode = 1;
		break;

	case 0x141: // evilhole1
		*pId = (pUnit && pUnit[1] == 0x2C7) ? MONSTERS_ValidateMonsterId(0x2C8) : MONSTERS_ValidateMonsterId(0x13);
		*pX = UnitCoordX(pUnit);
		*pY = UnitCoordY(pUnit);
		*pSpawnMode = 1;
		break;

	case 0x14E: // suckernest1
		*pId = MONSTERS_GetClassIdFromMonsterChain(MONSTERS_ValidateMonsterId(0x72), nChainId);
		*pX = UnitCoordX(pUnit) - 2;
		*pY = UnitCoordY(pUnit) - 2;
		*pSpawnMode = 1;
		break;

	case 0x1E4: // minionspawner1
		*pId = MONSTERS_GetClassIdFromMonsterChain(MONSTERS_ValidateMonsterId(0x1C5), nChainId);
		*pX = UnitCoordX(pUnit);
		*pY = UnitCoordY(pUnit) + 3;
		*pSpawnMode = 1;
		break;

	case 0x20E: // nihlathakboss
	case 0x210: // evilhut
		if (!pfSpawnClassCallback)
		{
			*pId = sgptDataTables->nMonStatsTxtRecordCount > 0 ? 0 : -1;
			if (pEnv && pEnv->fatal)
				pEnv->fatal(0x1FC);
			return;
		}
		*pId = pfSpawnClassCallback(pMonster);
		*pX = UnitCoordX(pUnit) + 2;
		*pY = UnitCoordY(pUnit) + 2;
		*pSpawnMode = 1;
		break;

	case 0x219: // ancientstatue1
		*pId = MONSTERS_GetClassIdFromMonsterChain(MONSTERS_ValidateMonsterId(0x21C), nChainId);
		*pX = UnitCoordX(pUnit);
		*pY = UnitCoordY(pUnit) + 2;
		*pSpawnMode = 1;
		if (sgptDataTables->nStatesTxtRecordCount < 0x93)
			return; // retail returns before the room refresh
		{
			const uint32_t pMonsterData = static_cast<uint32_t>(pUnit[0x17]);
			if (pMonsterData && RdI32(PC(pMonsterData), 0x10) < 0)
			{
				const uint32_t pFlags = RdU32(PC(pMonsterData), 0x58);
				if (pFlags)
				{
					const uint32_t dwOld = RdU32(PC(pFlags), 0x10);
					W(P(pFlags), 0x10) = gdwBitMasks[18] | dwOld;
					if ((gdwBitMasks[18] & dwOld) == 0)
					{
						const int32_t nStates = sgptDataTables->nStatesTxtRecordCount;
						W(P(pFlags), 0x10 + ((nStates + 0x1F + ((nStates + 0x1F) >> 31 & 0x1F)) >> 5) * 4) |= gdwBitMasks[18];
						if (sgptDataTables->nStatesTxtRecordCount > 0x92 && sgptDataTables->pStatesTxt)
						{
							const uint32_t pStatesTxt = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(sgptDataTables->pStatesTxt));
							if ((RdU8(reinterpret_cast<const uint8_t*>(gdwBitMasks), 0) & RdU8(PC(pStatesTxt), 0x224A)) != 0)
								pUnit[0x32] |= 8;
						}
					}
				}
			}
		}
		UNITROOM_RefreshUnit(reinterpret_cast<D2UnitStrc*>(pUnit));
		break;

	case 0x220: // baalcrab
	{
		const uint32_t pSeed = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pUnit[8]));
		if (*pId == MONSTERS_ValidateMonsterId(0x23A))
		{
			*pX = (pEnv && pEnv->seed_roll ? pEnv->seed_roll(pSeed, 24) : 0) - 12 + UnitCoordX(pUnit);
			*pY = (pEnv && pEnv->seed_roll ? pEnv->seed_roll(pSeed, 24) : 0) - 12 + UnitCoordY(pUnit);
			*pSpawnMode = 1;
			return;
		}
		const uint32_t nRoll = pEnv && pEnv->seed_roll ? pEnv->seed_roll(pSeed, 2) : 0;
		*pId = MONSTERS_GetClassIdFromMonsterChain(MONSTERS_ValidateMonsterId(0x232), static_cast<int32_t>(nDifficulty + nRoll));
		*pX = (pEnv && pEnv->seed_roll ? pEnv->seed_roll(pSeed, 24) : 0) - 12 + UnitCoordX(pUnit);
		*pY = (pEnv && pEnv->seed_roll ? pEnv->seed_roll(pSeed, 24) : 0) - 12 + UnitCoordY(pUnit);
		*pSpawnMode = 4;
		break;
	}

	default:
		*pId = sgptDataTables->nMonStatsTxtRecordCount > 0 ? 0 : -1;
		*pX = 0;
		*pY = 0;
		*pSpawnMode = 1;
		break;
	}
}
