// 1.13c D2Common+0x34D40 (#10488)
#include <cstdint>
// Set a dynamic path's velocity. Operates on retail's 32-bit image through the low 32 bits of a wasm address.
void D2Common_34D40_SetVelocity(uint32_t pPath, int32_t v)
{
    if (!pPath) return;
    uint8_t* p = (uint8_t*)(uintptr_t)pPath;
    if (v != *(int32_t*)(p + 0x7c)) *(uint32_t*)(p + 0x38) = 0xf;
    *(int32_t*)(p + 0x7c) = v;
    *(int32_t*)(p + 0x84) = v;
}
