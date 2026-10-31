// 1.13c D2Common+0x6A110 MISSILE_SetStreamMissile: stdcall(pMissile, n), void.
// Copies the scalar fields in, runs our setter, and writes the 16-bit nStreamMissile back to the blob's missile data (+4).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "Units/Units.h"
#include "Units/Missile.h"

static uint32_t rd(const uint8_t* b, uint32_t off) { uint32_t v; memcpy(&v, b + off, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6a110(uint32_t* r) {   // r[0] pMissile blob, r[1] nStreamMissile, r[4] result (void)
    uint8_t* b = (uint8_t*)(uintptr_t)r[0];
    D2UnitStrc u; D2MissileDataStrc md; memset(&u, 0, sizeof u); memset(&md, 0, sizeof md);
    uint8_t* db = nullptr;
    if (b) {
        u.dwUnitType = rd(b, 0);
        db = (uint8_t*)(uintptr_t)rd(b, 0x14);
        if (db) { md.nStreamMissile = (int16_t)(rd(db, 4) & 0xFFFF); u.pMissileData = &md; }
    }
    MISSILE_SetStreamMissile(b ? &u : nullptr, (uint16_t)r[1]);
    if (db) { uint16_t v = (uint16_t)md.nStreamMissile; memcpy(db + 4, &v, 2); }
    r[4] = 0;   // retmask 0
}
