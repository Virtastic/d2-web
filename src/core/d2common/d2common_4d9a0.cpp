// 1.13c D2Common+0x4D9A0: the same two sums as 0x4DA40, split by 32 with a bias of 31 on negatives
// (rounds toward minus infinity). Void in retail; the result register is undefined, so the probe
// pins it to 0.
#include <cstdint>

extern "C" {
// 1.13c D2Common+0x4D9A0
void D2Common_4D9A0_SplitPairShift5(int32_t a, int32_t b, int32_t* pSum, int32_t* pDiff)
{
	const int32_t s = static_cast<int32_t>(static_cast<uint32_t>(b) * 2u + static_cast<uint32_t>(a));
	*pSum = (static_cast<int32_t>(static_cast<uint32_t>(s) + (static_cast<uint32_t>(s >> 31) & 0x1Fu)) >> 5) - (s < 0 ? 1 : 0);
	const int32_t d = static_cast<int32_t>(static_cast<uint32_t>(b) * 2u - static_cast<uint32_t>(a));
	*pDiff = (static_cast<int32_t>(static_cast<uint32_t>(d) + (static_cast<uint32_t>(d >> 31) & 0x1Fu)) >> 5) - (d < 0 ? 1 : 0);
}
}
