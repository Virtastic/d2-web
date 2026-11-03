// 1.13c D2Common+0x46770: flag a runeword item and assign its runes.txt properties.
#include <cstdint>
#include "d2common_46770.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

extern "C" uint32_t D2Common_46770_AssignRuneProps(uint32_t pItem, uint32_t nArg, const D2Common46770Env* env)
{
	const uint32_t pRow = env->runesRow(pItem);
	if (pRow == 0) return 0;
	const uint32_t pSl = rd32(pItem + 0x5C);
	if (pSl != 0 && static_cast<int32_t>(rd32(pSl + 0x10)) < 0) {
		for (uint32_t n = rd32(pSl + 0x3C); n != 0; n = rd32(n + 0x2C))
			if (rd32(n + 0x14) == 0xAB) return 0;
		for (uint32_t n = rd32(pSl + 0x40); n != 0; n = rd32(n + 0x2C))
			if (rd32(n + 0x14) == 0xAB) return 0;
	}
	if (rd32(pItem) == 4 && rd32(pItem + 0x14) != 0)
		*reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(rd32(pItem + 0x14)) + 0x18) |= 0x4000000;
	uint32_t pProp = pRow + 0xB0;
	for (uint32_t i = 0; i < 7; i++, pProp += 0x10) {
		if (static_cast<int32_t>(rd32(pProp)) < 0) return 1;
		env->assign(6, pItem, nArg, i, pProp);
	}
	return 1;
}
