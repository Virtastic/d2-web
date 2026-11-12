// BNClient 0x11030: the BNCS message 0x57 (SID_AUTH_ACCOUNTUPGRADE) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x11030 (69 B): handler(ECX = payload, EDX = size).
// A payload under 8 bytes is refused (0). A nonzero status stores error 0x12 in 0x1F1F4 if none is pending and returns 0. Status 0 hands the second dword (the server token) to 0x11B00, which sends the proof message, and returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid57_11030(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (size < 8) return 0;
    if (bnSidU32(payload, 0) != 0) { bnSidFail(0x12); return 0; }
    bnSidCall(env, 0x11B00, {}, { bnSidU32(payload, 1) });
    return 1;
}
