// 1.13c D2Common+0x0E430: find the live entry with the given key in a maze owner's list and overwrite its pair (EAX = owner, ESI = key; stack a, b, enable; ret 0xC).
// Raw retail 32-bit layout: owner+0x10 = first node; node+0x20 = entry, node+0x24 = next; entry {+0 key/a, +4 b, +0xC flags (bit 1 = disabled)}.
#include <cstdint>
#include <emscripten.h>

extern "C"
uint32_t d2common_e430_port(void* pOwnerEAX, uint32_t nKeyESI, uint32_t nA, uint32_t nB, uint32_t nEnable)
{
	auto D = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
	uint32_t pNode = D(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pOwnerEAX)) + 0x10);
	while (pNode)
	{
		const uint32_t pEntry = D(pNode + 0x20);
		if (!((D(pEntry + 0xC) >> 1) & 1) && D(pEntry) == nKeyESI)
		{
			D(pEntry) = nA;
			D(pEntry + 4) = nB;
			if (nEnable == 0)
				D(pEntry + 0xC) |= 2;
			else
				D(pEntry + 0xC) &= ~2u;
			return pNode;
		}
		pNode = D(pNode + 0x24);
	}
	return 0;
}
