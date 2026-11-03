// 1.13c D2Common+0x2D020 (sub_2D020, ordinal 10495): stdcall(pGrid, nRow). Clears the bits
// nRow*0x10 + 2 through nRow*0x10 + 11 of the bit array whose address is the first dword of pGrid.
// Retail calls Fog #10120 (BITMANIP_MaskBitstate) once per column. A null pGrid is fatal in retail
// (Fog #10265 log, then _exit(-1)); the port returns instead. This is the grid sibling of 0x2D070, not
// the 1.10f quest-record reset in src/core/d2moo/src/D2QuestRecord.cpp.
#include <cstdint>
#include <emscripten.h>
#include "D2BitManip.h"

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_2D020_ClearRowColumns2To11(uint32_t pGrid, int32_t nRow)
{
	if (!pGrid)
		return;
	const uint32_t dwBits = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pGrid));
	uint8_t* pBits = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(dwBits));
	for (int32_t nColumn = 2; nColumn < 12; ++nColumn)
		BITMANIP_MaskBitstate(pBits, nRow * 0x10 + nColumn);
}
