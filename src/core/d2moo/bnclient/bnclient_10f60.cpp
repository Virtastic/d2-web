// BNClient 0x10F60: the BNCS message 0x1D (SID_LOGONCHALLENGEEX) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10F60 (45 B): handler(ECX = payload, EDX = size).
// Stores the second payload dword in 0x1F874 (the server token), then writes the log line "logon acknowledged" between two log-control calls (0x17480 with (2, 4) and (2, -4)). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid1d_10f60(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    g_bn_6ff3f874 = bnSidU32(payload, 1);
    bnSidCall(env, 0x17480, {}, { 2, 4 });
    bnSidCall(env, 0x17420, {}, { (uintptr_t)kBnSidLogonAck });
    bnSidCall(env, 0x17480, {}, { 2, (uintptr_t)-4 });
    return 1;
}
