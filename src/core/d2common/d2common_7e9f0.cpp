// 1.13c D2Common+0x7E9F0 -- sub_7E9F0. Pure read of the level's
// block (+0x14 outdoors): 1 when no row of the block has bit 2 in cell x or x+1, else 0. ECX is unused.
#include <cstdint>

namespace {
inline uint32_t rd(uintptr_t p) { return *reinterpret_cast<const uint32_t*>(p); }
}

extern "C" uint32_t d2c_7e9f0_port(uintptr_t level, uint32_t ecx, int32_t x) {
    (void)ecx;
    if (!level) return 1;
    const uintptr_t od = rd(level + 0x14);
    const uintptr_t flags = rd(od + 0x2c);
    const uintptr_t rows = rd(od + 0x30);
    const int32_t count = (int32_t)rd(od + 0x60);
    for (int32_t y = 0; y < count; ++y) {
        const int32_t row = (int32_t)rd(rows + 4u * (uint32_t)y);
        const uint32_t o = (uint32_t)(row + x) * 4u;
        if (*reinterpret_cast<const uint8_t*>(flags + o) & 2) return 0;
        if (*reinterpret_cast<const uint8_t*>(flags + o + 4) & 2) return 0;
    }
    return 1;
}
