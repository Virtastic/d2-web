// 1.13c D2Common batch: +0x53780, +0x54430 (treasure-class loader leaves of DATATBLS_LoadTreasureClassExTxt).
// Prior art: D2MOO 1.10f inlined bodies, vendored as DATATBLS_NewTreasureClassEx / ParseTreasureClassItem.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "D2DataTbls.h"
#include "DataTbls/MonsterTbls.h"

// 1.13c D2Common+0x53780: register a treasure-class name in the linker, grow the record array in steps of
// 16, zero the new slot, bump the count, return the slot. (The vendored D2MOO twin leaves the bump to the caller.)
D2TCExShortStrc* D2Common_53780_NewTreasureClassEx(const char* szName)
{
	FOG_10216_AddRecordToLinkingTable(sgptDataTables->pTreasureClassExLinker, szName);
	if (!(sgptDataTables->nTreasureClassEx % 16))
	{
		sgptDataTables->pTreasureClassEx = (D2TCExShortStrc*)D2_REALLOC_POOL(nullptr, sgptDataTables->pTreasureClassEx,
			sizeof(D2TCExShortStrc) * (sgptDataTables->nTreasureClassEx + 16));
	}
	D2TCExShortStrc* pRecord = &sgptDataTables->pTreasureClassEx[sgptDataTables->nTreasureClassEx];
	memset(pRecord, 0x00, sizeof(*pRecord));
	++sgptDataTables->nTreasureClassEx;
	return pRecord;
}

// Append one info slot (1.13c D2Common+0x54020) seeded with the record's running totals.
static D2TCExInfoStrc* AppendInfo(D2TCExShortStrc* pTC)
{
	const int n = pTC->nTypes;
	DATATBLS_ReallocTCExInfo(pTC, n + 1);
	D2TCExInfoStrc* pInfo = &pTC->pInfo[n];
	pInfo->nClassic = pTC->nClassic;
	pInfo->nProb = pTC->nProb;
	return pInfo;
}

// Consume the next ',' separated numeric field (1.13c D2Common+0x535D0); *ppNext is the rest.
static int16_t TakeField(char* szValue, char** ppNext)
{
	char* p = strchr(szValue, ',');
	if (p) { *p = '\0'; ++p; }
	*ppNext = p;
	return (int16_t)atol(szValue);
}

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
		const int16_t v = TakeField(szValue, &szValue);
		if (!strcmp(szNext, "mul") || !strcmp(szNext, "ma") || !strcmp(szNext, "mg")) pInfo->nTxtRow = v;
		else if (!strcmp(szNext, "cu")) pInfo->nUnique = v;
		else if (!strcmp(szNext, "cs")) pInfo->nSet = v;
		else if (!strcmp(szNext, "cr")) pInfo->nRare = v;
		else if (!strcmp(szNext, "cm")) pInfo->nMagic = v;
		else if (!strcmp(szNext, "ce")) pInfo->nSuperior = v;
		else if (!strcmp(szNext, "cg")) pInfo->nNormal = v;
		else break;
		szNext = szValue;
	}
	return 1;
}
