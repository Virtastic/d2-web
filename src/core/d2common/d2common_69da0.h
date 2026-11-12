// 1.13c D2Common+0x69DA0: the larger of two signed values. ecx = a, edx = b; the two stack words the table
// signature carries are dropped unread (ret 8). Calc callback table entry.
#pragma once
#include <cstdint>

namespace c69da0 {
inline int32_t Max(int32_t a, int32_t b) { return a > b ? a : b; }
}
