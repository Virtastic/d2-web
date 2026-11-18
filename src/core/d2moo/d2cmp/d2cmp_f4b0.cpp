#include <cstdint>
// 1.13c D2CMP+0x0F4B0
// Same RLE row blit as EA20 but each copied pixel goes through two maps: map2[map1[src]].
void d2cmp_f4b0(const uint8_t* src, uint8_t* dst, int rows, int stride, const uint8_t* map1, const uint8_t* map2) {
	uint8_t* row = dst + stride;
	for (;;) {
		uint8_t b = *src++;
		if (b & 0x80) {
			if (b & 0x7f) { dst += b & 0x7f; continue; }
			if (--rows == 0) return;
			dst = row; row += stride;
			continue;
		}
		unsigned n = (b & 7) + 8u * (b >> 3);
		while (n--) *dst++ = map2[map1[*src++]];
	}
}
