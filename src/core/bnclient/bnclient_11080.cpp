// BNClient 0x11080: the BNCS message 0x56 (SID_AUTH_ACCOUNTCHANGEPROOF) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x11080 (179 B): handler(ECX = payload, EDX = size).
// A payload under 0x18 bytes is refused (0). The first 0x20 bytes of the account name (0x1ECB8) are cleared whatever the status. Status 0: the server proof at payload+4 is checked by 0x11BB0; a match marks the answer slot of message 0x55 (0x1F02C) and returns 1, a mismatch raises error 0x12 through 0xFCF0 and returns 0. Status 2 marks the slot, stores error 0xE if none is pending and returns 1. Any other status stores error 0x12 if none is pending and returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid56_11080(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (size < 0x18) return 0;
    const uint32_t status = bnSidU32(payload, 0);
    memset(g_bn_6ff3ecb8, 0, 0x20);
    if (status == 0) {
        if (bnSidCall(env, 0x11BB0, {}, { (uintptr_t)(payload + 4) }) != 0) { g_bn_6ff3eed8[0x55] = 1; return 1; }
        bnSidCall(env, 0xFCF0, {}, { 0x12 });
        return 0;
    }
    if (status == 2) { g_bn_6ff3eed8[0x55] = 1; bnSidFail(0xE); return 1; }
    bnSidFail(0x12);
    return 0;
}
