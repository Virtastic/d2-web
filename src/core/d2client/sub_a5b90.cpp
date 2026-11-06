// D2Client rva 0xA5B90 (CUnit.cpp): may unit a (ESI) target unit b (EDI)? Like 0xA51F0 but with mask byte 8 and lines
// 0xB7F/0xB88: either hand's skill record (thunks 0xC5BC/0xC5CE, 0xC42A, 0x15B0) flagged, then b's id (remapped through
// 0x216A0 when b is type 1, -1 fails) and a's id go to 0x9D6C0 (EDX = a+0xC, stack id). Callees in retail order.
#include <cstdint>
#include "d2client_callee.h"

static bool a5b90_hit(const D2cSvc& v, uint32_t thunk, uint32_t a, uint32_t line, const uint8_t* bitMasks) {
    const uint32_t id = d2c_call(v, thunk, 0, 0, 0, 0, 0, a);
    if (!id) return false;
    const uint32_t q = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, id, 0xB2000000u | 0xd4144, line);   // CUnit.cpp
    const uint32_t x = d2c_call(v, 0x15b0, q, 0, 0, 0, 0);
    return x && (bitMasks[8] & ((const uint8_t*)(uintptr_t)x)[7]);
}

// 1.13c D2Client+0xA5B90. bitMasks = the table the import pointer D2Client+0xCEE50 points at.
extern "C" uint32_t d2_client_a5b90(const D2cSvc& v, uint32_t a, const uint32_t* ap, const uint32_t* bp, const uint8_t* bitMasks) {
    if (!ap || !bp) return 0;
    if (ap[0] != 0 && bp[0] != 0 && bp[0] != 1) return 0;
    const bool h1 = a5b90_hit(v, 0xc5bc, a, 0xb7f, bitMasks);
    const bool h2 = a5b90_hit(v, 0xc5ce, a, 0xb88, bitMasks);
    if (!h1 && !h2) return 0;
    uint32_t id = bp[3];
    if (bp[0] == 1) {
        id = d2c_call(v, 0x216a0, 0, id, 0, 0, 0);
        if (id == 0xffffffffu) return 0;
    }
    return d2c_call(v, 0x9d6c0, 0, 0, ap[3], 0, 0, id) != 0;
}
