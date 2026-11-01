// 1.13c D2Common+0x4DAF0: two-value step on int32 pair (a, b) through pointers. Stores the
// quotients by 160 of (2b + a) into *pA and of (2b - a) into *pB, and returns the low 32 bits of
// (2b - a) times 0x66666667 (the divide magic residue, kept as retail returns it).
#include <cstdint>

extern "C" {
// 1.13c D2Common+0x4DAF0
int32_t D2Common_4DAF0_StepPair(int32_t* pA, int32_t* pB)
{
	const int32_t b = *pB;
	const int32_t a = *pA;
	const int32_t sum = static_cast<int32_t>(static_cast<uint32_t>(b) * 2u + static_cast<uint32_t>(a));
	*pA = sum / 160;
	const int32_t diff = static_cast<int32_t>(static_cast<uint32_t>(b) * 2u - static_cast<uint32_t>(a));
	*pB = diff / 160;
	return static_cast<int32_t>(static_cast<uint32_t>(diff) * 0x66666667u);
}
}
