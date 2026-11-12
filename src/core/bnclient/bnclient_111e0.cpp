// BNClient 0x111E0: the BNCS message 0x55 (SID_AUTH_ACCOUNTCHANGE) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x111E0 (62 B): handler(ECX = payload, EDX = size).
// A payload under 0x44 bytes is refused (0). Status 0 ends in 0x11F80 with the salt (payload+4, ECX) and the server key (payload+0x24, EAX) and returns its result; status 1 raises error 0xD through 0xFCF0 and returns 1; status 5 asks for the account upgrade through 0x11BF0(0) and returns its result; any other status raises error 0x12 and returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid55_111e0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (size < 0x44) return 0;
    switch (bnSidU32(payload, 0)) {
    case 0: return bnSidCall(env, 0x11F80, { (uintptr_t)(payload + 0x24), (uintptr_t)(payload + 4) });
    case 1: bnSidCall(env, 0xFCF0, {}, { 0xD }); return 1;
    case 5: return bnSidCall(env, 0x11BF0, {}, { 0 });
    default: bnSidCall(env, 0xFCF0, {}, { 0x12 }); return 0;
    }
}
