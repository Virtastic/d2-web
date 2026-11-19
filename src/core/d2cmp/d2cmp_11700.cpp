// 1.13c D2CMP+0x11700 (#10000): return the dword at +0x8 of the object; null is fatal in retail
// The probe src/core/fn/d2cmp_11700.cpp drives this body.
#include <cstdint>
#include <cstddef>
#include <unistd.h>

struct CmpObj_11700 { uint8_t pad[0x8]; uint32_t dwValue; };
static_assert(offsetof(CmpObj_11700, dwValue) == 0x8, "D2CMP obj +0x8");

// 1.13c D2CMP+0x11700
extern "C" uint32_t d2cmp_11700(const void* obj) {
    if (!obj) _exit(-1);
    return ((const CmpObj_11700*)obj)->dwValue;
}
