// 1.13c D2Common+0x6A0E0 MISSILE_GetStreamMissile: stdcall(pMissile) -> int.
// Copies dwUnitType, the missile data pointer (+0x14, a blob address) and its nStreamMissile (data+4) into our structs.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "Units/Units.h"
#include "Units/Missile.h"

static uint32_t rd(const uint8_t* b, uint32_t off) { uint32_t v; memcpy(&v, b + off, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6a0e0(uint32_t* r) {   // r[0] pMissile blob, r[4] result
    const uint8_t* b = (const uint8_t*)(uintptr_t)r[0];
    D2UnitStrc u; D2MissileDataStrc md; memset(&u, 0, sizeof u); memset(&md, 0, sizeof md);
    if (b) {
        u.dwUnitType = rd(b, 0);
        const uint8_t* db = (const uint8_t*)(uintptr_t)rd(b, 0x14);
        if (db) { md.nStreamMissile = (int16_t)(rd(db, 4) & 0xFFFF); u.pMissileData = &md; }
    }
    r[4] = (uint32_t)MISSILE_GetStreamMissile(b ? &u : nullptr);
}
