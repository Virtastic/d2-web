// src/core/d2win/d2win_sub_f410.cpp -- 1.13c D2Win.dll control release/click dispatch, from the cached
// Ghidra decompile (build-wasm/ghidra-d2win/decomp/0xF410.c).
#include <cstdint>
#include <emscripten.h>

extern "C" [[noreturn]] void d2_mcp_startup_overflow(uint32_t message);

// env seam: DAT_6f9014a8 (the in-message-pump depth counter)
extern "C" int32_t d2win_f410_pumpDepth();

// 1.13c D2Win+0x0F410
// ctrl[0] -> type-6 control c (else fatal 0x232). Sets flag bit1 of c[2]. If bit2 and bit0 of c[2] are
// both set and (byte c[0x10] & 3)==0: clear c[0x11]; if c[0x12] and callback c[0xd] are set, require
// pump depth >=1 (else fatal 0x243) and call the callback(ctrl) for the return value; clear c[0x12].
// Then clear bit1 and return. If the bit2&bit0 test fails: clear bit1, return 0.
// gdwBitMasks[i] = 1<<i, gdwInvBitMasks[i] = ~(1<<i) (Fog tables).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_F410(int32_t* ctrl) {
    int32_t* c = (int32_t*)(intptr_t)*ctrl;  // 32-bit retail-layout link; ponytail: wasm64 addresses must fit 32 bits
    if (c[0] != 6) d2_mcp_startup_overflow(0x232);
    uint32_t f = (uint32_t)c[2] | 2u;
    c[2] = (int32_t)f;
    uint32_t ret = 0;
    if ((f & 4u) && (f & 1u)) {
        if ((((uint8_t*)c)[0x40] & 3) == 0) {
            c[0x11] = 0;
            if (c[0x12] && c[0xd]) {
                if (d2win_f410_pumpDepth() < 1) d2_mcp_startup_overflow(0x243);
                ret = ((uint32_t(*)(int32_t*))(intptr_t)c[0xd])(ctrl);
            }
            c[0x12] = 0;
        }
        c[2] = (int32_t)((uint32_t)c[2] & ~2u);
        return ret;
    }
    c[2] = (int32_t)(f & ~2u);
    return 0;
}
