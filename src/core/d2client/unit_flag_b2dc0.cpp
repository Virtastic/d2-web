// D2Client+0xB2DC0: unit state update; ECX = unit (retail pointer, kept in EDI). Env-seam style.
// Retail: if the predicate 0xC46C(unit) is zero, tail-jump to 0xB17C0 with ECX = unit. Otherwise OR 0x10000
// into unit+0xC4, and if unit+0x64 is non-null call 0xA8630 with ESI = that pointer, then store 0 to unit+0x64.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cB2DC0Env {
    int32_t (*pred46C)(uint32_t unit);   // D2Client+0xC46C (stdcall, one argument)
    void (*a8630)(uint32_t esi);         // D2Client+0xA8630 (pointer in ESI)
    void (*b17C0)(uint32_t unit);        // D2Client+0xB17C0 (unit in ECX; tail-jumped)
};

static inline uint32_t d2c_rd32(const uint8_t* p, uint32_t off) { uint32_t v; std::memcpy(&v, p + off, 4); return v; }
static inline void d2c_wr32(uint8_t* p, uint32_t off, uint32_t v) { std::memcpy(p + off, &v, 4); }

extern "C" {
// 1.13c D2Client+0xB2DC0
EMSCRIPTEN_KEEPALIVE void d2c_b2dc0(const D2cB2DC0Env* e, uint8_t* unit, uint32_t unitRetail) {
    if (!e->pred46C(unitRetail)) { e->b17C0(unitRetail); return; }
    if (unit) d2c_wr32(unit, 0xC4, d2c_rd32(unit, 0xC4) | 0x10000u);
    uint32_t esi = unit ? d2c_rd32(unit, 0x64) : 0;
    if (esi) {
        e->a8630(esi);
        d2c_wr32(unit, 0x64, 0);
    }
}
}
