// 1.13c D2Common+0x3D040 -- sort comparator over two 16-bit keys (ECX, EDX point at them): 1 when the first is larger, -1 when smaller, 0 when equal (unsigned).
#include <cstdint>
int32_t d2common_3d040(const uint16_t* a, const uint16_t* b) { return *a > *b ? 1 : (*a < *b ? -1 : 0); }
