// 1.13c D2Common+0x712D0 -- sort comparator (cdecl) over two records by the unsigned 32-bit key at +4: 1 when the first is larger, -1 when smaller, 0 when equal.
#include <cstdint>
int32_t d2common_712d0(const uint32_t* a, const uint32_t* b) { return a[1] > b[1] ? 1 : (a[1] < b[1] ? -1 : 0); }
