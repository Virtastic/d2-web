// CRT __shift, moved from the stand-alone d2common tiny batch into the game tree.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Common+0x87209 (__shift): when the offset (retail EDI) is non-zero, move the whole
// NUL-terminated string including its terminator to pStr+offset, i.e. memmove of strlen+1 bytes.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_87209_Shift(char* pStr, int32_t nOffset)
{
	if (nOffset != 0)
		std::memmove(pStr + nOffset, pStr, std::strlen(pStr) + 1);
}
