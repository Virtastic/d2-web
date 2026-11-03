// D2Common 0x46770 (#10260): assign the rune properties of an item (callee seam: the runes row lookup and the property assigner).
#pragma once
#include <cstdint>
struct D2Common46770Env {
	uint32_t (*runesRow)(uint32_t pItem);                                                    // 0x26BA0 (stdcall, ret 4): runes.txt row of the item, 0 = none
	void (*assign)(uint32_t nMode, uint32_t pItem, uint32_t nArg, uint32_t nIndex, uint32_t pProp); // 0x45990 (ret 0x28; 10 words: 6, item, arg, 0, index, 0, prop, 0xAB, 0x40, 0)
};
extern "C" uint32_t D2Common_46770_AssignRuneProps(uint32_t pItem, uint32_t nArg, const D2Common46770Env* env);
