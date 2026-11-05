// 1.13c D2Common+0x5A810 (#10249) DATATBLS_RollRandomUniquePrefixString.
// fastcall(ECX = unit): a null unit gives the shared empty string. With a positive unique-prefix record count the unit's
// seed (+0x20 in retail) rolls an index (a mask for a power-of-two count, else the low dword modulo the count); a nonzero
// string id there goes through the D2Lang lookup, whose non-null answer is the result. Every other case is the empty string.
// The lookup is the seam (the vendored function passes D2LANG_GetStringFromTblIndex; the FN probe passes a hook recorder).
#include "D2DataTbls.h"
#include "D2Seed.h"
#include "Units/Units.h"

const Unicode* DATATBLS_RollRandomUniquePrefixStringSeam(D2UnitStrc* pUnit, const Unicode* (*pfnLookup)(short))
{
	if (!pUnit) return &sgptDataTables->wszDefault;
	const int32_t nCount = sgptDataTables->nUniquePrefixTxtRecordCount;
	if (nCount > 0)
	{
		const int32_t nIndex = (int32_t)SEED_RollLimitedRandomNumber(&pUnit->pSeed, nCount);
		const uint16_t wId = sgptDataTables->pUniquePrefixTxt[nIndex].wStringId;
		if (wId)
		{
			if (const Unicode* pString = pfnLookup((short)wId)) return pString;
		}
	}
	return &sgptDataTables->wszDefault;
}
