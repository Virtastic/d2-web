// 1.13c D2Client+0xA6210 (56 B, ESI = unit, no result): clears flag 0x10000 of the dword at +0xC4; for a unit whose type
// (+0) is 1 the flag 4 is set only when the skill lookup 0x1510(unit+4, 9) answers nonzero, any other type sets it.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2c_port_a6210(uint32_t unit, uint32_t (*f1510)(uint32_t id, uint32_t sel)) {
    uint32_t* u = (uint32_t*)(uintptr_t)unit;
    uint32_t fl = u[0xC4 / 4] & 0xFFFEFFFFu;
    u[0xC4 / 4] = fl;
    if (u[0] != 1 || f1510(u[1], 9) != 0) u[0xC4 / 4] = fl | 4u;
}
