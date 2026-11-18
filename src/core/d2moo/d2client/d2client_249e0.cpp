// 1.13c D2Client+0x249E0 -- jump-table handler 18 of the unit dispatcher at 0x24680 (thiscall, ECX = unit).
// Idle-sound timer: once GetTickCount passes the stamp at unit+0xD4, a 10% roll (seed at +0x20/+0x24) fires the
// 0x88A70 callee, then the stamp is rearmed to now + (roll % 60) * 1000. Always returns 1.
#include <cstdint>
#include <cstring>
namespace {
uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
void wr(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
// the 64-bit LCG step shared by the unit handlers: seed = lo * 0x6AC690C5 + hi; returns the new low word
uint32_t step(uint8_t* u) {
    uint64_t n = (uint64_t)rd(u, 0x20) * 0x6AC690C5ull + rd(u, 0x24);
    wr(u, 0x20, (uint32_t)n); wr(u, 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n;
}
}
extern "C" uint8_t d2c_249e0(void* unit, uint32_t (*tick)(), void (*f88a70)(void*, int32_t, int32_t, int32_t)) {
    uint8_t* u = (uint8_t*)unit;
    if (tick() > rd(u, 0xD4)) {
        if (step(u) % 100 < 10) f88a70(u, 0, 0, 0);
        uint32_t s = step(u) % 60;
        wr(u, 0xD4, tick() + s * 1000);
    }
    return 1;
}
