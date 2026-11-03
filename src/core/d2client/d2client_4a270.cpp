// 1.13c D2Client+0x4A270 -- [119663] = 1; e = 45C00(); [119675] = e * 0x16 + [F37A8]; 49D80 (EAX = e, stack 0xD35, ret 4); result 1.
#include <cstdint>
#include <emscripten.h>
#include "d2client_hcall.h"
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_4a270(D2cHost h, uint32_t* g119663, uint32_t* g119675, uint32_t gF37A8) {
    *g119663 = 1;
    uint32_t e = d2cCall(h, 0x45C00);
    *g119675 = e * 0x16 + gF37A8;
    d2cCall(h, 0x49D80, 0xD35, 0, 0, 0, e);
    return 1;
}
