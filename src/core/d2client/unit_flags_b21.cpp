// Unit-pointer leaf, regs eax = pUnit (32-bit retail layout read as dwords).
#include <cstdint>
#include <cstddef>

struct D2cUnitB21 {          // only the offsets these leaves read
    uint8_t  pad00[0x54];
    uint32_t pPath;          // +0x54
};
static_assert(offsetof(D2cUnitB21, pPath) == 0x54, "pPath");

// 1.13c D2Client+0xC7BF0
extern "C" void d2_client_c7bf0(uint32_t pUnit) {
    if (!pUnit) return;
    uint32_t inner = ((D2cUnitB21*)(uintptr_t)pUnit)->pPath;
    if (!inner) return;
    uint32_t* f = (uint32_t*)(uintptr_t)((uint32_t*)(uintptr_t)inner)[0x30 / 4];
    if (f) *f = (*f & 0xfffffffeu) | 8u;
}

