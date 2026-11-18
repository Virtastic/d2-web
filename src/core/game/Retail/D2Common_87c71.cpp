// CRT ___addl, moved from the stand-alone d2common tiny batch into the game tree.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Common+0x87C71 (___addl): add two unsigned dwords, store the low 32-bit sum through pOut,
// and return 1 when the add carries out of bit 31. A carry out shows up as the sum being below
// either operand.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_87C71_Addl(uint32_t nLeft, uint32_t nRight,
	uint32_t* pOut)
{
	const uint32_t nSum = nLeft + nRight;
	const uint32_t nCarry = (nSum < nLeft || nSum < nRight) ? 1u : 0u;
	*pOut = nSum;
	return nCarry;
}
