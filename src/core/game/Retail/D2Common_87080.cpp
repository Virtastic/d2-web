// 1.13c D2Common CRT helper __alldiv, moved from the stand-alone d2common batch into the game tree.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Common+0x87080: the CRT's signed 64-bit division helper __alldiv. Each operand is a pair
// of 32-bit halves. The routine negates each operand, divides unsigned and negates the quotient when
// exactly one operand was negative. A zero divisor reaches integer division by zero in retail and
// raises the processor's divide-error exception; preserve that trap in the wasm implementation.
extern "C"
EMSCRIPTEN_KEEPALIVE uint64_t D2Common_87080_Alldiv(uint32_t nDividendLow, uint32_t nDividendHigh,
                                                    uint32_t nDivisorLow, uint32_t nDivisorHigh)
{
	const uint64_t nDividend = (static_cast<uint64_t>(nDividendHigh) << 32) | nDividendLow;
	const uint64_t nDivisor  = (static_cast<uint64_t>(nDivisorHigh) << 32) | nDivisorLow;
	if (nDivisor == 0)
		__builtin_trap();

	const bool bNegDividend = (nDividendHigh & 0x80000000u) != 0;
	const bool bNegDivisor  = (nDivisorHigh & 0x80000000u) != 0;
	const uint64_t nAbsDividend = bNegDividend ? 0 - nDividend : nDividend;
	const uint64_t nAbsDivisor  = bNegDivisor ? 0 - nDivisor : nDivisor;

	uint64_t nQuotient = nAbsDividend / nAbsDivisor;
	if (bNegDividend != bNegDivisor)
		nQuotient = 0 - nQuotient;
	return nQuotient;
}
