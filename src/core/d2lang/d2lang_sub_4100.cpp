// 1.13c D2Lang+0x04100: MSVC CRT strcpy (word-at-a-time); copies through the NUL, returns dest.
#include <cstdint>

extern "C" uint8_t* d2lang_4100_strcpy(uint8_t* dst, const uint8_t* src)
{
	uint8_t* d = dst;
	while ((*d++ = *src++) != 0) {}
	return dst;
}
