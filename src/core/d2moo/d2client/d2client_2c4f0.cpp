// 1.13c D2Client+0x2C4F0 (EAX item or 0, ESI unit; stack p1 value out, p2 flag out, p3, p4; ret 0x10). No unit or p4: 0.
// desc = 0xC640(ECX p4) (zero: 0); *p2 = 0; *p1 = 0xCEC8(unit); item = EAX, else 0xCA0C(unit+0x60) when that is set; 0xC220(item) == 0x26
// zeroes *p1 and *p2 (skipped when there is no item). pct = 0xC922(unit, item, p4, 0) + 0xC454(unit, 0x77, 0) + 0xCEAA(unit, (int16)desc[0], p3);
// when desc byte 4 & gdwBitMasks[3] (D2Common's table, 8) and 0xC1C6(unit, 0x145, 0) is nonzero: pct += it, *p2 = 3.
// *p1 += *p1 * pct / 100; 3 on 0xCEB0(unit), 1 on 0xCEBC(unit) into *p2. Returns 1.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_2c4f0(const D2cSvc& v, uint32_t item, uint32_t unit, int32_t* p1, int32_t* p2,
                                                  uint32_t p3, uint32_t p4) {
    if (!unit || !p4) return 0;
    const uint32_t desc = d2c_call(v, 0xc640, 0, p4, 0, 0, 0);
    if (!desc) return 0;
    *p2 = 0;
    *p1 = (int32_t)d2c_call(v, 0xcec8, 0, 0, 0, 0, 0, unit);
    if (!item) { uint32_t u60; memcpy(&u60, (const uint8_t*)(uintptr_t)unit + 0x60, 4); if (u60) item = d2c_call(v, 0xca0c, 0, 0, 0, 0, 0, u60); }
    if (item && d2c_call(v, 0xc220, 0, 0, 0, 0, 0, item) == 0x26) { *p1 = 0; *p2 = 0; }
    int32_t pct = (int32_t)d2c_call(v, 0xc922, 0, 0, 0, 0, 0, unit, item, p4, 0);
    pct += (int32_t)d2c_call(v, 0xc454, 0, 0, 0, 0, 0, unit, 0x77, 0);
    const uint8_t* d = (const uint8_t*)(uintptr_t)desc;
    int16_t w; memcpy(&w, d, 2);
    pct += (int32_t)d2c_call(v, 0xceaa, 0, 0, 0, 0, 0, unit, (uint32_t)(int32_t)w, p3);
    if (d[4] & 8) { const int32_t b = (int32_t)d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, unit, 0x145, 0); if (b) { pct += b; *p2 = 3; } }
    *p1 = (int32_t)((uint32_t)*p1 * (uint32_t)pct) / 100 + *p1;
    if (d2c_call(v, 0xceb0, 0, 0, 0, 0, 0, unit)) *p2 = 3;
    if (d2c_call(v, 0xcebc, 0, 0, 0, 0, 0, unit)) *p2 = 1;
    return 1;
}
