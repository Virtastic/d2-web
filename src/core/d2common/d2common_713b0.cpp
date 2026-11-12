// 1.13c D2Common+0x713B0 -- cdecl sort comparator over two records: compares the signed 16-bit fields at +2; returns -1 when the first is smaller, 1 when they differ otherwise, 0 when equal.
#include <cstdint>
uint32_t d2common_713b0(const int16_t* a, const int16_t* b)
{
    if (a[1] < b[1]) return ~0u;
    return a[1] != b[1];
}
