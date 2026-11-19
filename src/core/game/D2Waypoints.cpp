#include "D2Waypoints.h"
#include <test_seam.h>

#include "D2DataTbls.h"

static const int gnNumberOfWaypoints = 7 * 8 * sizeof(short);

// 1.13c D2Common+0x99C58: the module's waypoint bit table, one {word index, mask} pair per waypoint number (word 1 + n/16,
// mask 1 << n%16), with its entry count at +0x99E18 (0x70). Every set/test/alloc/copy below indexes it, as retail does;
// the FN probes rewrite an entry to replay a capture's `gvar`.
static constexpr D2WaypointFlagTables WAYPOINTS_MakeFlagTable()
{
	D2WaypointFlagTables tTable = {};
	for (int i = 0; i < gnNumberOfWaypoints; ++i)
	{
		tTable.pEntries[i].nArrayId = (uint16_t)(1 + i / 16);
		tTable.pEntries[i].nFlag = (uint16_t)(1u << (i % 16));
	}
	tTable.nCount = gnNumberOfWaypoints;
	return tTable;
}

static_assert(sizeof(D2WaypointFlagTableStrc) == 4 && offsetof(D2WaypointFlagTables, nCount) == 0x1C0, "waypoint bit table layout");
D2WaypointFlagTables gWaypointFlagTable = WAYPOINTS_MakeFlagTable();

