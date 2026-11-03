// 1.13c D2Common+0x46830: roll and apply the socket count of an item unit.
#include <cstdint>
#include "d2common_46830.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

// pItem = retail unit (+0 type, +4 class id, +0x14 item data whose +0x18 flags take 0x800), pRec = the 4-word-plus record
// (+4 default count, +8 and +0xC the roll operands), nReq = stack argument 6 (a requested count, <1 = roll it).
// Retail's `ret 0x24` pops nine words; only these three are read.
extern "C" int32_t D2Common_46830_RollSockets(uint32_t pItem, uint32_t pRec, int32_t nReq, const D2Common46830Env* env)
{
	if (pRec == 0 || pItem == 0 || rd32(pItem) != 4) return 0;
	uint32_t cols = 0, rows = 0;
	if (const uint32_t row = env->itemsRow(rd32(pItem + 4))) {
		cols = *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(row) + 0x10F);
		rows = *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(row) + 0x110);
	}
	int32_t cap = static_cast<int32_t>(rows * cols);
	if (cap <= 6) { if (cap <= 0) return 0; } else cap = 6;
	const int32_t have = static_cast<int32_t>(env->maxSockets(pItem) & 0xFF);
	if (cap >= have) cap = have;
	if (nReq < 1) {
		nReq = env->rollCount(rd32(pRec + 0xC), rd32(pRec + 8), pItem);
		if (nReq < 1) nReq = static_cast<int32_t>(rd32(pRec + 4));
	}
	if (nReq < 2) nReq = 1;
	if (nReq < cap) cap = nReq;
	if (cap <= 0) return 0;
	if (rd32(pItem) == 4 && rd32(pItem + 0x14) != 0) {
		uint32_t* f = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(rd32(pItem + 0x14)) + 0x18);
		*f |= 0x800;
	}
	env->setStat(pItem, 0xC2, cap);
	return cap;
}
