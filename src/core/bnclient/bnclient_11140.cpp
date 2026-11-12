// BNClient 0x11140: the BNCS message 0x54 (SID_AUTH_ACCOUNTLOGONPROOF) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x11140 (156 B): handler(ECX = payload, EDX = size).
// A payload under 0x18 bytes is refused (0). Status 0: the server proof at payload+4 is checked by 0x11BB0; a match stores 0 in 0x1E120 and returns 1, a mismatch raises error 0x12 through 0xFCF0 and returns 0. Status 2 stores error 0xE if none is pending, stores 2 in 0x1E120 and returns 1. Any other status stores error 0x12 if none is pending and returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid54_11140(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (size < 0x18) return 0;
    const uint32_t status = bnSidU32(payload, 0);
    if (status == 0) {
        if (bnSidCall(env, 0x11BB0, {}, { (uintptr_t)(payload + 4) }) != 0) { g_bn_6ff3e120 = 0; return 1; }
        bnSidCall(env, 0xFCF0, {}, { 0x12 });
        return 0;
    }
    if (status == 2) { bnSidFail(0xE); g_bn_6ff3e120 = 2; return 1; }
    bnSidFail(0x12);
    return 0;
}
