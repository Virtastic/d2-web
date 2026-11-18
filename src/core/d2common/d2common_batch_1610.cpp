// 1.13c D2Common implementation batch: the ItemStatCost/Sets/SetItems row getters and the skill-desc
// validity test (0x17A0..0x1800, 0x18F0). The skills.txt dwParam getters, the unique-items row getter,
// the monster base id and the monstats flag bit (0x1550..0x18A0) now live in the game tree
// (SkillsTbls.cpp, ItemsTbls.cpp, MonsterTbls.cpp). Table strides are retail's 32-bit record sizes.
#include <cstddef>
#include <cstdint>

#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/SkillsTbls.h"

static_assert(sizeof(D2SkillsTxt) == 0x23C, "skills.txt record stride");
static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "itemstatcost.txt record stride");
static_assert(sizeof(D2SetsTxt) == 0x128, "sets.txt record stride");
static_assert(sizeof(D2SetItemsTxt) == 0x1B8, "setitems.txt record stride");
static_assert(sizeof(D2UniqueItemsTxt) == 0x14C, "uniqueitems.txt record stride");

// 1.13c D2Common+0x17A0
D2ItemStatCostTxt* D2Common_17A0_GetItemStatCostTxtRecord(int32_t nStatId)
{
	if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount) return nullptr;
	return (D2ItemStatCostTxt*)((uintptr_t)sgptDataTables->pItemStatCostTxt
		+ (uintptr_t)nStatId * sizeof(D2ItemStatCostTxt));
}

// 1.13c D2Common+0x17D0
D2SetsTxt* D2Common_17D0_GetSetsTxtRecord(int32_t nSetId)
{
	return DATATBLS_GetSetsTxtRecord(nSetId);
}

// 1.13c D2Common+0x1800
D2SetItemsTxt* D2Common_1800_GetSetItemsTxtRecord(int32_t nSetItemId)
{
	return DATATBLS_GetSetItemsTxtRecord(nSetItemId);
}

// 1.13c D2Common+0x18F0 (skill id arrives in EAX): 1 if the skill's wSkillDesc (+0x194) indexes a valid SkillDesc row
int32_t D2Common_18F0_SkillHasValidDesc(int32_t nSkillId)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) return 0;
	const uint8_t* pRec = (const uint8_t*)sgptDataTables->pSkillsTxt + (uintptr_t)nSkillId * 0x23C;
	if (!pRec) return 0;
	const int32_t nDesc = *(const int16_t*)(pRec + 0x194);
	return nDesc >= 0 && nDesc < sgptDataTables->nSkillDescTxtRecordCount;
}
