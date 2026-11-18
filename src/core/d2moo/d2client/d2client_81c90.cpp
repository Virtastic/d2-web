// 1.13c D2Client+0x81C90: fastcall. Runs 0x14660 with the caller's ECX (plain ret, 0x14660 injected),
// then [0xF8C9C] = 0, [0x11C404] = 1, [0x11C3A4] = 1.
#include <cstdint>

extern "C" void d2c_port_81c90(uint32_t ecx, void (*c14660)(uint32_t ecx), uint32_t* g_f8c9c, uint32_t* g_11c404, uint32_t* g_11c3a4) {
    c14660(ecx);
    *g_f8c9c = 0;
    *g_11c404 = 1;
    *g_11c3a4 = 1;
}
