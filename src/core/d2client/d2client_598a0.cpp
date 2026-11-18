// 1.13c D2Client+0x598A0 (105 B, no arguments, cdecl): the "leave game" confirm button. Calls 0x88A70 (EBX = 2, four zero stack
// words, ret 16) and 0xA3D20, clears DAT 0x1177F0, sets the exit state DAT 0x11C2CC to 7, stamps GetTickCount into DAT 0x1177F4,
// sends the 7-byte packet {'O', 2, 0, 0, 0, 0, 0} through 0x143E0 (EBX = 7), returns 1.
#include <emscripten.h>
#include <cstdint>
struct D2c598a0Env {
    void (*c88a70)(uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void (*ca3d20)();
    uint32_t (*tick)();
    void (*c143e0)(const uint8_t* buf, uint32_t ebx);
};
extern "C" uint32_t d2c_port_598a0(uint32_t* g1177f0, uint32_t* g1177f4, uint32_t* g11c2cc, const D2c598a0Env* e) {
    e->c88a70(2, 0, 0, 0, 0);
    e->ca3d20();
    *g1177f0 = 0;
    *g11c2cc = 7;
    *g1177f4 = e->tick();
    const uint8_t p[8] = {0x4F, 2, 0, 0, 0, 0, 0, 0};
    e->c143e0(p, 7);
    return 1;
}
