// D2Common 1.13c batch. Register-ABI wrapper: r[0] = EAX (nMax), r[1] = ECX (param_1),
// r[2] = EDX (param_2, struct pointer), r[3] = EDI (nMin); result in r[4].
#include <cstdint>
#include <emscripten.h>

extern "C" {
void D2Common_1180_RollRange(uint32_t* r);

// 1.13c D2Common+0x465B0: if EAX == ECX return ECX. Otherwise the two are the bounds of the roll (signed: the larger
// is the max, the smaller the min; EDI is saved, never read) on a seed chosen by EDX: kind 4 uses the seed at
// (EDX[5] + 4) (null link -> null seed, which retail dereferences when the range is non-empty), anything else (incl. a
// null EDX) uses the seed at EDX + 0x20. r[3] is unused.
void D2Common_465B0_RegRoll(uint32_t* r)
{
	const int32_t a0 = static_cast<int32_t>(r[0]), c0 = static_cast<int32_t>(r[1]);
	if (a0 == c0)
	{
		r[4] = r[1];
		return;
	}
	const int32_t nMax = a0 >= c0 ? a0 : c0, nMin = a0 >= c0 ? c0 : a0;
	const uint32_t* p2 = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(r[2]));
	uint32_t pSeed;
	if (p2 != nullptr && p2[0] == 4)
		pSeed = p2[5] != 0 ? p2[5] + 4 : 0;
	else
		pSeed = r[2] + 0x20; // retail: EDX + 8 ints, even when EDX is null
	uint32_t a[5] = { pSeed, static_cast<uint32_t>(nMin), static_cast<uint32_t>(nMax), 0, 0 };
	if (pSeed == 0 && static_cast<int32_t>(static_cast<uint32_t>(nMax) - static_cast<uint32_t>(nMin) + 1u) > 0)
		__builtin_trap(); // retail faults on the null seed pointer
	D2Common_1180_RollRange(a);
	r[4] = a[4];
}
}
