// 1.13c D2Client+0x24490 (UnitSnd.cpp) --
// EAX = pUnit (kept in ESI), [esp+4] = nMode, [esp+8] = bUseGiven, ret 8, void. The unit-mode sound
// dispatch: picks the mode (the given one, or the effective one from 0x223D0 when bUseGiven == 0) and
// routes a player (type 0) or monster (type 1) to the matching sound handler. Reads no global itself;
// the monster path reads three {mode, counter, replacement} triples of the 0x22A70 profile.
#include <cstdint>
#include <emscripten.h>

#include "client_host.h"

static uint32_t go(D2cHost h, D2cCall c) { return h(&c); }
static uint8_t  b8(uint32_t p, uint32_t o)  { return *(const uint8_t*)(uintptr_t)(p + o); }
static int32_t  i32(uint32_t p, uint32_t o) { return *(const int32_t*)(uintptr_t)(p + o); }

// uUnit is retail's EAX/ESI register value. 0x22CA0 takes its mode in EDI, which D2cCall cannot carry:
// it travels in the edx slot (the probe's hook does not log edx for it).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_24490(uint32_t uUnit, uint32_t nMode, uint32_t bUseGiven, D2cHost h) {
    uint32_t type = go(h, { 0x22430, uUnit, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
    uint32_t cls  = go(h, { 0x22490, type, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
    uint32_t m = nMode;
    if (bUseGiven == 0) m = go(h, { 0x223D0, 0, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });

    if (type == 0) {
        switch (m) {
        case 0: case 4: case 0x13:
            go(h, { 0x23630, uUnit, 0, 0, 0, uUnit, { m, 0, 0, 0 } }); return;
        case 7: case 8:
            go(h, { 0x242B0, 1, uUnit, 0, 0, uUnit, { m, 0, 0, 0 } }); return;
        case 9:
            go(h, { 0x22D60, 0, 0, 0, 0, uUnit, { 0, 0, 0, 0 } }); return;
        case 0xC:
            go(h, { 0x88A70, 0, 0, 0, 0xFF, uUnit, { uUnit, cls == 6 ? 4u : 0u, 0, 0 } }); return;
        case 0xF:
            if (cls == 4) go(h, { 0x242B0, 1, uUnit, 0, 0, uUnit, { 0xF, 0, 0, 0 } });
            return;
        case 0x10:
            if (cls == 6) go(h, { 0x242B0, 1, uUnit, 0, 0, uUnit, { 0x10, 0, 0, 0 } });
            return;
        }
    } else if (type == 1) {
        uint32_t p = go(h, { 0x22A70, uUnit, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
        if (p == 0) return;
        for (uint32_t o = 0x7C; o <= 0x8C; o += 8)
            if (m == b8(p, o) && i32(p, o + 4) < 0) m = b8(p, o + 1);
        switch (m) {
        case 0: case 3: case 0xD:
            go(h, { 0x23630, uUnit, 0, 0, 0, uUnit, { m, 0, 0, 0 } }); return;
        case 4: case 5:
            go(h, { 0x242B0, 1, uUnit, 0, 0, uUnit, { m, 0, 0, 0 } }); return;
        case 8: case 9: case 0xA: case 0xB:
            go(h, { 0x22CA0, uUnit, 0, m, 0, uUnit, { 0, 0, 0, 0 } }); return;
        case 0xC:
            go(h, { 0x22DA0, uUnit, 0, 0, 0, uUnit, { 0, 0, 0, 0 } }); return;
        }
    }
}
