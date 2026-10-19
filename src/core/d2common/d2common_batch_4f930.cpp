// D2Common implementation batch 4f930: six UNMAPPED rows translated from the pinned 1.13c decompiles.
// The two skill rows are the vendored D2MOO bodies, which match the 1.13c decompiles line for line; the
// DRLG generators reach the level list through the engine's D2DrlgStrc fields (retail's +0x47C list
// head, +0x1AC next link and +0x1D0 level id are pLevel / pNextLevel / nLevelId) and its +0x450
// difficulty byte is nDifficulty. Linker/room helpers that are still UNMAPPED are marked partial.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "DataTbls/LevelsTbls.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgOutPlace.h"
#include "Units/Units.h"

static_assert(offsetof(D2LevelDefBin, dwSizeX) == 0x0C, "levels.bin size X");
static_assert(offsetof(D2LevelDefBin, dwSizeY) == 0x18, "levels.bin size Y");
static_assert(offsetof(D2LevelDefBin, dwOffsetX) == 0x24, "levels.bin offset X");

// 1.13c D2Common+0x4F930 (#?): the unarmed/kick base damage of 0x52340's skill damage formulas.
// A player is strength + dexterity - 20; every other unit (or no stat list) is 3 * level. The result
// is clamped to at least 1 and scaled by 256: divided by 4 when nMin is set (minimum damage) and by 3
// otherwise (maximum damage). The unit arrives in EAX in retail and the selector is the stack argument.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_4f930(D2UnitStrc* pUnit, int32_t nMin)
{
	int32_t nDamage;

	if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
	{
		nDamage = STATLIST_UnitGetStatValue(pUnit, STAT_STRENGTH, 0)
			+ STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0) - 20;
	}
	else
	{
		nDamage = 3 * STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0);
	}

	if (nDamage < 1)
	{
		nDamage = 1;
	}

	return nMin != 0 ? (nDamage / 4) << 8 : (nDamage / 3) << 8;
}

// 1.13c D2Common+0x528B0 (#?): remove a native skill (owner GUID -1) from a unit: toggle its passive
// state off, clear the left/right/used skill slots that point at it, unlink the matching entry from the
// skill list (freeing it when bDecrementAndCheckSkillLevel is clear or the level drops to 0) and refresh
// the unit. The unit arrives in EBX in retail. The vendored body is the same function.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_528b0(D2UnitStrc* pUnit, int32_t nSkillId,
	int32_t bDecrementAndCheckSkillLevel)
{
	D2COMMON_SKILLS_RemoveSkill_6FDAFFF0(pUnit, nSkillId, bDecrementAndCheckSkillLevel, __FILE__, __LINE__);
}

// 1.13c D2Common+0x4FCB0 (#?): SKILLS_GetBonusSkillLevel. Starts from the skill's nLevelBonus, adds the
// shrine-skill state bonus (2), stat 0x7F (all skills), and only for a player whose class matches the
// records' nCharClass the class skill 0x53 plus, when the skilldesc's page is set, the tab skill 0xBC.
// Stat 0x61 (non-class skill) is added clamped to 3 on the class path and on the non-player path with a
// positive skill level. Stat 0x7E (element skill) is added when nEType is set, and stat 0x6B (single
// skill) always. The skill is in EAX and the unit in ESI in retail; the vendored body is the same.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_4fcb0(D2UnitStrc* pUnit, D2SkillStrc* pSkill)
{
	return SKILLS_GetBonusSkillLevel(pUnit, pSkill);
}

// 1.13c D2Common+0x17930 (#?): difficulty-4 DRLG generator. Retail registers two level linkers
// (0x16E10) and links the level pairs 0x6F/0x70, 0x6E/0x6F and 0x6D/0x6E through 0x16CF0, then links
// 0x6F/0x70 through 0x17760.
// partial: 0x16E10, 0x16CF0 and 0x17760 are UNMAPPED, so only the parameter the decompiler recovers
// (the DRLG object in EAX) is fixed here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_17930(D2DrlgStrc* pDrlg)
{
	(void)pDrlg;
}

// 1.13c D2Common+0x17990 (#?): difficulty-3 DRLG generator. Registers two linkers (0x16E10), finds or
// allocates level 0x68 and ORs a level dword with a global, then links 0x67/0x6A through 0x17760.
// partial: the linker helper and the image global at the level's +0x14 dword (an unresolved constant)
// are not translated; the level lookup itself is the vendored 0x2D9B0/0x2DD80 pair.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_17990(D2DrlgStrc* pDrlg)
{
	if (!pDrlg)
	{
		return;
	}

	// Retail ORs level+0x14 (the level seed in the engine layout) with global 0x6fdf146c.
	(void)DRLG_GetLevel(pDrlg, 0x68);
}

// 1.13c D2Common+0x17A00 (#?): difficulty-2 DRLG generator. Finds or allocates level 0x4B, stores the
// levels.txt row's offset X/Y into its position and the sizes for the current difficulty into its
// width/height, generates the level's jungles, then links 0x4B/0x53 through 0x16CF0 and 0x17760.
// partial: 0x15980 (room placement), 0x16CF0 and 0x17760 are UNMAPPED and not translated.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_17a00(D2DrlgStrc* pDrlg)
{
	if (!pDrlg)
	{
		return;
	}

	D2DrlgLevelStrc* pLevel = DRLG_GetLevel(pDrlg, 0x4B);

	const D2LevelDefBin* pLevelDef = DATATBLS_GetLevelDefRecord(0x4B);
	if (pLevelDef)
	{
		const uint32_t nDifficulty = pDrlg->nDifficulty;
		if (nDifficulty < 3)
		{
			pLevel->nPosX = (int32_t)pLevelDef->dwOffsetX;
			pLevel->nPosY = (int32_t)pLevelDef->dwOffsetY;
			pLevel->nWidth = (int32_t)pLevelDef->dwSizeX[nDifficulty];
			pLevel->nHeight = (int32_t)pLevelDef->dwSizeY[nDifficulty];
		}
	}

	(void)DRLG_GenerateJungles(pLevel);
}
