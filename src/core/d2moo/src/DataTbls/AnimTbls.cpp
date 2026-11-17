#include "D2DataTbls.h"

#include "D2Composit.h"
#include "Units/Units.h"

#include <Archive.h>

//D2Common.0x6FD473C0
// 1.13c D2Common+0x41E50
D2AnimDataTableStrc* __fastcall DATATBLS_LoadAnimDataD2(HD2ARCHIVE hArchive)
{
	D2AnimDataTableStrc* pAnimDataTable = D2_CALLOC_STRC_POOL(nullptr, D2AnimDataTableStrc);

	char szPath[MAX_PATH] = {};
	wsprintfA(szPath, "%s\\AnimData.d2", "DATA\\GLOBAL");
	pAnimDataTable->pBinaryData = ARCHIVE_ALLOC_BUFFER_AND_READ_FILE_TO_IT(hArchive, szPath, nullptr);
;
	D2AnimDataBucketStrc* pBucketBinaryData = (D2AnimDataBucketStrc*)pAnimDataTable->pBinaryData;
	for (int i = 0; i < 256; ++i)
	{
		pAnimDataTable->pHashTableBucket[i] = pBucketBinaryData;
		const size_t nBucketSize = sizeof(pBucketBinaryData->nbEntries) + sizeof(D2AnimDataRecordStrc) * pBucketBinaryData->nbEntries;
		pBucketBinaryData = (D2AnimDataBucketStrc*)(((char*)pBucketBinaryData) + nBucketSize);
	}

	pAnimDataTable->tDefaultRecord.dwFrames = 2048;
	pAnimDataTable->tDefaultRecord.dwAnimSpeed = 256;

	return pAnimDataTable;
}

//D2Common.0x6FD47460
// not-1.13c: retail inlines this unload in DATATBLS_UnloadAllBins (0x5E870); fold target DataTbls.cpp is outside this batch
void __fastcall DATATBLS_UnloadAnimDataD2(D2AnimDataTableStrc* pAnimData)
{
	if (pAnimData)
	{
		D2_FREE(pAnimData->pBinaryData);
		D2_FREE_POOL(nullptr, pAnimData);
	}
}

// 1.13c D2Common+0x41DD0 (upper-case in place, byte-sum bucket, 8-byte name compare); the upper-casing and the byte-sum hash are inlined here as in retail
static D2AnimDataRecordStrc* DATATBLS_GetAnimDataRecordFromPath(char* szPath)
{
	for (char* pChar = szPath; *pChar != '\0'; ++pChar)
	{
		if (*pChar >= 'a' && *pChar <= 'z')
		{
			*pChar -= ' ';
		}
	}
	uint8_t nHash = 0;
	for (const char* pChar = szPath; *pChar != '\0'; ++pChar)
	{
		nHash += *pChar;
	}

	D2AnimDataTableStrc* pAnimData = DATATBLS_GetAnimData();

	D2AnimDataBucketStrc* pAnimDataBucket = pAnimData->pHashTableBucket[nHash];
	for (int i = 0; i < pAnimDataBucket->nbEntries; i++)
	{
		D2AnimDataRecordStrc& rBucketEntry = pAnimDataBucket->aEntries[i];
		D2_ASSERT(strlen(szPath) <= 8); // This one is useless. Remains of a more generic function?
		D2_ASSERT(strlen(rBucketEntry.szAnimDataName) <= 8);
		// Note: Only works if strings are completly padded after null terminator.
		if (0 == memcmp(szPath, rBucketEntry.szAnimDataName, 8))
		{
			return &rBucketEntry;
		}
	}
	return nullptr;
}

//D2Common.0x6FD474A0
// 1.13c D2Common+0x41F70
const D2AnimDataRecordStrc* __fastcall DATATBLS_GetAnimDataRecord(D2UnitStrc* pUnit, int nClassId, int nMode, int nUnitType, D2InventoryStrc* pInventory)
{
	int nWeaponClassCode = 0;
	char szPath[8] = {};
	D2Common_10884_COMPOSIT_unk(pUnit, nClassId, nMode, nUnitType, pInventory, szPath, &nWeaponClassCode, FALSE, 1);
	if (D2AnimDataRecordStrc* pFound = DATATBLS_GetAnimDataRecordFromPath(szPath))
	{
		return pFound;
	}
	else
	{
		return &DATATBLS_GetAnimData()->tDefaultRecord;
	}
}

//D2Common.0x6FD475D0 (#10640)
// 1.13c D2Common+0x41FD0: sets the anim speed on the unit's record and returns; no halt in retail
void __stdcall DATATBLS_UnitAnimInfoDebugSet(D2UnitStrc* pUnit, int nAnimSpeed)
{
	int nWeaponClassCode = 0;
	char szPath[8] = {};
	D2Common_10885_COMPOSIT_unk(pUnit, szPath, &nWeaponClassCode, 0, 1, pUnit->pInventory, -1);
	if (D2AnimDataRecordStrc* pRecord = DATATBLS_GetAnimDataRecordFromPath(szPath))
	{
		pRecord->dwAnimSpeed = nAnimSpeed;
	}
}

//D2Common.0x6FD47700 (#10641)
// 1.13c D2Common+0x41EF0
BOOL __stdcall DATATBLS_GetAnimDataInfo(char* szPath, int* pOutLength, int* pOutAnimSpeed, int* pOutFirstFrameTagged)
{
	if (D2AnimDataRecordStrc* pRecord = DATATBLS_GetAnimDataRecordFromPath(szPath))
	{
		*pOutLength = pRecord->dwFrames;
		*pOutAnimSpeed = pRecord->dwAnimSpeed;

		for (uint32_t i = 0; i < pRecord->dwFrames; ++i)
		{
			if (pRecord->pFrameFlags[i] || i >= D2AnimDataRecordStrc::MAX_FRAME_FLAGS)
			{
				*pOutFirstFrameTagged = i;
				return TRUE;
			}
		}

		*pOutFirstFrameTagged = 0;
		return TRUE;
	}

	const D2AnimDataRecordStrc& pDefaultRecord = DATATBLS_GetAnimData()->tDefaultRecord;
	*pOutLength = pDefaultRecord.dwFrames;
	*pOutAnimSpeed = pDefaultRecord.dwAnimSpeed;
	*pOutFirstFrameTagged = *pOutLength;
	return FALSE;
}
