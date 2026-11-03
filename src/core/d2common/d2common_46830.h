// D2Common 0x46830: roll the socket count of an item (callee seam: the unit/stat/roll helpers run outside the port).
#pragma once
#include <cstdint>
struct D2Common46830Env {
	uint32_t (*itemsRow)(uint32_t nClassId);                    // 0x719A0 DATATBLS_GetItemsTxtRecord (host address of the row, 0 = none)
	uint32_t (*maxSockets)(uint32_t pItem);                     // 0x24610 (low byte used)
	int32_t (*rollCount)(uint32_t nMax, uint32_t nFirst, uint32_t pItem); // 0x465B0: EAX = rec+0xC, ECX = rec+8, EDX = pItem
	void (*setStat)(uint32_t pItem, uint32_t nStat, int32_t nValue);      // 0x3A740 (stdcall, 4th arg 0)
};
extern "C" int32_t D2Common_46830_RollSockets(uint32_t pItem, uint32_t pRec, int32_t nReq, const D2Common46830Env* env);
