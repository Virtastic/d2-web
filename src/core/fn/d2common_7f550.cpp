// 1.13c D2Common+0x7f550 UNITS_IsInRange: r[0]=unit blob (type +0, path pointer +0x2C), r[1]=coord blob (nX +0, nY +4), r[2]=distance.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "Units/Units.h"
#include "Path/Path.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7f550(uint32_t* r) {
    const uint8_t* b = (const uint8_t*)(uintptr_t)r[0];
    const uint8_t* c = (const uint8_t*)(uintptr_t)r[1];
    D2UnitStrc unit = {}; D2DynamicPathStrc dp = {}; D2StaticPathStrc sp = {};
    D2CoordStrc coord = {};
    uint32_t t, p; std::memcpy(&t, b, 4); std::memcpy(&p, b + 0x2C, 4);
    unit.dwUnitType = (D2C_UnitTypes)t;
    const uint8_t* pb = (const uint8_t*)(uintptr_t)p;
    if (t == 2 || (t > 3 && t < 6)) {
        std::memcpy(&sp.tGameCoords.nX, pb + 0xC, 4); std::memcpy(&sp.tGameCoords.nY, pb + 0x10, 4);
        unit.pStaticPath = &sp;
    } else if (p) {
        uint16_t x, y; std::memcpy(&x, pb + 2, 2); std::memcpy(&y, pb + 6, 2);
        dp.tGameCoords.wPosX = x; dp.tGameCoords.wPosY = y;
        unit.pDynamicPath = &dp;
    }
    std::memcpy(&coord.nX, c, 4); std::memcpy(&coord.nY, c + 4, 4);
    r[4] = (uint32_t)UNITS_IsInRange(&unit, &coord, (int)r[2]);
}
