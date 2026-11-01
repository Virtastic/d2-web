// 1.13c D2Common+0x4E5E0 (#10832): skills.txt row nSkillId gives its value at nLevel, the interpolation of
// D2Common_11033 over dwParam[4] and dwParam[5]. A bad id or a non-positive level yields 0.
#include <cstdint>
#include "D2DataTbls.h"

int32_t D2Common_11033(int32_t nLevel, int32_t nParam, int32_t nMax);

extern "C" int32_t D2Common_4E5E0_SkillParamAtLevel(int32_t nLevel, int32_t nSkillId)
{
	const D2SkillsTxt* pTxt = DATATBLS_GetSkillsTxtRecord(nSkillId);
	if (!pTxt || nLevel <= 0)
		return 0;
	return D2Common_11033(nLevel, static_cast<int32_t>(pTxt->dwParam[4]), static_cast<int32_t>(pTxt->dwParam[5]));
}
