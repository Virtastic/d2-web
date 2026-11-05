// 1.13c D2Client+0x2C8C0 (144 B, EBX = unit or null, plain ret): 1 when the latch [0x1234A4] is set. Else walks the
// sgptDataTables skill rows (+0xB98, count +0xBA0, stride 0x23C; both re-read every step), skipping a null row; a row
// whose byte +0xC equals the unit's byte +4 (0xFF for null) calls D2Common 10708(unit, i, -1) and, when that is nonzero,
// 10306(unit, it, 0); a nonzero answer sets the latch and returns 1. 0 when no row answers.
#include <cstdint>
#include <cstring>
struct D2Client2C8C0Env {
    uint32_t* latch;                                   // [0x1234A4]
    const uint8_t* (*dt)();                            // *sgptDataTables
    uint32_t (*skill)(const uint8_t* unit, uint32_t i, int32_t m1);   // D2Common 10708 (thunk 0xC4EA)
    uint32_t (*check)(const uint8_t* unit, uint32_t s, uint32_t z);   // D2Common 10306 (thunk 0xC718)
};
static uint32_t rd32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
extern "C" uint32_t d2c_port_2c8c0(const uint8_t* unit, const D2Client2C8C0Env& e) {
    if (*e.latch) return 1;
    const uint8_t* dt = e.dt();
    for (int32_t i = 0; i < (int32_t)rd32(dt + 0xBA0); dt = e.dt(), ++i) {
        uint32_t row = rd32(dt + 0xB98) + (uint32_t)i * 0x23Cu;
        if (!row) continue;
        uint8_t cl = unit ? unit[4] : 0xFF;
        if (((const uint8_t*)(uintptr_t)row)[0xC] != cl) continue;
        uint32_t s = e.skill(unit, (uint32_t)i, -1);
        if (s && e.check(unit, s, 0)) { *e.latch = 1; return 1; }
    }
    return 0;
}
