// 1.13c D2Common implementation batch: +0x4E7A0.
// Passive-stat formula evaluator. The row selects one of three (resist, resist-max) stat-id pairs,
// finds the first matching entry in the skill's five passive-stat slots, and evaluates the calc byte
// code at the matching dwPassiveCalc offset. The evaluator itself (retail ordinal 10253) is pinned
// to the skills code block and its callback table, so the seam only carries the expression span and
// a three-dword {context, skill, level} user block.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/SkillsTbls.h"

static_assert(offsetof(D2SkillsTxt, nPassiveStat) == 0x98, "skills.txt nPassiveStat");
static_assert(offsetof(D2SkillsTxt, dwPassiveCalc) == 0xA4, "skills.txt dwPassiveCalc");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t A(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }
}

// Ordinal 10253 seam: evaluate one expression over the pinned skills calc block.
struct D2Common4E7A0Env
{
	int32_t (*eval_expression)(uint32_t pExpression, int32_t nExpressionSize, void* pUserData);
};

// 1.13c D2Common+0x4E7A0 (#10548): evaluate a passive-stat formula.
// nVariant 0/1/2 picks stat pairs (0x156,0x159), (0x157,0x15A), (0x158,0x15B); anything else is 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t D2haveColdResMalus(uint32_t nContext, int32_t nSkillId, int32_t nLevel,
                                                  int32_t nVariant, const D2Common4E7A0Env* pEnv)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
		return 0;
	if (!sgptDataTables->pSkillsTxt || nLevel < 1)
		return 0;

	int16_t nStatA;
	int16_t nStatB;
	switch (nVariant)
	{
	case 0: nStatA = 0x156; nStatB = 0x159; break;
	case 1: nStatA = 0x157; nStatB = 0x15A; break;
	case 2: nStatA = 0x158; nStatB = 0x15B; break;
	default: return 0;
	}

	const D2SkillsTxt* pRow = sgptDataTables->pSkillsTxt + nSkillId;
	int32_t nSlot = 0;
	while (nSlot < 5 && pRow->nPassiveStat[nSlot] != nStatA && pRow->nPassiveStat[nSlot] != nStatB)
		++nSlot;
	if (nSlot >= 5)
		return 0;

	const uint32_t nOffset = pRow->dwPassiveCalc[nSlot];
	if (!sgptDataTables->pSkillsCode || !pEnv || !pEnv->eval_expression)
		return 0;
	if (nOffset >= sgptDataTables->nSkillsCodeSize)
		return 0;

	uint32_t user[3] = { nContext, static_cast<uint32_t>(nSkillId), static_cast<uint32_t>(nLevel) };
	const uint32_t nExpr = A(sgptDataTables->pSkillsCode) + nOffset;
	const int32_t nSize = static_cast<int32_t>(sgptDataTables->nSkillsCodeSize - nOffset);
	return pEnv->eval_expression(nExpr, nSize, user);
}
