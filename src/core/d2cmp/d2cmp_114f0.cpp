// 1.13c D2CMP+0x114F0 (#10098): return the dword at +0x18 of the object; null is fatal in retail
// The probe src/core/fn/d2cmp_114f0.cpp drives this body.
#include <cstdint>
#include <cstddef>
#include <unistd.h>

struct CmpObj_114F0 { uint8_t pad[0x18]; uint32_t dwValue; };
static_assert(offsetof(CmpObj_114F0, dwValue) == 0x18, "D2CMP obj +0x18");

// 1.13c D2CMP+0x114F0
extern "C" uint32_t d2cmp_114f0(const void* obj) {
    if (!obj) _exit(-1);
    return ((const CmpObj_114F0*)obj)->dwValue;
}
