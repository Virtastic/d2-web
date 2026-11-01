// D2Common 1.13c 0x1B9D0: copy up to four characters of a NUL-terminated name into a four-byte field,
// padding with spaces from the first NUL (an empty name is four spaces). No terminator is written.
#include <cstdint>

// 1.13c D2Common+0x1B9D0. Retail passes dst in EAX and src in ECX; the port takes them as parameters.
void D2Common_1B9D0_CopyFieldChars(char* dst, const char* src) {
    int32_t i = 0;
    for (; i < 4 && src[i]; ++i) dst[i] = src[i];
    for (; i < 4; ++i) dst[i] = ' ';
}
