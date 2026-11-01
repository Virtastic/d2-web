// 1.13c D2Common+0x4DA40: splits (2b + a) and (2b - a) by 160. The sum quotient rounds toward
// minus infinity for negatives (decompile's -1 fixup). The difference is shifted by its sign
// word; for a negative difference the return is the 0x66666667 residue, otherwise the quotient.
#include <cstdint>

extern "C" {
// 1.13c D2Common+0x4DA40
int32_t D2Common_4DA40_SplitPair(int32_t a, int32_t b, int32_t* pSum, int32_t* pDiff)
{
	const int32_t s = static_cast<int32_t>(static_cast<uint32_t>(b) * 2u + static_cast<uint32_t>(a));
	*pSum = s < 0 ? s / 160 - 1 : s / 160;
	const int32_t d = static_cast<int32_t>(static_cast<uint32_t>(b) * 2u - static_cast<uint32_t>(a));
	const int32_t sign = d >> 31;
	const int32_t q = d / 160 + sign;
	if (d < 0) {
		*pDiff = (q - 1) - sign;
		return static_cast<int32_t>(static_cast<uint32_t>(d) * 0x66666667u);
	}
	*pDiff = q - sign;
	return *pDiff;
}
}
