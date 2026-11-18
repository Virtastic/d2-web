// BNClient 0x10E90: the BNCS message 0x58 (SID_AUTH_ACCOUNTUPGRADEPROOF) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10E90 (270 B): handler(ECX = payload, EDX = size).
// A payload under 4 bytes is refused (0). Status 0: while an upgrade is in progress (0x1EB50 nonzero) the handler ends in 0x11910 (the logon request) and returns its result; otherwise it clears the first 0x20 bytes of the account name (0x1ECB8), stores 0 in 0x1EED0, marks the answer slot of message 0x55 (0x1F02C) and returns 1. Status 2: clears the name, stores 2 in 0x1E120 (upgrade) or in 0x1EED0 with the slot marked (no upgrade), raises error 0xE through 0xFCF0, returns 1. Any other status stores error 0x12 in 0x1F1F4 if none is pending and returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid58_10e90(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (size < 4) return 0;
    const uint32_t status = bnSidU32(payload, 0);
    if (status == 0) {
        if (g_bn_6ff3eb50 != 0) return bnSidCall(env, 0x11910, {});
        memset(g_bn_6ff3ecb8, 0, 0x20);
        g_bn_6ff3eed0 = 0;
        g_bn_6ff3eed8[0x55] = 1;
        return 1;
    }
    if (status == 2) {
        memset(g_bn_6ff3ecb8, 0, 0x20);
        if (g_bn_6ff3eb50 != 0) g_bn_6ff3e120 = 2;
        else { g_bn_6ff3eed0 = 2; g_bn_6ff3eed8[0x55] = 1; }
        bnSidCall(env, 0xFCF0, {}, { 0xE });
        return 1;
    }
    bnSidFail(0x12);
    return 0;
}
