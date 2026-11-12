// 1.13c D2Common+0x23D50: the larger of two signed values. ecx = a, edx = b; the two stack words the table
// signature carries are dropped unread (ret 8). Missile-calc callback table entry.
#pragma once
#include <cstdint>

namespace c23d50 {
inline int32_t Max(int32_t a, int32_t b) { return a > b ? a : b; }
}
