// 1.13c D2Client+0x2C3D0 (ECX unit, EDX gate; stack p1 desc, p2, p3 value out, p4 flag out, p5, p6 unused; ret 0x18).
// item = unit+0x60 ? 0xCA0C(unit+0x60) : 0; *p3 = *p4 = 0; no gate: returns p4 (EAX still holds it).
// With an item: slot = 0, k = 0xC51A(item, &slot), 0xC4FC(unit, item, 0), *p3 = 0xCEC8(unit), then 0xC4FC(unit, item, slot) when k,
// else 0xC544(unit, item). Without: *p3 = 0xCEC8(unit). pct = 0xC454(unit, 0x77, 0) + 0xCEAA(unit, (int16)desc[0], p2); when
// desc byte 4 & gdwBitMasks[3] (8) and 0xC1C6(unit, 0x145, 0) is nonzero: pct += it, *p4 = 3. *p3 += *p3 * pct / 100;
// *p4 = 3 on 0xCEB0(unit); returns 0xCEBC(unit), and *p4 = 1 when it is nonzero.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_2c3d0(const D2cSvc& v, uint32_t unit, uint32_t gate, uint32_t desc, uint32_t p2,
                                                  int32_t* p3, int32_t* p4) {
    uint32_t u60, item = 0; memcpy(&u60, (const uint8_t*)(uintptr_t)unit + 0x60, 4);
    if (u60) item = d2c_call(v, 0xca0c, 0, 0, 0, 0, 0, u60);
    *p3 = 0; *p4 = 0;
    if (!gate) return (uint32_t)(uintptr_t)p4;
    if (item) {
        uint32_t slot = 0;
        const uint32_t k = d2c_call(v, 0xc51a, 0, 0, 0, 0, 0, item, (uint32_t)(uintptr_t)&slot);
        d2c_call(v, 0xc4fc, 0, 0, 0, 0, 0, unit, item, 0);
        *p3 = (int32_t)d2c_call(v, 0xcec8, 0, 0, 0, 0, 0, unit);
        if (k) d2c_call(v, 0xc4fc, 0, 0, 0, 0, 0, unit, item, slot);
        else d2c_call(v, 0xc544, 0, 0, 0, 0, 0, unit, item);
    } else {
        *p3 = (int32_t)d2c_call(v, 0xcec8, 0, 0, 0, 0, 0, unit);
    }
    int32_t pct = (int32_t)d2c_call(v, 0xc454, 0, 0, 0, 0, 0, unit, 0x77, 0);
    const uint8_t* d = (const uint8_t*)(uintptr_t)desc;
    int16_t w; memcpy(&w, d, 2);
    pct += (int32_t)d2c_call(v, 0xceaa, 0, 0, 0, 0, 0, unit, (uint32_t)(int32_t)w, p2);
    if (d[4] & 8) { const int32_t b = (int32_t)d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, unit, 0x145, 0); if (b) { pct += b; *p4 = 3; } }
    *p3 = (int32_t)((uint32_t)*p3 * (uint32_t)pct) / 100 + *p3;
    if (d2c_call(v, 0xceb0, 0, 0, 0, 0, 0, unit)) *p4 = 3;
    const uint32_t r = d2c_call(v, 0xcebc, 0, 0, 0, 0, 0, unit);
    if (r) *p4 = 1;
    return r;
}
