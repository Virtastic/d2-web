// D2Common 1.13c batch: CRT 96-bit mantissa add helper.
#include <cstdint>
#include <emscripten.h>

extern "C" {
uint32_t D2Common_87C71_Addl(uint32_t nLeft, uint32_t nRight, uint32_t* pOut);

// 1.13c D2Common+0x87C92 (___add_12): 96-bit add of pB into pA, three little-endian dwords.
EMSCRIPTEN_KEEPALIVE void D2Common_87C92_Add12(uint32_t* pA, const uint32_t* pB)
{
	if (D2Common_87C71_Addl(pA[0], pB[0], pA) != 0) {
		if (D2Common_87C71_Addl(pA[1], 1, pA + 1) != 0) {
			pA[2] = pA[2] + 1;
		}
	}
	if (D2Common_87C71_Addl(pA[1], pB[1], pA + 1) != 0) {
		pA[2] = pA[2] + 1;
	}
	D2Common_87C71_Addl(pA[2], pB[2], pA + 2);
}
}
