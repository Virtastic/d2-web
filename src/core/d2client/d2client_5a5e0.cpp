// 1.13c D2Client+0x5A5E0 -- debug-overlay toggle. gl: +0x00 0x11C2C0 flag, +0x08 0x11C2C8 detail. flag := (flag == 0), detail := 0; when set,
// the 0xD16E (Ordinal 10073) record, if any, has the words 1,2,4,5,9,10,11,12,13,14,0x12,0x13,0x14,0x18,0x19,0x1A zeroed; then 0xC142 (Ordinal 10064).
#include "cl_env.h"
extern "C" void d2c_5a5e0(const cl::Env& e) {
    const uint32_t g = e.gl;
    const uint32_t f = e.R(g) == 0;
    e.W(g, f);
    e.W(g + 8, 0);
    if (!f) return;
    const uint32_t r = e.H(0xd16e);
    if (r) {
        static const uint8_t o[] = {4, 8, 0x10, 0x14, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x48, 0x4c, 0x50, 0x60, 0x64, 0x68};
        for (uint8_t k : o) e.W(r + k, 0);
    }
    e.H(0xc142);
}
