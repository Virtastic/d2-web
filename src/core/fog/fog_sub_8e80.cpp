// 1.13c Fog+0x8E80 -- statically linked CRT strcat(dst, src); returns dst.
// Retail scans word-wise; the byte result is identical (final NUL written, nothing past it).
#include <cstdint>

extern "C" char* fog_sub_8e80(char* dst, const char* src) {
    char* p = dst;
    while (*p) ++p;
    while ((*p++ = *src++)) {}
    return dst;
}
