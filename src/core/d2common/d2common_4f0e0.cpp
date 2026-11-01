// 1.13c D2Common+0x4F0E0 (#10313): skills.txt nPassiveState (+0x94) when positive; a bad id or a
// non-positive value answers 0.
#include <cstdint>
#include "D2DataTbls.h"

extern "C" int32_t D2Common_4F0E0_PassiveState(int32_t nSkillId)
{
	if (!sgptDataTables || !sgptDataTables->pSkillsTxt) return 0;
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) return 0;

	const int16_t nPassiveState = sgptDataTables->pSkillsTxt[nSkillId].nPassiveState;
	return nPassiveState > 0 ? nPassiveState : 0;
}
