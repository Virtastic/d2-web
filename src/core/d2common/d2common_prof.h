// The ProfCore timing table (retail 23 records of 0x78 bytes): the accumulator (0x4A370) adds a clock
// delta and a call to one record, the reset (0x4A450) dumps and zeroes them. Both take the table as an
// argument so a pinned image can stand in for the module's own.
#pragma once
#include <cstddef>
#include <cstdint>

struct D2ProfSlot
{
	uint32_t nLow;      // 0x00 accumulated clock delta, low dword
	uint32_t nHigh;     // 0x04 high dword
	uint32_t nCount;    // 0x08 calls
	uint32_t nRest[(0x78 - 0x0C) / 4];   // 0x0C function name and the dump's fields
};
static_assert(sizeof(D2ProfSlot) == 0x78, "profiling record stride");
static_assert(offsetof(D2ProfSlot, nCount) == 0x08, "count at +8");
constexpr int32_t kD2ProfSlots = 23;

void D2Common_4A370_PerfAccumulate(D2ProfSlot* pSlots, int32_t nSlot, uint32_t nPreviousLow, uint32_t nPreviousHigh,
	void (*pfnCounter)(uint32_t* pLowHigh));
void D2Common_4A450_ProfileReset(D2ProfSlot* pSlots, void (*pfnDump)());
D2ProfSlot* D2Common_ProfSlots();
