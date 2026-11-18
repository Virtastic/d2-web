// D2Client+0x81B90: save the three skill selections and the two item handles of a unit (EAX = unit/handle, ESI = out, 7 words).
#include <cstdint>
#include <emscripten.h>
#include "d2client_callee.h"

// 1.13c D2Client+0x81B90. out: [0..2] = #C1C6(h,6/8/0xA,0), [3]/[5] = #C42A(item2,..,0x161)/#C7AE(item2), [4]/[6] = same for item1 (0x155).
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_81b90(const D2cSvc& v, uint32_t h, uint32_t* out) {
    const uint32_t file = 0xB2000000u | 0xd4964;   // "..\Source\D2Client\UNIT\Item.cpp"
    out[0] = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, h, 6, 0);
    out[1] = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, h, 8, 0);
    out[2] = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, h, 0xa, 0);
    const uint32_t a = d2c_call(v, 0xc5bc, 0, 0, 0, 0, 0, h);
    if (!a) { out[6] = 0xffffffffu; out[4] = 0; }
    else {
        out[6] = d2c_call(v, 0xc7ae, 0, 0, 0, 0, 0, a);
        out[4] = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, a, file, 0x155);
    }
    const uint32_t b = d2c_call(v, 0xc5ce, 0, 0, 0, 0, 0, h);
    if (!b) { out[5] = 0xffffffffu; out[3] = 0; return; }
    out[5] = d2c_call(v, 0xc7ae, 0, 0, 0, 0, 0, b);
    out[3] = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, b, file, 0x161);
}
