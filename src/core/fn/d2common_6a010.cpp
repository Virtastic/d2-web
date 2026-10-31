// 1.13c D2Common+0x6A010 MISSILE_GetAnimSpeed: stdcall(pMissile) -> int.
// Copies dwUnitType and the signed 16-bit wAnimSpeed (retail +0x4C) into our struct.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "Units/Units.h"
#include "Units/Missile.h"

static uint32_t rd(const uint8_t* b, uint32_t off) { uint32_t v; memcpy(&v, b + off, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6a010(uint32_t* r) {   // r[0] pMissile blob, r[4] result
    const uint8_t* b = (const uint8_t*)(uintptr_t)r[0];
    D2UnitStrc u; memset(&u, 0, sizeof u);
    if (b) { u.dwUnitType = rd(b, 0); u.wAnimSpeed = (int16_t)(rd(b, 0x4C) & 0xFFFF); }
    r[4] = (uint32_t)MISSILE_GetAnimSpeed(b ? &u : nullptr);
}
