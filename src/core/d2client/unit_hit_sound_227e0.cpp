// D2Client unit hit/action sound selection.
#include <cstdint>
#include "client_host.h"

static uint32_t go(D2cHost h, D2cCall c) { return h(&c); }

// 1.13c D2Client+0x227E0. EAX = pUnit (register), EDI = pOut1, [esp+4] = dwLastHitClass, [esp+8] = nMode,
// [esp+0xc] = pOut2; `ret 0xc`, void. Both out dwords are zeroed at entry. Callees go through the host seam in
// retail's call order: 0x22430 (type, ESI = pUnit; retail's stale EAX is still pUnit there), 0x22490 (class,
// ESI = pUnit, retail's EAX is the 1 just returned by 0x22430) and 0x1510 (EAX = class, EDX = 0xD).
// EAX == 0 at entry is retail's fatal path (Fog #10265 -> D2FogAssert -> _exit(-1)) and cannot be vectored.
extern "C" void d2_client_227e0(uint32_t uUnit, uint32_t uOut1, uint32_t dwLastHitClass, int32_t nMode, uint32_t uOut2, D2cHost h) {
    int32_t* pOut1 = (int32_t*)(uintptr_t)uOut1;
    int32_t* pOut2 = (int32_t*)(uintptr_t)uOut2;
    uint32_t nType = go(h, { 0x22430, uUnit, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
    *pOut1 = 0;
    *pOut2 = 0;
    if (nType == 1) {
        uint32_t nClass = go(h, { 0x22490, 1, 0, 0, 0, uUnit, { 0, 0, 0, 0 } });
        if (go(h, { 0x1510, nClass, 0, 0xD, 0, 0, { 0, 0, 0, 0 } }) != 0) return;   // MonStats2 flag bit 0xD (CRITTER)
    }
    switch (dwLastHitClass & 0xf) {          // impact sound id
    case 1: *pOut1 = 0x141; break;
    case 2: case 3: case 4: case 5: *pOut1 = 0x155; break;
    case 6: case 7: *pOut1 = 0x14F; break;
    case 8: case 9: *pOut1 = 0x149; break;
    case 10: case 11: *pOut1 = 0x15B; break;
    case 12: *pOut1 = 0x145; break;
    }
    switch (dwLastHitClass & 0xf0) {         // action/body sound id
    case 0x10: if (*pOut1 != 0) *pOut2 = 0x17B; return;
    case 0x20: *pOut2 = (*pOut1 != 0) ? 0x164 : 0x161; return;   // (-(out1 != 0) & 3) + 0x161
    case 0x30: if (*pOut1 == 0) { *pOut2 = 0x167; return; }
               if ((nType != 0 && nType != 1) || nMode != 0) { *pOut2 = 0x16A; return; }
               break;
    case 0x40: *pOut2 = (*pOut1 != 0) ? 0x174 : 0x170; return;
    case 0x50: if (*pOut1 == 0 || (nType != 0 && nType != 1) || nMode != 0) { *pOut2 = 0x178; return; }
               break;
    case 0x60: *pOut2 = 0x17E; return;
    case 0x70: *pOut2 = 0x18B; return;
    case 0x80: *pOut2 = 0x956; return;
    case 0xB0: *pOut2 = 0x187; return;
    }
}
