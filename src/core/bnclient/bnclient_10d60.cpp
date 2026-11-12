// BNClient 0x10D60: the BNCS message 0x3D (SID_CREATEACCOUNT2) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10D60 (18 B): handler(ECX = payload, EDX = size).
// Stores the status dword in 0x1F050 and marks the answer slot of message 0x3D (0x1EFCC) with 1. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid3d_10d60(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    g_bn_6ff3f050 = bnSidU32(payload, 0);
    g_bn_6ff3eed8[0x3d] = 1;
    return 1;
}
