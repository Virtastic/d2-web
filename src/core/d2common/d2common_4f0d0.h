// 1.13c D2Common+0x4F0D0: the smaller of two signed values. ecx = a, edx = b; the two stack words the table
// signature carries are dropped unread (ret 8). Skill-calc callback table entry.
#pragma once
#include <cstdint>

namespace c4f0d0 {
inline int32_t Min(int32_t a, int32_t b) { return a < b ? a : b; }
}
