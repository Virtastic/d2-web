// 1.13c D2Common implementation batch (six UNMAPPED function), translated from the pinned
// decompiles. Retail pointers are four bytes wide; the guest-side layouts are modelled with raw
// 32-bit offsets where no vendored structure matches, and the vendored 1.10f twins are called
// where their body is line-for-line the same.
#include <cstdint>

#include "D2DataTbls.h"
#include "DataTbls/MonsterTbls.h"
#include "DataTbls/TokenTbls.h"
#include "Units/Units.h"

// Helper for 0x3C680: the outdoor-room tile-cell reader at 0x81950. Retail
// drops its first argument and reaches the reader with the room already in a register (ESI), which
// this translation spells out as the trailing argument. The reader takes subtile coordinates
// (world / 5), looks the row start up in the room's row-offset table, subtracts the room origin and
// returns the dword cell; a room flagged 0x01 has a single flat cell at +0x30 instead.
static uint32_t Batch18_TileCell_81950(const uint8_t* pRoom, int32_t nX, int32_t nY)
{
	const uint8_t* pGrid = *reinterpret_cast<const uint8_t* const*>(pRoom + 0x64);
	if (!pGrid)
	{
		return 0;                                   // retail fatals (Fog #10265, code 0x29C)
	}

	if (pGrid[0] & 1)
	{
		return *reinterpret_cast<const uint32_t*>(pGrid + 0x30);
	}

	const int32_t nSubX = nX / 5 - *reinterpret_cast<const int32_t*>(pRoom + 0x34);
	const int32_t nSubY = nY / 5 - *reinterpret_cast<const int32_t*>(pRoom + 0x38);
	const int32_t* pRowOffsets = *reinterpret_cast<const int32_t* const*>(pGrid + 0x20);
	const int32_t* pCells = *reinterpret_cast<const int32_t* const*>(pGrid + 0x1C);
	const int32_t nColumn = pRowOffsets[nSubY] - *reinterpret_cast<const int32_t*>(pRoom + 0x34) + nSubX;

	return *reinterpret_cast<const uint32_t*>(&pCells[nColumn]);
}

// 1.13c D2Common+0x3C680: forwards to the tile-cell reader above with the room passed through.
uint32_t d2_fn_d2common_3c680(uint32_t param_1, int32_t nX, int32_t nY, const uint8_t* pRoom)
{
	(void)param_1;                                  // retail's ECX argument is never read
	return Batch18_TileCell_81950(pRoom, nX, nY);
}

// 1.13c D2Common+0x18DB0: record check for the armtype table (record stride 0x34). The decompile's
// `nId * 0x34 + base == NULL` can only be true when the table is unloaded and nId is zero, so the
// only effect retail has is the D2_ASSERT fatal (Fog #10265, code 0x150); the 1.10f twin is
// DATATBLS_GetArmTypeTxtRecord, which performs the same assert.
void d2_fn_d2common_18db0(int32_t nId)
{
	(void)DATATBLS_GetArmTypeTxtRecord(nId);
}

// 1.13c D2Common+0x5A790: rolls the unit's unique-appellation string. A null unit, an empty table
// or a rolled record with a zero string id all yield the shared default unicode string, which is
// exactly the 1.10f body of DATATBLS_RollRandomUniqueAppellationString.
const Unicode* d2_fn_d2common_5a790(D2UnitStrc* pUnit)
{
	return DATATBLS_RollRandomUniqueAppellationString(pUnit);
}
