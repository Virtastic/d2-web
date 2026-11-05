// 1.13c D2Common+0x69170: take the next 0x30-byte slot of the owner's slot pool (pool = owner +0x54;
// slot = array +0x1C plus 0x30 * used count +0xC), link it in front of the list whose head cell is ECX
// (the slot's +0x20 gets the old head; no cell: +0x20 = 0), count it, then let 0x68DF0 fill it.
// EAX = owner, stack a, b, c, d (ret 0x10); returns the slot.
#include <cstring>
#include "d2common_4bb70.h"

namespace
{
	uint32_t Rd(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
	void Wr(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
}

uint32_t D2Common_69170_TakeSlot(uint32_t pHead, uint32_t pOwner, int32_t nA, int32_t nB, int32_t nC, int32_t nD, D2SlotFillFn pfnFill)
{
	const uint32_t pPool = Rd(pOwner + 0x54);
	const uint32_t pSlot = Rd(pPool + 0xC) * 0x30 + Rd(pPool + 0x1C);
	if (pHead)
	{
		Wr(pSlot + 0x20, Rd(pHead));
		Wr(pHead, pSlot);
	}
	else
	{
		Wr(pSlot + 0x20, 0);
	}
	Wr(pPool + 0xC, Rd(pPool + 0xC) + 1);
	pfnFill(pOwner, pSlot, nB, nA, nC, nD);
	return pSlot;
}
