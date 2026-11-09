// 1.13c D2Common+0x6B2B0 (#10231): evaluate a missile calc expression. The expression evaluator (retail Fog #10253 through thunk
// 0x9270) arrives through an env function; the data-table pointer is sgptDataTables (+0x60 expression buffer, +0x64 size).
#include <cstdint>
#include "D2DataTbls.h"

struct D2Common_6B2B0_Env
{
	int32_t (*evaluate)(const FOGASTNodeStrc* pExpression, int32_t nExpressionSize, const uint32_t* pCalc);
};

extern "C" int32_t D2Common_6B2B0_EvaluateMissileFormula(const uint8_t* pMissile, uint32_t pOwner, uint32_t nCalc, int32_t nMissileId, int32_t nLevel, const D2Common_6B2B0_Env* env)
{
	auto rd = [](const uint8_t* p, int o) { return *reinterpret_cast<const int32_t*>(p + o); };
	if (pMissile)
	{
		if (rd(pMissile, 0) != 3) return 0;
		if (nMissileId < 0) nMissileId = rd(pMissile, 4);
		if (nLevel < 0)
		{
			const uint8_t* data = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(rd(pMissile, 0x14))));
			nLevel = data ? *reinterpret_cast<const int16_t*>(data + 0xC) : 0;
		}
	}
	if (sgptDataTables->pMissCode && nCalc < sgptDataTables->nMissCodeSize)
	{
		const uint32_t ctx[4] = { static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pMissile)), pOwner, static_cast<uint32_t>(nMissileId), static_cast<uint32_t>(nLevel) };
		return env->evaluate(&sgptDataTables->pMissCode[nCalc], static_cast<int32_t>(sgptDataTables->nMissCodeSize - nCalc), ctx);
	}
	return 0;
}
