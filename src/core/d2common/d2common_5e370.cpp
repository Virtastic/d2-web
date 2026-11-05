// 1.13c D2Common+0x5E370 Copy a four-character code: the source comes in ECX, the destination in EAX.
// Bytes up to the first NUL are copied; every remaining position of the four is a space.
#include <cstdint>

extern "C" void D2Common_5E370_PadCode4(char* pDst, const char* pSrc)
{
	int32_t i = 0;
	for (; i < 4 && pSrc[i] != '\0'; ++i)
		pDst[i] = pSrc[i];
	for (; i < 4; ++i)
		pDst[i] = ' ';
}
