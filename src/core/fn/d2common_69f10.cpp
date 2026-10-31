// 1.13c D2Common+0x69F10 MISSILE_SetHomeType: stdcall(pMissile, pTarget), void.
// The blobs carry retail's 32-bit layout. The probe copies the scalar fields the function reads into our structs and
// writes back the two home fields it owns (data+0x20 type, +0x24 id), so the wasm64 layout does not matter.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "Units/Units.h"
#include "Units/Missile.h"

static uint32_t rd(const uint8_t* b, uint32_t off) { uint32_t v; memcpy(&v, b + off, 4); return v; }
static void wr(uint8_t* b, uint32_t off, uint32_t v) { memcpy(b + off, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_69f10(uint32_t* r) {   // r[0] pMissile blob, r[1] pTarget blob, r[4] result (void)
    const uint8_t* mb = (const uint8_t*)(uintptr_t)r[0];
    const uint8_t* tb = (const uint8_t*)(uintptr_t)r[1];
    D2UnitStrc m, t; D2MissileDataStrc md;
    memset(&m, 0, sizeof m); memset(&t, 0, sizeof t); memset(&md, 0, sizeof md);
    uint8_t* db = nullptr;
    if (mb) {
        m.dwUnitType = rd(mb, 0);
        db = (uint8_t*)(uintptr_t)rd(mb, 0x14);
        if (db) { md.nHomeType = (int32_t)rd(db, 0x20); md.dwHomeGUID = rd(db, 0x24); m.pMissileData = &md; }
    }
    if (tb) { t.dwUnitType = rd(tb, 0); t.dwUnitId = rd(tb, 0x0C); }
    MISSILE_SetHomeType(mb ? &m : nullptr, tb ? &t : nullptr);
    if (db) { wr(db, 0x20, (uint32_t)md.nHomeType); wr(db, 0x24, (uint32_t)md.dwHomeGUID); }
    r[4] = 0;   // retmask 0
}
