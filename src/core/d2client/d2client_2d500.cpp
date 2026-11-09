// 1.13c D2Client+0x2D500 (EAX unit; stack a0..a9, ret 0x28). a7 is a code pointer the function calls with ECX = unit, EDX and seven
// stack words (the callee pops them); the port hands it to the callee dispatcher keyed by the pointer itself.
// u60 = unit+0x60; zero or 0xC6E8(unit) == 0: 0. r4 = 0xC262(u60, 4), kept when 0xC724(r4) and 0xC1CC(r4, 0x26) == 0, else 0;
// r5 = 0xC262(u60, 5) likewise. Both kept: 0x2C230 (EBX unit) nonzero calls a7(EDX a3; a4, a5, a8, a9, a6, 0, result), zero clears
// *a3 and *a4; then 0x2C190 (EBX unit) nonzero calls a7(EDX a0; a1, a2, a8, a9, a6, 0, result), zero clears *a0 and *a1.
// Otherwise item = 0xCA0C(u60); item and 0xC724(item): 0xC4FC(unit, item, 1); a7(EDX a0; a1, a2, a8, a9, a6, 0, 0). Returns 1.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

static uint32_t cb7(const D2cSvc& v, uint32_t fn, uint32_t unit, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3,
                    uint32_t s4, uint32_t s5, uint32_t s6) {
    const uint32_t r[5] = { 0, unit, edx, 0, 0 }, s[7] = { s0, s1, s2, s3, s4, s5, s6 };
    return v.call(v.ctx, fn, r, s);
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_2d500(const D2cSvc& v, uint32_t unit, uint32_t* a0, uint32_t* a1, uint32_t a2, uint32_t* a3,
                                                  uint32_t* a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9) {
    uint32_t u60; memcpy(&u60, (const uint8_t*)(uintptr_t)unit + 0x60, 4);
    if (!u60 || !d2c_call(v, 0xc6e8, 0, 0, 0, 0, 0, unit)) return 0;
    uint32_t r4 = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, u60, 4);
    if (!d2c_call(v, 0xc724, 0, 0, 0, 0, 0, r4) || d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, r4, 0x26)) r4 = 0;
    uint32_t r5 = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, u60, 5);
    if (!d2c_call(v, 0xc724, 0, 0, 0, 0, 0, r5) || d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, r5, 0x26)) r5 = 0;
    if (r4 && r5) {
        uint32_t k = d2c_call(v, 0x2c230, 0, 0, 0, unit, 0);
        if (k) cb7(v, a7, unit, (uint32_t)(uintptr_t)a3, (uint32_t)(uintptr_t)a4, a5, a8, a9, a6, 0, k);
        else { *a3 = 0; *a4 = 0; }
        k = d2c_call(v, 0x2c190, 0, 0, 0, unit, 0);
        if (k) cb7(v, a7, unit, (uint32_t)(uintptr_t)a0, (uint32_t)(uintptr_t)a1, a2, a8, a9, a6, 0, k);
        else { *a0 = 0; *a1 = 0; }
        return 1;
    }
    const uint32_t item = d2c_call(v, 0xca0c, 0, 0, 0, 0, 0, u60);
    if (item && d2c_call(v, 0xc724, 0, 0, 0, 0, 0, item)) d2c_call(v, 0xc4fc, 0, 0, 0, 0, 0, unit, item, 1);
    cb7(v, a7, unit, (uint32_t)(uintptr_t)a0, (uint32_t)(uintptr_t)a1, a2, a8, a9, a6, 0, 0);
    return 1;
}
