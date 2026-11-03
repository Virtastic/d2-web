// 1.13c D2Client+0x3D3C0: draw a count as text. EBX = count, stack (ret 8) = x, y. A count <= 0 draws nothing. Otherwise the decimal digits go into a
// 16-wchar buffer; while 0xD35A (text width, ECX = buffer) exceeds 0x28 the last character is cut (starting at the terminator slot); then one more
// 0xD35A and 0xD37E(ECX = buffer, EDX = x + 4, y - 1, 3, 0).
#include "d2c_seam.h"
extern "C" void d2c_3d3c0(const D2cSeam* s, int32_t count, uint32_t x, uint32_t y) {
    if (count <= 0) return;
    uint16_t w[16] = {};
    char a[16]; int32_t n = 0;
    for (uint32_t v = (uint32_t)count; v; v /= 10) a[n++] = (char)('0' + v % 10);
    for (int32_t i = 0; i < n; ++i) w[i] = (uint16_t)a[n - 1 - i];
    int32_t p = n;
    int32_t width = (int32_t)(*s)(0xd35a, 0, (uintptr_t)w);
    while (width > 0x28) {
        if (p >= 0) w[p] = 0;
        --p;
        width = (int32_t)(*s)(0xd35a, 0, (uintptr_t)w);
    }
    (*s)(0xd35a, 0, (uintptr_t)w);
    (*s)(0xd37e, 0, (uintptr_t)w, x + 4, 0, 0, y - 1, 3, 0);
}
