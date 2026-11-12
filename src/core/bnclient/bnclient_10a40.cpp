// BNClient 0x10A40: the BNCS message 0x29 (SID_LOGONRESPONSE) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10A40 (20 B): handler(ECX = payload, EDX = size).
// Stores 1 in 0x1E120 when the result dword is 0, else 0. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid29_10a40(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    g_bn_6ff3e120 = bnSidU32(payload, 0) == 0;
    return 1;
}
