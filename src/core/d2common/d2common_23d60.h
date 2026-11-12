// 1.13c D2Common+0x23D60: the smaller of two signed values. ecx = a, edx = b; the two stack words the table
// signature carries are dropped unread (ret 8). Missile-calc callback table entry.
#pragma once
#include <cstdint>

namespace c23d60 {
inline int32_t Min(int32_t a, int32_t b) { return a < b ? a : b; }
}
