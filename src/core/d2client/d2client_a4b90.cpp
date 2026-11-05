// 1.13c D2Client+0xA4B90 (66 B, EAX = unit, non-null): a zero counter at +0x58 answers 0; otherwise the counter is
// decremented and the answer is 1 when ordinal 0xC1FC(unit, 0x8F) returns zero, else 0. (Null EAX is the fatal-exit path, 0xA19.)
#include <emscripten.h>
#include <cstdint>
extern "C" uint32_t d2c_port_a4b90(uint32_t unit, uint32_t (*c1fc)(uint32_t, uint32_t)) {
    uint32_t* u = (uint32_t*)(uintptr_t)unit;
    uint32_t c = u[0x58 / 4];
    if (c == 0) return 0;
    u[0x58 / 4] = c - 1;
    return c1fc(unit, 0x8F) == 0 ? 1u : 0u;
}
