// 1.13c D2Game+0x38E90: missile hit callback (table entry). One stack argument (the unit that was hit, or null for
// a wall hit): 2 (keep flying) when there is one, 0 otherwise (ret 4).
#pragma once
#include <cstdint>

namespace g38e90 {
inline uint32_t AlwaysPierce(uint32_t target) { return target ? 2u : 0u; }
}
