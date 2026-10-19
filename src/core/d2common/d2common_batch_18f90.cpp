// Teardown order and reset scope checked against cached retail 1.13c decompiles.
// Prior art used: D2MOO DataTbls, MonsterTbls, ItemsTbls and TokenTbls unload routines.
#include <emscripten.h>
#include "D2DataTbls.h"

// 1.13c D2Common+0x18F90: release the token/table blocks held by five slots, each guarded by its own
// non-null test and cleared afterwards.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_18f90()
{
	if (sgptDataTables->pPlrModeDataTables.pPlrModeTypeTxt != 0)
	{
		D2_FREE_POOL(nullptr, sgptDataTables->pPlrModeDataTables.pPlrModeTypeTxt);               // TokTbls.cpp:0xD6
	}
	sgptDataTables->pPlrModeDataTables.pPlrModeTypeTxt = 0;
	if (sgptDataTables->pMonModeDataTables.pMonModeTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonModeDataTables.pMonModeTxt);               // DataTbls.cpp
	}
	sgptDataTables->pMonModeDataTables.pMonModeTxt = 0;
	if (sgptDataTables->pObjModeDataTables.pObjModeTypeTxt != 0)
	{
		D2_FREE_POOL(nullptr, sgptDataTables->pObjModeDataTables.pObjModeTypeTxt);               // TokTbls.cpp:0xE2
	}
	sgptDataTables->pObjModeDataTables.pObjModeTypeTxt = 0;
	if (sgptDataTables->pCompositTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pCompositTxt);               // DataTbls.cpp
	}
	sgptDataTables->pCompositTxt = 0;
	if (sgptDataTables->pArmTypeTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pArmTypeTxt);               // DataTbls.cpp
	}
	sgptDataTables->pArmTypeTxt = 0;
}

// 1.13c D2Common+0x53BD0: release the monster-table allocation behind 0x6FDF0B10 and clear the pair.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_53bd0()
{
	if (sgptDataTables->pMonTypeTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonTypeTxt);               // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonTypeLinker);
		D2_FREE_POOL(nullptr, sgptDataTables->pMonTypeNest);
		sgptDataTables->pMonTypeTxt = 0;
		sgptDataTables->pMonTypeLinker = 0;
	}
}

// 1.13c D2Common+0x53EB0: clear the 512-dword retail table at 0x6FDF0080..0x6FDF087C to -1.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_53eb0()
{
	if (sgptDataTables->pHirelingTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pHirelingTxt);               // DataTbls.cpp
	}
	sgptDataTables->nHirelingTxtRecordCount = 0;
	for (int& id : sgptDataTables->nHirelingStartRecordIds) id = -1;
}

// 1.13c D2Common+0x53AD0: release one table allocation, then clear ten dword slots.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_53ad0()
{
	if (sgptDataTables->pMonPresetTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonPresetTxt);               // DataTbls.cpp
	}
	sgptDataTables->pMonPresetTxtActSections[0] = 0;
	sgptDataTables->nMonPresetTxtActRecordCounts[0] = 0;
	sgptDataTables->pMonPresetTxtActSections[1] = 0;
	sgptDataTables->nMonPresetTxtActRecordCounts[1] = 0;
	sgptDataTables->pMonPresetTxtActSections[2] = 0;
	sgptDataTables->nMonPresetTxtActRecordCounts[2] = 0;
	sgptDataTables->pMonPresetTxtActSections[3] = 0;
	sgptDataTables->nMonPresetTxtActRecordCounts[3] = 0;
	sgptDataTables->pMonPresetTxtActSections[4] = 0;
	sgptDataTables->nMonPresetTxtActRecordCounts[4] = 0;
}

// 1.13c D2Common+0x534B0: release the linker at 0x6FDF0890, report every one of the 0x2C-stride
// records whose +0x28 is still non-null, then clear the base/count pair. Native typed indexing
// preserves the widened pInfo pointer and record stride.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_534b0()
{
	FOG_FreeLinker(sgptDataTables->pTreasureClassExLinker);
	for (int i = 0; i < sgptDataTables->nTreasureClassEx; ++i)
	{
		if (sgptDataTables->pTreasureClassEx[i].pInfo != 0)
		{
			D2_FREE_POOL(nullptr, sgptDataTables->pTreasureClassEx[i].pInfo);
		}
	}
	D2_FREE_POOL(nullptr, sgptDataTables->pTreasureClassEx);
	sgptDataTables->pTreasureClassEx = 0;
	sgptDataTables->nTreasureClassEx = 0;
}

