// 1.13c D2Client+0x59910 (105 B, no arguments, cdecl): the second confirm box's "stay" button. Same callees as 0x598A0
// (0x88A70 with EBX = 2, 0xA3D20) then clears DAT 0x1177E8, sets DAT 0x11C2CC to 7, sends {'O', 2, 0, 0, 0, 0, 0}
// through 0x143E0 (EBX = 7) and only then stamps GetTickCount into DAT 0x1177F4; returns 1.
#include <emscripten.h>
#include <cstdint>
struct D2c59910Env {
    void (*c88a70)(uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void (*ca3d20)();
    uint32_t (*tick)();
    void (*c143e0)(const uint8_t* buf, uint32_t ebx);
};
extern "C" uint32_t d2c_port_59910(uint32_t* g1177e8, uint32_t* g1177f4, uint32_t* g11c2cc, const D2c59910Env* e) {
    e->c88a70(2, 0, 0, 0, 0);
    e->ca3d20();
    *g1177e8 = 0;
    *g11c2cc = 7;
    const uint8_t p[8] = {0x4F, 2, 0, 0, 0, 0, 0, 0};
    e->c143e0(p, 7);
    *g1177f4 = e->tick();
    return 1;
}
