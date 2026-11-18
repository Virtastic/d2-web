// Moved from the stand-alone d2common batch into the game tree (treasure-class loader leaf of DATATBLS_LoadTreasureClassExTxt).
// Prior art: 1.10f inlined body, vendored as ParseTreasureClassItem.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "D2DataTbls.h"
#include "DataTbls/MonsterTbls.h"

// Append one info slot (1.13c D2Common+0x54020) seeded with the record's running totals.
static D2TCExInfoStrc* AppendInfo(D2TCExShortStrc* pTC)
{
	return DATATBLS_ReallocTCExInfo(pTC);
}

// Consume the next ',' separated numeric field through the game-tree field splitter (retail 0x535D0); *ppNext is the rest.
extern "C" int32_t D2Common_535D0_SplitAndAtoi(char** ppCursor, char cSeparator, int32_t (*retailAtol)(const char*));
static int32_t HostAtol(const char* sz) { return (int32_t)atol(sz); }
static int16_t TakeField(char* szValue, char** ppNext)
{
	char* p = szValue;
	const int32_t n = D2Common_535D0_SplitAndAtoi(&p, ',', HostAtol);
	*ppNext = p;
	return (int16_t)n;
}

// Called by DATATBLS_LoadTreasureClassExTxt (MonsterTbls.cpp) in place of the 1.10f inline parser, which matched key names
// by prefix and stopped on an empty value. The item id is the out-parameter of 0x71960 (GetItemRecordFromItemCode, always written).
// 1.13c D2Common+0x54430: parse one "name[,key=value...]" treasure item string with pick probability
// nProb into a new info slot of pTC. Name lookup order: item code (<=4 chars), treasure class, uniques,
// sets. Keys: mul/ma/mg -> nTxtRow, cu nUnique, cs nSet, cr nRare, cm nMagic, ce nSuperior, cg nNormal;
// an unknown key ends parsing with success. Returns 0 when the name resolves nowhere or nProb < 1.
int D2Common_54430_ParseTreasureClassItem(D2TCExShortStrc* pTC, char* szText, int nProb)
{
	if (nProb < 1) return 0;
	if (*szText == '"') ++szText;
	for (char* p = szText; *p; ++p) if (*p == '"') *p = '\0';

	char* szNext = strchr(szText, ',');
	if (szNext) { *szNext = '\0'; ++szNext; }

	D2TCExInfoStrc* pInfo = nullptr;
	int nItemId = 0;
	if (strlen(szText) < 5)
	{
		D2ItemsTxt* pItem = DATATBLS_GetItemRecordFromItemCode(DATATBLS_StringToCode(szText), &nItemId);
		if (pItem)
		{
			pInfo = AppendInfo(pTC);
			pInfo->nItemId = (int16_t)nItemId;
			const bool bExp = pItem->wVersion >= 100;
			if (bExp) pInfo->nFlags |= 0x10;
			DATATBLS_UpdateTreasureClassProbabilities(pTC, pInfo, nProb, bExp);
		}
	}
	if (!pInfo)
	{
		int nRow = FOG_GetRowFromTxt(sgptDataTables->pTreasureClassExLinker, szText, 0);
		if (nRow > 0)
		{
			pInfo = AppendInfo(pTC);
			pInfo->nItemId = (int16_t)nRow;
			pInfo->nFlags |= 4;
			const bool bNoClassic = sgptDataTables->pTreasureClassEx[nRow].nClassic == 0;
			if (bNoClassic) pInfo->nFlags |= 0x10;
			DATATBLS_UpdateTreasureClassProbabilities(pTC, pInfo, nProb, bNoClassic);
		}
		else if (sgptDataTables->pUniqueItemsLinker && (nRow = FOG_GetRowFromTxt(sgptDataTables->pUniqueItemsLinker, szText, 0)) > 0)
		{
			pInfo = AppendInfo(pTC);
			DATATBLS_GetItemRecordFromItemCode(sgptDataTables->pUniqueItemsTxt[nRow].dwBaseItemCode, &nItemId);
			pInfo->nFlags |= 0x11;
			pInfo->nItemId = (int16_t)nItemId;
			pInfo->nTxtRow = (int16_t)nRow;
			DATATBLS_UpdateTreasureClassProbabilities(pTC, pInfo, nProb, TRUE);
		}
		else if (sgptDataTables->pSetItemsLinker && (nRow = FOG_GetRowFromTxt(sgptDataTables->pSetItemsLinker, szText, 0)) >= 0)
		{
			pInfo = AppendInfo(pTC);
			DATATBLS_GetItemRecordFromItemCode(sgptDataTables->pSetItemsTxt[nRow].szItemCode, &nItemId);
			pInfo->nFlags |= 0x12;
			pInfo->nItemId = (int16_t)nItemId;
			pInfo->nTxtRow = (int16_t)nRow;
			DATATBLS_UpdateTreasureClassProbabilities(pTC, pInfo, nProb, TRUE);
		}
		else return 0;
	}

	while (szNext)
	{
		char* szValue = strchr(szNext, '=');
		if (!szValue) break;
		*szValue++ = '\0';
		// The key is matched before the value is consumed: an unknown key leaves the rest of the string untouched.
		int16_t* pField = nullptr;
		if (!strcmp(szNext, "mul") || !strcmp(szNext, "ma") || !strcmp(szNext, "mg")) pField = &pInfo->nTxtRow;
		else if (!strcmp(szNext, "cu")) pField = &pInfo->nUnique;
		else if (!strcmp(szNext, "cs")) pField = &pInfo->nSet;
		else if (!strcmp(szNext, "cr")) pField = &pInfo->nRare;
		else if (!strcmp(szNext, "cm")) pField = &pInfo->nMagic;
		else if (!strcmp(szNext, "ce")) pField = &pInfo->nSuperior;
		else if (!strcmp(szNext, "cg")) pField = &pInfo->nNormal;
		else break;
		*pField = TakeField(szValue, &szValue);
		szNext = szValue;
	}
	return 1;
}
