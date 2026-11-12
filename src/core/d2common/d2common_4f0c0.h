// 1.13c D2Common+0x4F0C0: the larger of two signed values. ecx = a, edx = b; the two stack words the table
// signature carries are dropped unread (ret 8). Skill-calc callback table entry.
#pragma once
#include <cstdint>

namespace c4f0c0 {
inline int32_t Max(int32_t a, int32_t b) { return a > b ? a : b; }
}