//D2Common.0x6FDC3D20 (#11153)
// 1.13c D2Common+0x67EB0
BOOL __stdcall WAYPOINTS_GetLevelIdFromWaypointNo(short nWaypointNo, int* pLevelId)
{
	*pLevelId = 0;

	// 1.13c: the number is an unsigned 16-bit word (a negative short is >= 255 and answers 0 without a scan);
	// the levels row is base +0xC58 + i * 0x220, read directly.
	const uint16_t nWp = (uint16_t)nWaypointNo;
	if (nWp < 255)
	{
		for (int i = 1; i < sgptDataTables->nLevelsTxtRecordCount; ++i)
		{
			if (sgptDataTables->pLevelsTxt[i].nWaypoint == nWp)
			{
				*pLevelId = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FDC3D90 (#11152)
// 1.13c D2Common+0x67E50 (#10436): *pWaypointNo := 0xFF, then for 0 < nLevelId < count the levels row's +0xE4 byte;
// TRUE when that byte is not 0xFF.
BOOL __stdcall WAYPOINTS_GetWaypointNoFromLevelId(int nLevelId, short* pWaypointNo)
{
	if (D2Seam<&WAYPOINTS_GetWaypointNoFromLevelId>::hook) return D2Seam<&WAYPOINTS_GetWaypointNoFromLevelId>::hook(nLevelId, pWaypointNo);
	*pWaypointNo = 255;

	// Retail reads the levels row directly (id inside 1..count-1, row base nonzero), no table accessor.
	if (nLevelId < sgptDataTables->nLevelsTxtRecordCount && nLevelId > 0)
	{
		const D2LevelsTxt* pLevelsTxtRecord = &sgptDataTables->pLevelsTxt[nLevelId];
		if (pLevelsTxtRecord)
		{
			*pWaypointNo = (uint16_t)pLevelsTxtRecord->nWaypoint;
			if ((uint16_t)*pWaypointNo != 0xFF)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FDC3DE0 (#11146)
// 1.13c D2Common+0x67C90 (#10373): null data, a table count other than 0x70 and a number >= 0x70 are fatal; otherwise the
// table entry's word ANDed with its mask (the masked word itself, not 0/1).
int __stdcall WAYPOINTS_IsActivated(D2WaypointDataStrc* pData, uint16_t wField)
{
	if (D2Seam<&WAYPOINTS_IsActivated>::hook) return D2Seam<&WAYPOINTS_IsActivated>::hook(pData, wField);
	// Retail asserts the pointer, then the waypoint count global (0x70), then the index; the bit is the
	// 16-bit word at index wField / 16 + 1 (the word before it is the header) masked by 1 << (wField & 15).
	D2_ASSERT(pData);
	D2_ASSERT(gWaypointFlagTable.nCount == gnNumberOfWaypoints);
	D2_ASSERT(wField < gnNumberOfWaypoints);

	const D2WaypointFlagTableStrc& tEntry = gWaypointFlagTable.pEntries[wField];
	return (uint16_t)(pData->nFlags[tEntry.nArrayId] & tEntry.nFlag);
}

//D2Common.0x6FDC3E80 (#11147)
// 1.13c D2Common+0x67C20 (#11154): as 0x67C90, ORs the table entry's mask into its word.
void __stdcall WAYPOINTS_ActivateWaypoint(D2WaypointDataStrc* pData, uint16_t wField)
{
	D2_ASSERT(pData);
	D2_ASSERT(gWaypointFlagTable.nCount == gnNumberOfWaypoints);
	D2_ASSERT(wField < gnNumberOfWaypoints);

	const D2WaypointFlagTableStrc& tEntry = gWaypointFlagTable.pEntries[wField];
	pData->nFlags[tEntry.nArrayId] |= tEntry.nFlag;
}

//D2Common.0x6FDC3F20 (#11148)
// 1.13c D2Common+0x67BD0 (#10309): 0x10 bytes from the pool (Waypoint.cpp line 0xFC), zeroed, word 0 := 0x102, then
// table entry 0's mask ORed into its word.
D2WaypointDataStrc* __stdcall WAYPOINTS_AllocWaypointData(void* pMemPool)
{
	D2WaypointDataStrc* pData = (D2WaypointDataStrc*)FOG_AllocPool(pMemPool, sizeof(D2WaypointDataStrc), __FILE__, 0xFC, 0);
	memset(pData, 0x00, sizeof(D2WaypointDataStrc));

	pData->nFlags[0] = 0x102;
	pData->nFlags[gWaypointFlagTable.pEntries[0].nArrayId] |= gWaypointFlagTable.pEntries[0].nFlag;

	return pData;
}

//D2Common.0x6FDC3F70 (#11149)
// 1.13c D2Common+0x67DF0 (#10897): a version word other than 0x102 must be 0 or 0x101 (fatal otherwise) and becomes 0x102;
// then the pool free (Waypoint.cpp line 0x121).
void __stdcall WAYPOINTS_FreeWaypointData(void* pMemPool, D2WaypointDataStrc* pData)
{
	if (pData->nFlags[0] != 0x102)
	{
		D2_ASSERT(pData->nFlags[0] == 0x00 || pData->nFlags[0] == 0x101);
		pData->nFlags[0] = 0x102;
	}

	FOG_FreePool(pMemPool, pData, __FILE__, 0x121, 0);
}

//D2Common.0x6FDC3FD0 (#11150)
// 1.13c D2Common+0x67D60 (#10979): copy the 16 bytes; a version word other than 0x102 (0 or 0x101, fatal otherwise) clears
// the whole record, word 0 included; then table entry 0's mask is ORed into its word. Retail returns the address of that
// word; no caller reads it.
void __stdcall WAYPOINTS_CopyAndValidateWaypointData(D2WaypointDataStrc* pDestination, D2WaypointDataStrc* pSource)
{
	// Retail copies four dwords, then if the header is not 0x102 asserts it is 0 or 0x101 and clears all
	// eight words (the header too), and finally sets word 1 bit 0.
	memmove(pDestination, pSource, 4 * sizeof(uint32_t));

	if (pDestination->nFlags[0] != 0x102)
	{
		D2_ASSERT(pDestination->nFlags[0] == 0x00 || pDestination->nFlags[0] == 0x101);
		memset(pDestination, 0x00, sizeof(D2WaypointDataStrc));
	}

	pDestination->nFlags[gWaypointFlagTable.pEntries[0].nArrayId] |= gWaypointFlagTable.pEntries[0].nFlag;
}

//D2Common.0x6FDC4060 (#11151)
// 1.13c D2Common+0x67D00
void __stdcall WAYPOINTS_ValidateAndCopyWaypointData(D2WaypointDataStrc* pSource, D2WaypointDataStrc* pDestination)
{
	if (pSource->nFlags[0] != 0x102)
	{
		D2_ASSERT(pSource->nFlags[0] == 0x00 || pSource->nFlags[0] == 0x101);
		pSource->nFlags[0] = 0x102;
	}

	pDestination->nFlags[0] = pSource->nFlags[0];
	pDestination->nFlags[1] = pSource->nFlags[1];
	pDestination->nFlags[2] = pSource->nFlags[2];
	pDestination->nFlags[3] = pSource->nFlags[3];
	pDestination->nFlags[4] = pSource->nFlags[4];
	pDestination->nFlags[5] = pSource->nFlags[5];
	pDestination->nFlags[6] = pSource->nFlags[6];
	pDestination->nFlags[7] = pSource->nFlags[7];
}
