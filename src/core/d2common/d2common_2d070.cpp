// 1.13c D2Common+0x2D070 (sub_2D070, ordinal 10593): stdcall(pGrid, nRow, nColumn). Clears bit
// (nRow * 0x10 + nColumn) of the bit array whose address is the first dword of pGrid. Retail's
// call target is Fog #10120 (BITMANIP_MaskBitstate). A null pGrid is fatal in retail (Fog #10265
// log, then _exit(-1)); the port returns instead.
#include <cstdint>
#include <emscripten.h>
#include "D2BitManip.h"

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_2D070_ClearGridBit(uint32_t pGrid, int32_t nRow, int32_t nColumn)
{
	if (!pGrid)
		return;
	const uint32_t dwBits = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pGrid));
	uint8_t* pBits = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(dwBits));
	BITMANIP_MaskBitstate(pBits, nRow * 0x10 + nColumn);
}
