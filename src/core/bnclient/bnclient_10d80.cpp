// BNClient 0x10D80: the BNCS message 0x2A (SID_CREATEACCOUNT) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10D80 (13 B): handler(ECX = payload, EDX = size).
// Stores the first payload dword in the answer slot of message 0x2A (0x1EF80). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid2a_10d80(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    g_bn_6ff3eed8[0x2a] = bnSidU32(payload, 0);
    return 1;
}
