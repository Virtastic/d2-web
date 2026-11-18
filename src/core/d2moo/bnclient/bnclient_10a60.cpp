// BNClient 0x10A60: the BNCS message 0x1C (SID_STARTADVEX3) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10A60 (16 B): handler(ECX = payload, EDX = size).
// Marks the answer slot of message 0x1C (0x1EF48) with 1 when the status dword is 0; a nonzero status leaves it alone. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid1c_10a60(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (bnSidU32(payload, 0) == 0) g_bn_6ff3eed8[0x1c] = 1;
    return 1;
}
