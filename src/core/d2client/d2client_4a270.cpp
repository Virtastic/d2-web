// 1.13c D2Client+0x4A270 -- [119663] = 1; e = 45C00(); [119675] = &row table F37A8 + e * 0x16 (the address, not the dword there);
// 49D80 (EAX = e, stack 0xD35, ret 4); result 1. rows = the table's address in the image (retail 0x6FBA37A8).
#include <cstdint>
#include <emscripten.h>
#include "d2client_hcall.h"
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_4a270(D2cHost h, uint32_t* g119663, uint32_t* g119675, uint32_t rows) {
    *g119663 = 1;
    uint32_t e = d2cCall(h, 0x45C00);
    *g119675 = rows + e * 0x16;
    d2cCall(h, 0x49D80, 0xD35, 0, 0, 0, e);
    return 1;
}
