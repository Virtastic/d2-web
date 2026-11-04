// D2Common 0x26930: AreStackablesEqual(pFirst, pSecond), stdcall, ret 8. Helpers arrive through the Env seams.
#pragma once
#include <cstdint>
struct D2Common26930Env {
	int32_t  (*fileIndex)(uint32_t unit);            // 0x22640 ITEMS_GetFileIndex
	uint32_t (*stackable)(uint32_t unit);            // 0x22E20 ITEMS_CheckIfStackable (byte result)
	uint32_t (*ethereal)(uint32_t unit);             // 0x23E00 masked item flag bit 0x400000
	int32_t  (*quality)(uint32_t unit);              // 0x23B40 ITEMS_GetItemQuality
	int32_t  (*statValue)(uint32_t unit, int32_t id);// 0x38B70 layer 0
	int32_t  (*opStatTotal)(uint32_t unit);          // 0x26080 socket total (byte test)
};
extern "C" uint32_t D2Common_26930_AreStackablesEqual(uint32_t pFirst, uint32_t pSecond, const D2Common26930Env* env);
