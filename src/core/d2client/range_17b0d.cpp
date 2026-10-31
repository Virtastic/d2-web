// 1.13c D2Client+0x17B0D -- tail fragment of the AI step (reached with ESI = the monster unit); not a free-standing function.
#include "client_host.h"
#include <emscripten.h>

// roll = 0x1330(EAX = 100). roll < 75 (signed): when unit[0] == 1 and unit[5] (a pointer) is non-null the word at
// unit[5] + 0x30 is set to 0x1E (inner = that pointer's words, so inner[0xC]), then 0x7270 runs (returns 0).
// roll >= 75: 0x73E0(5, 1, 0) runs (returns 1).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_17b0d(D2cHost h, uint32_t unitTok, const uint32_t* unit, uint32_t* inner) {
    D2cCall r = { 0x1330, 100, 0, 0, 0, unitTok, { 0, 0, 0, 0 } };
    int32_t roll = (int32_t)h(&r);
    if (roll < 0x4B) {
        if (unit[0] == 1 && unit[5] != 0 && inner) inner[0x30 / 4] = 0x1E;
        D2cCall a = { 0x7270, 0, 0, 0, 0, unitTok, { 0, 0, 0, 0 } };
        h(&a);
        return 0;
    }
    D2cCall b = { 0x73E0, 0, 0, 0, 0, unitTok, { 5, 1, 0, 0 } };
    h(&b);
    return 1;
}