// 1.13c D2Common+0x539E0: clear two table pairs.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_539e0()
{
	if (sgptDataTables->pMonSeqTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonSeqTxt);               // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonSeqLinker);
		sgptDataTables->nMonSeqTxtRecordCount = 0;
	}
	if (sgptDataTables->pMonSeqTable != 0)
	{
		D2_FREE_POOL(nullptr, sgptDataTables->pMonSeqTable);
		sgptDataTables->nMonSeqTableRecordCount = 0;
	}
}

// 1.13c D2Common+0x53F10: release the linker at 0x6FDF09B0 and the guarded slot at 0x6FDF09AC.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_53f10()
{
	if (sgptDataTables->pSuperUniquesLinker != 0)
	{
		FOG_FreeLinker(sgptDataTables->pSuperUniquesLinker);
	}
	if (sgptDataTables->pSuperUniquesTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pSuperUniquesTxt);               // DataTbls.cpp
		sgptDataTables->pSuperUniquesTxt = 0;
	}
	sgptDataTables->nSuperUniquesTxtRecordCount = 0;
}

// 1.13c D2Common+0x53F70: release the linker at 0x6FDF0B24 and the guarded slot at 0x6FDF0B28.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_53f70()
{
	if (sgptDataTables->pMonUModLinker != 0)
	{
		FOG_FreeLinker(sgptDataTables->pMonUModLinker);
		sgptDataTables->pMonUModLinker = 0;
	}
	if (sgptDataTables->pMonUModTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonUModTxt);               // DataTbls.cpp
		sgptDataTables->pMonUModTxt = 0;
	}
	sgptDataTables->nMonUModTxtRecordCount = 0;
}

// 1.13c D2Common+0x54200: teardown of the level/monster table group. It runs six of the routines
// above in retail's order, releases three linker/pointer pairs and walks four slots (plus their +0x10
// companions) that still hold an allocation.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_54200()
{
	if (sgptDataTables->pMonEquipTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonEquipTxt);               // DataTbls.cpp
		sgptDataTables->nMonEquipTxtRecordCount = 0;
	}
	if (sgptDataTables->pNpcTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pNpcTxt);               // DataTbls.cpp
	}
	d2_fn_d2common_53eb0();
	d2_fn_d2common_53ad0();
	d2_fn_d2common_53f10();
	d2_fn_d2common_53f70();
	if (sgptDataTables->pMonStatsTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonStatsTxt);               // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonStatsLinker);
	}
	sgptDataTables->pMonStatsTxt = 0;
	if (sgptDataTables->pMonStats2Txt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonStats2Txt);               // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonStats2Linker);
	}
	if (sgptDataTables->pMonPropTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonPropTxt);               // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonPropLinker);
	}
	sgptDataTables->nMonPropTxtRecordCount = 0;
	if (sgptDataTables->pMonSoundsTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonSoundsTxt);               // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonSoundsLinker);
		sgptDataTables->nMonSoundsTxtRecordCount = 0;
	}
	d2_fn_d2common_539e0();
	d2_fn_d2common_534b0();
	if (sgptDataTables->pMonLvlTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pMonLvlTxt);               // DataTbls.cpp
		sgptDataTables->nMonLvlTxtRecordCount = 0;
	}
	if (sgptDataTables->pUniqueTitleTxt) { DATATBLS_UnloadBin(sgptDataTables->pUniqueTitleTxt); sgptDataTables->pUniqueTitleTxt = nullptr; sgptDataTables->nUniqueTitleTxtRecordCount = 0; }
	if (sgptDataTables->pUniquePrefixTxt) { DATATBLS_UnloadBin(sgptDataTables->pUniquePrefixTxt); sgptDataTables->pUniquePrefixTxt = nullptr; sgptDataTables->nUniquePrefixTxtRecordCount = 0; }
	if (sgptDataTables->pUniqueSuffixTxt) { DATATBLS_UnloadBin(sgptDataTables->pUniqueSuffixTxt); sgptDataTables->pUniqueSuffixTxt = nullptr; sgptDataTables->nUniqueSuffixTxtRecordCount = 0; }
	if (sgptDataTables->pUniqueAppellationTxt) { DATATBLS_UnloadBin(sgptDataTables->pUniqueAppellationTxt); sgptDataTables->pUniqueAppellationTxt = nullptr; sgptDataTables->nUniqueAppellationTxtRecordCount = 0; }
}

