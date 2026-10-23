// 1.13c D2Lang+0x04110: CRT strcat (cdecl, byte strings). Returns dst.
// ponytail: retail's word-at-a-time scan collapsed to a byte loop; same result bytes.
#include <cstdint>

extern "C" uint8_t* d2lang_4110_strcat(uint8_t* dst, const uint8_t* src)
{
	uint8_t* p = dst;
	while (*p) p++;
	while ((*p++ = *src++) != 0) {}
	return dst;
}
