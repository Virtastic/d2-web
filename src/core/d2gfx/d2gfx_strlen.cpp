#include <cstdint>

// The MSVC 2003 Release CRT `_strlen`, reached from D2Gfx's own code (callers 0x360C, 0x608C, 0x1E28, 0x2F66,
// 0x7300). Counts bytes before the NUL. Retail scans byte-wise to a dword boundary, then aligned dwords with the
// `(*p ^ 0xffffffff ^ *p + 0x7efefeff) & 0x81010100` zero-byte test and byte order picks the offset 0..3; the byte
// scan returns the same count for every input, so this is a plain scan.
extern "C"
// 1.13c D2Gfx+0x4150
uint32_t d2gfx_strlen(const char* s) {
    const char* p = s;
    while (*p != '\0') ++p;
    return (uint32_t)(p - s);
}
