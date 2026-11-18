// 1.13c D2Client+0x22B20: flip a unit's "sound refs held" flag (+0x88) and
// add/release the sound-bank refcounts (0x5DE90, delta in EBX) of the sound ids its profile names.
#include <cstdint>
#include "client_host.h"

static uint32_t go(D2cHost h, D2cCall c) { return h(&c); }

// ECX = pState (fastcall param_1), EAX = sign (>0 acquire, else release), ESI = pUnit (stale into callees).
// uTable = &DAT_6fba6c58 (0x23d dwords, each a pointer to 8 sound ids or 0). pState==0 and class > 0x23c are
// retail's fatal path (Fog #10265 -> _exit) and cannot be vectored; here they return.
// 0x5DE90 is called with ECX = id and EBX = +1 / -1 (ref delta); ids of 0 are no-ops in 0x5DE90 but still called.
extern "C" void d2_client_22b20(uint32_t uState, int32_t nSign, uint32_t uUnit, uint32_t uTable, D2cHost h) {
    uint32_t nType = go(h, { 0x22430, (uint32_t)nSign, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
    if (!uState) return;
    uint32_t* pFlag = (uint32_t*)(uintptr_t)(uState + 0x88);
    if (*pFlag == 0 ? nSign < 0 : nSign > 0) return;   // already in the requested state
    *pFlag = nSign > 0;
    uint32_t nRef = nSign > 0 ? 1u : 0xFFFFFFFFu;     // retail: EBX at the 0x5DE90 calls is the delta
    if (nType == 1) {
        uint32_t p = go(h, { 0x22A70, uUnit, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
        if (!p) return;
        static const uint8_t off[6] = { 0x04, 0x1c, 0x34, 0x3c, 0x78, 0x58 };
        for (int i = 0; i < 6; i++)
            go(h, { 0x5DE90, 0, *(uint32_t*)(uintptr_t)(p + off[i]), 0, nRef, 0, { 0, 0, 0, 0 } });
    } else if (nType == 2) {
        uint32_t c = go(h, { 0x22490, 0, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
        if (c > 0x23c) return;
        uint32_t row = ((const uint32_t*)(uintptr_t)uTable)[c];
        if (!row) return;
        for (int i = 0; i < 8; i++)
            go(h, { 0x5DE90, 0, ((const uint32_t*)(uintptr_t)row)[i], 0, nRef, 0, { 0, 0, 0, 0 } });
    }
}
