// D2Common 1.13c batch. Register-ABI wrapper: r[0] = EAX (nMax), r[1] = ECX (param_1),
// r[2] = EDX (param_2, struct pointer), r[3] = EDI (nMin); result in r[4].
#include <cstdint>
#include <emscripten.h>

extern "C" {
void D2Common_1180_RollRange(uint32_t* r);

// 1.13c D2Common+0x465B0: if EAX == param_1 return param_1. Otherwise roll nMin..nMax on a seed
// chosen by param_2: kind 4 uses the seed at (param_2[5] + 4) (null link -> null seed, which
// retail then dereferences), anything else (incl. null param_2) uses the seed at param_2 + 0x20.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_465b0(uint32_t* r)
{
	const uint32_t p1 = r[1];
	if (r[0] == p1)
	{
		r[4] = p1;
		return;
	}
	const uint32_t* p2 = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(r[2]));
	uint32_t pSeed;
	if (p2 != nullptr && p2[0] == 4)
		pSeed = p2[5] != 0 ? p2[5] + 4 : 0;
	else
		pSeed = r[2] + 0x20; // retail: param_2 + 8 ints, even when param_2 is null
	uint32_t a[5] = { pSeed, r[3], r[0], 0, 0 };
	if (pSeed == 0)
		__builtin_trap(); // retail faults on the null seed pointer
	D2Common_1180_RollRange(a);
	r[4] = a[4];
}
}
