// BNClient 0x10D10: the BNCS message 0x31 (SID_CHANGEPASSWORD) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10D10 (69 B): handler(ECX = payload, EDX = size).
// Stores the status dword in 0x1EED0 and marks the answer slot of message 0x31 (0x1EF9C) with 1; a status of 0 also stores error 0xE in 0x1F1F4 if no error is pending. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid31_10d10(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    const uint32_t status = bnSidU32(payload, 0);
    g_bn_6ff3eed0 = status;
    g_bn_6ff3eed8[0x31] = 1;
    if (status == 0) bnSidFail(0xE);
    return 1;
}
