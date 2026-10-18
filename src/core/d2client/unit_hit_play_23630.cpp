// 1.13c D2Client+0x23630 (UnitSnd.cpp) --
// EAX = pUnit (also ESI), [esp+4] = nMode, ret 4, void. The struck-unit sound dispatch: asks the
// verified 0x227E0 for the impact/action pair, submits both to the sound queue (0x88A70), then the
// per-type reaction sound (MonSounds profile for a monster, the class table for a player). Reads
// [0x11BBFC] and [0x1197F8], writes unit+0x7C and [0x11C4C4]; all three globals are host parameters.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "../d2moo/include/Units/Units.h"
#include "client_host.h"

static_assert(offsetof(D2UnitStrc, dwSfxTicks)     == 0x7C + 12 * (sizeof(void*) - 4), "D2UnitStrc::dwSfxTicks = retail +0x7C");
static_assert(offsetof(D2UnitStrc, dwLastHitClass) == 0xB0 + 18 * (sizeof(void*) - 4), "D2UnitStrc::dwLastHitClass = retail +0xB0");

extern "C" void d2_client_227e0(uint32_t uUnit, uint32_t uOut1, uint32_t dwLastHitClass, int32_t nMode, uint32_t uOut2, D2cHost h);

static uint32_t go(D2cHost h, D2cCall c) { return h(&c); }

// The rule proper. uUnit is retail's EAX/ESI register value (the address every callee is handed); the
// FN probe passes the retail-layout unit blob there while the field this body reads and writes travel
// as dwHit and pSfxTicks. uFrameTick = D2Client+0x1197F8; pOutFrameTick is the D2Client+0x11C4C4 cell
// (stamped on the normal path, untouched on the three early returns).
static void rule(uint32_t uUnit, uint32_t dwHit, int32_t nMode, uint32_t uPlayerUnit,
                 uint32_t uFrameTick, uint32_t* pSfxTicks, uint32_t* pOutFrameTick, D2cHost h) {
    int32_t nType = (int32_t)go(h, { 0x22430, uUnit, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
    if (nType == 1 && go(h, { 0xC1FC, 0, 0, 0, 0, 0, { uUnit, 0x92, 0, 0 } }) != 0) return;

    int32_t out1, out2;
    d2_client_227e0(uUnit, (uint32_t)(uintptr_t)&out1, dwHit, nMode, (uint32_t)(uintptr_t)&out2, h);
    uint32_t r1 = go(h, { 0x88A70, 0, 0, 0, (uint32_t)out1, 0, { uUnit, 0, 0, 0 } });
                 go(h, { 0x88A70, 0, 0, 0, (uint32_t)out2, 0, { uUnit, 0, 0, 0 } });
    if (r1 != 0 && uUnit == uPlayerUnit)
        go(h, { 0x87ED0, 0, 0, r1, 0, 0, { 0xFF, 0, 0, 0 } });
    if (nType == 1 && nMode == 0) {
        go(h, { 0x22DA0, uUnit, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
        go(h, { 0x22B20, 0xFFFFFFFF, uUnit, 0, 0, uUnit, { 0, 0, 0, 0 } });
    }
    uint32_t c1 = go(h, { 0xC1FC, 0, 0, 0, 0, 0, { uUnit, 1, 0, 0 } });
    if (c1 != 0) return;
    if ((dwHit & 0xF0) == 0xA0) return;

    if (nType == 1) {
        uint32_t p = go(h, { 0x22A70, uUnit, 0, 0, 0, 0, { 0, 0, 0, 0 } });
        if (p == 0) goto end;
        uint32_t id, arg;
        if (nMode == 3 || nMode == 0xD) {
            if (go(h, { 0x87BD0, 0, 0, 0, uUnit, 0, { 0, 0, 0, 0 } }) != 0) goto end;
            id = *(const uint32_t*)(uintptr_t)(p + 0x34);    // HitSound
            arg = *(const uint32_t*)(uintptr_t)(p + 0x38);   // HitDelay
        } else {
            id = *(const uint32_t*)(uintptr_t)(p + 0x3C);    // DeathSound
            arg = *(const uint32_t*)(uintptr_t)(p + 0x40);   // DeaDelay
        }
        if (id != 0) go(h, { 0x88A70, 0, 0, 0, id, 0, { uUnit, arg, 0, 0 } });
        if (id == 0x2E9) {
            go(h, { 0x88A70, 0, 0, 0, 0x2EA, 0, { uUnit, 0x6A, 0, 0 } });
            go(h, { 0x88A70, 0, 0, 0, 0x2EB, 0, { uUnit, 0xC1, 0, 0 } });
        }
    } else if (nType == 0) {
        uint32_t nClass = go(h, { 0x22490, c1, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
        uint32_t p = go(h, { 0x222C0, nClass, 0, 0, 0, 0, { 0, 0, 0, 0 } });
        if (p == 0) goto end;
        uint32_t id, arg;
        if (nMode == 4 || nMode == 0x13) {
            if (go(h, { 0x87BD0, 0, 0, 0, uUnit, 0, { 0, 0, 0, 0 } }) != 0) goto end;
            id = *(const uint32_t*)(uintptr_t)p;
            arg = 2;
        } else {
            id = *(const uint32_t*)(uintptr_t)(p + 4);
            arg = 1;
        }
        if (id == 0) goto end;
        go(h, { 0x88A70, 0, 0, 0, id, 0, { uUnit, arg, 0, 0 } });
    } else goto end;

end:
    *pSfxTicks = uFrameTick;
    *pOutFrameTick = uFrameTick;
}

// Engine-facing entry: pPlayerUnit = D2Client+0x11BBFC, pOutFrameTick = D2Client+0x11C4C4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_23630(D2UnitStrc* pUnit, int32_t nMode,
                                                     D2UnitStrc* pPlayerUnit, uint32_t uFrameTick,
                                                     uint32_t* pOutFrameTick, D2cHost h) {
    rule((uint32_t)(uintptr_t)pUnit, pUnit->dwLastHitClass, nMode, (uint32_t)(uintptr_t)pPlayerUnit,
         uFrameTick, &pUnit->dwSfxTicks, pOutFrameTick, h);
}

// Probe entry: retail register values and the field this body reads
// arrive separately, so the unit address in the hook log stays the retail-layout blob.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_23630_rule(uint32_t uUnit, uint32_t dwHit, int32_t nMode,
                                                          uint32_t uPlayerUnit, uint32_t uFrameTick,
                                                          uint32_t* pSfxTicks, uint32_t* pOutFrameTick,
                                                          D2cHost h) {
    rule(uUnit, dwHit, nMode, uPlayerUnit, uFrameTick, pSfxTicks, pOutFrameTick, h);
}
