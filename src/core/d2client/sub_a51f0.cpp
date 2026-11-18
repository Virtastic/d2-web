// D2Client rva 0xA51F0: register-arg predicate (EAX = a, EDI = b). True when a's owner record matches b's id and either of
// a's two class lookups (import thunks 0xC5BC / 0xC5CE, each resolved through 0xC42A and the 0x23C-stride table at 0x15B0)
// has its +7 flag byte set in byte 4 of the imported bit-mask table. Callees in retail order.
#include <cstdint>
#include "d2client_callee.h"

static bool a51f0_hit(const D2cSvc& v, uint32_t thunk, uint32_t a, uint32_t line, const uint8_t* bitMasks) {
    const uint32_t id = d2c_call(v, thunk, 0, 0, 0, 0, 0, a);
    if (!id) return false;
    const uint32_t q = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, id, 0xB2000000u | 0xd4144, line);   // CUnit.cpp
    const uint32_t x = d2c_call(v, 0x15b0, q, 0, 0, 0, 0);
    return x && (bitMasks[4] & ((const uint8_t*)(uintptr_t)x)[7]);
}

// 1.13c D2Client+0xA51F0. bitMasks = the table the import pointer D2Client+0xCEE50 points at.
extern "C" uint32_t d2_client_a51f0(const D2cSvc& v, uint32_t a, const uint32_t* ap, const uint32_t* bp, const uint8_t* bitMasks) {
    if (!ap || !bp) return 0;
    if (ap[0] != 0 && bp[0] != 1) return 0;
    const bool h1 = a51f0_hit(v, 0xc5bc, a, 0xb4a, bitMasks);
    const bool h2 = a51f0_hit(v, 0xc5ce, a, 0xb55, bitMasks);
    if (!h1 && !h2) return 0;
    return d2c_call(v, 0x216a0, 0, bp[3], 0, 0, 0) == ap[3];
}
