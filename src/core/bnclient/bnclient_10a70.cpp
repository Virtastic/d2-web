// BNClient 0x10A70: the BNCS message 0x28 (SID_LOGONCHALLENGE) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10A70 (13 B): handler(ECX = payload, EDX = size).
// Stores the first payload dword in 0x1F874 (the server token). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid28_10a70(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    g_bn_6ff3f874 = bnSidU32(payload, 0);
    return 1;
}
