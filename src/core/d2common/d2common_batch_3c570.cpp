// 1.13c D2Common implementation batch: +0x3C570.
// Reads a stat-list slot out of a room/unit pair. The unit's +0x48 block supplies a base address;
// the second argument's +0x10 stat list either short-circuits the check (its +0x44 byte set, or its
// +0x1C dword not equal to base+0xA0) or falls back to the dword at base+0x1A8. A non-null
// candidate yields its +0x30 dword, otherwise 0. Retail's two fatal paths are modelled as 0.
#include <cstdint>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// 1.13c D2Common+0x3C570 (#10547): resolve the +0x30 slot of a stat record.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_3C570_GetLinkedSlot30(uint32_t pUnit, uint32_t pArg)
{
	if (!pUnit || !pArg)  // fatal 0x3CE / 0x3D1 in retail
		return 0;

	uint8_t* unit = P(pUnit);
	const int32_t nBase = static_cast<int32_t>(W(unit, 0x48));
	uint8_t* statList = P(W(P(pArg), 0x10));
	int32_t nCandidate = static_cast<int32_t>(W(statList, 0x1C));

	bool bTake = (*reinterpret_cast<int8_t*>(statList + 0x44) != 0) || (nCandidate != nBase + 0xA0);
	if (!bTake)
	{
		nCandidate = static_cast<int32_t>(W(P(static_cast<uint32_t>(nBase)), 0x1A8));
		bTake = (nCandidate != nBase + 0x18C);
	}

	if (bTake && nCandidate != 0)
		return W(P(static_cast<uint32_t>(nCandidate)), 0x30);

	return 0;
}