// 1.13c D2Common+0x5DE00: teardown of the 0x6FDEFE.. / 0x6FDEFF.. block. One guarded slot, then,
// when bCompileTxt is set, fifteen guarded bins each followed by its linker.
// All linkers are released whether or not their corresponding bin is non-null.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5de00()
{
	if (sgptDataTables->pCompCodeTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pCompCodeTxt);               // DataTbls.cpp
	}
	FOG_FreeLinker(sgptDataTables->pCompCodeLinker);
	if (sgptDataTables->bCompileTxt != 0)
	{
		if (sgptDataTables->pPlayerClassTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pPlayerClassTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pPlayerClassLinker);
		if (sgptDataTables->pBodyLocsTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pBodyLocsTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pBodyLocsLinker);
		if (sgptDataTables->pStorePageTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pStorePageTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pStorePageLinker);
		if (sgptDataTables->pElemTypesTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pElemTypesTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pElemTypesLinker);
		if (sgptDataTables->pHitClassTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pHitClassTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pHitClassLinker);
		if (sgptDataTables->pColorsTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pColorsTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pColorsLinker);
		if (sgptDataTables->pHireDescTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pHireDescTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pHireDescLinker);
		if (sgptDataTables->pMonModeTxtStub != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pMonModeTxtStub); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonModeLinker);
		if (sgptDataTables->pPlrModeTxtStub != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pPlrModeTxtStub); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pPlrModeLinker);
		if (sgptDataTables->pMonAiTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pMonAiTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonAiLinker);
		if (sgptDataTables->pMonPlaceTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pMonPlaceTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMonPlaceLinker);
		if (sgptDataTables->pSkillCalcTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pSkillCalcTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pSkillCalcLinker);
		if (sgptDataTables->pMissileCalcTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pMissileCalcTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pMissileCalcLinker);
		if (sgptDataTables->pSkillCode != 0) {
		DATATBLS_UnloadBin(const_cast<char*>(sgptDataTables->pSkillCode)); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->iSkillCode);
		if (sgptDataTables->pEventsTxt != 0) {
		DATATBLS_UnloadBin(sgptDataTables->pEventsTxt); }  // DataTbls.cpp
		FOG_FreeLinker(sgptDataTables->pEventsLinker);
	}
}

// 1.13c D2Common+0x6C7E0: clear the two words at sgptDataTables+0xC58/+0xC5C; the first is a guard
// naming the levels bin and its record count in the retail layout.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6c7e0()
{
	DATATBLS_UnloadBin(sgptDataTables->pLevelsTxt);
	sgptDataTables->nLevelsTxtRecordCount = 0;
}

// 1.13c D2Common+0x71180: one unconditional pool release, the guarded slot at 0x6FDF0AD0, then the
// linker at 0x6FDF0ACC.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_71180()
{
	D2_FREE_POOL(nullptr, sgptDataTables->pItemTypesEquivalenceLUTs);
	if (sgptDataTables->pItemTypesTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pItemTypesTxt);               // DataTbls.cpp
	}
	FOG_FreeLinker(sgptDataTables->pItemTypesLinker);
}

// 1.13c D2Common+0x716B0: release two linkers (0x6FDF0AE0, 0x6FDF0AEC) unconditionally, two guarded
// slots, then clear the two companion words.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_716b0()
{
	FOG_FreeLinker(sgptDataTables->pSetsLinker);
	FOG_FreeLinker(sgptDataTables->pSetItemsLinker);
	if (sgptDataTables->pSetItemsTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pSetItemsTxt);               // DataTbls.cpp
	}
	if (sgptDataTables->pSetsTxt != 0)
	{
		DATATBLS_UnloadBin(sgptDataTables->pSetsTxt);               // DataTbls.cpp
	}
	sgptDataTables->nSetsTxtRecordCount = 0;
	sgptDataTables->nSetItemsTxtRecordCount = 0;
}
