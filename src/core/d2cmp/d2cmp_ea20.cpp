#include <cstdint>
// 1.13c D2CMP+0x0EA20
// Palette-mapped run-length row blit. src: control bytes; bit7 set = skip (b&0x7f)
// dst pixels (0x80 alone = end of row); else copy (b&7) + 8*(b>>3) bytes through map[].
// rows rows, stride bytes apart; returns after the rows-th end-of-row.
void d2cmp_ea20(const uint8_t* src, uint8_t* dst, int rows, int stride, const uint8_t* map) {
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
		while (n--) *dst++ = map[*src++];
	}
}
