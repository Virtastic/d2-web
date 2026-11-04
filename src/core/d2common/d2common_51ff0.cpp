// D2Common 1.13c: the maximum physical damage of a skill (RVA 0x51FF0), retail record layout.
// The skills.txt row is read through the engine's sgptDataTables (D2SkillsTxt is the 0x23C retail row).
// Callees are seams; a missing seam answers 0.
#include <cstdint>
#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2Skills.h"

struct D2Common51FF0Env
{
	int32_t  (*sub4F930)(uint32_t pUnit, int32_t nMin);                    // EAX = unit, stack = 0
	uint32_t (*sub31B90)(uint32_t pUnit, int32_t nIndex);                  // the unit's weapon (index 0)
	int32_t  (*sub1FB80)(uint32_t pUnit, uint32_t pInventory);             // wield type; inventory = unit +0x60
	int32_t  (*sub38F60)(uint32_t pItem, int32_t nStatId);                 // the weapon's stat 0x18
	int32_t  (*sub38B70)(uint32_t pUnit, int32_t nStatId, int32_t nLayer); // the unit's stat 0x16, layer 0
	int32_t  (*sub51BF0)(uint32_t pUnit, uint32_t nCalc, int32_t nSkillId, int32_t nSkillLevel);
};

// 1.13c D2Common+0x51FF0 (#10297): stdcall (unit, skill id, skill level, scale source), ret 0x10.
// An out-of-range skill id or a null row answers 2. When the row's +0x05 flags byte shares a bit with
// the low byte of Fog's gdwBitMasks[1] (0x02), the answer is 0x4F930(unit, 0). Otherwise: with nSrcDam
// (+0x1A5) and the scale source both set, the source is the weapon's stat 0x18 when there is a weapon
// and its wield type is 2, else the unit's stat 0x16; it is scaled by nSrcDam and divided by 128 toward
// zero. Then dwMaxDam (+0x1AC) and the level bonus over dwMaxLvlDam (+0x1C4) are added. A formula
// (+0x1D8) other than -1 whose value is nonzero adds ApplyRatio(damage, value, 100). The result is
// shifted left by nToHitShift (+0x1A4).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_51FF0(uint32_t pUnit, int32_t nSkillId, int32_t nSkillLevel,
                                                  int32_t nScaleSource, const D2Common51FF0Env* env)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount || sgptDataTables->pSkillsTxt == nullptr)
		return 2;

	const D2SkillsTxt* pRow = &sgptDataTables->pSkillsTxt[nSkillId];
	const uint8_t* b = reinterpret_cast<const uint8_t*>(pRow);
	if (((uint8_t)gdwBitMasks[1] & b[5]) != 0)
		return env->sub4F930 ? env->sub4F930(pUnit, 0) : 0;

	int32_t nDamage = 0;
	if (pRow->nSrcDam != 0 && nScaleSource != 0)
	{
		const uint32_t pItem = env->sub31B90 ? env->sub31B90(pUnit, 0) : 0;
		int32_t nSource;
		const uint32_t pInventory = pItem ? *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pUnit) + 0x60) : 0;
		if (pItem && env->sub1FB80 && env->sub1FB80(pUnit, pInventory) == 2)
			nSource = env->sub38F60 ? env->sub38F60(pItem, 0x18) : 0;
		else
			nSource = env->sub38B70 ? env->sub38B70(pUnit, 0x16, 0) : 0;
		nDamage = (int32_t)((uint32_t)pRow->nSrcDam * (uint32_t)nSource) / 128;
	}

	nDamage += SKILLS_CalculateDamageBonusByLevel(nSkillLevel, (int*)pRow->dwMaxLvlDam) + (int32_t)pRow->dwMaxDam;

	if (pRow->dwDmgSymPerCalc != 0xFFFFFFFFu)
	{
		const int32_t nFormula = env->sub51BF0 ? env->sub51BF0(pUnit, pRow->dwDmgSymPerCalc, nSkillId, nSkillLevel) : 0;
		if (nFormula != 0)
			nDamage += DATATBLS_ApplyRatio(nDamage, nFormula, 100);
	}

	return (int32_t)((uint32_t)nDamage << (pRow->nToHitShift & 0x1F));
}
