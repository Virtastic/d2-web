#include <cstdint>
// 1.13c Storm+0x2410: RGN ctor. Zeroes the dwords at +0x04..+0x0C, +0x10, +0x18..+0x44 and returns the element
// (eax). +0x00 and +0x14 are left as they were (retail never writes them).
extern "C" void* Storm_Port2410(void* e) {
    uint32_t* d = (uint32_t*)e;
    d[1] = 0; d[2] = 0; d[3] = 0; d[4] = 0;
    for (int i = 6; i <= 0x11; i++) d[i] = 0;
    return e;
}
