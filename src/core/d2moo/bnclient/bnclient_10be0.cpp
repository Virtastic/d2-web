// BNClient 0x10BE0: the BNCS message 0x52 (SID_AUTH_ACCOUNTCREATE) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10BE0 (150 B): handler(ECX = payload, EDX = size).
// A payload under 4 bytes is refused (0). The status dword maps to the create-account result stored in 0x1F050: 0 to 0, 7 to 1, 8 to 2, 9 to 3, 10 to 6, 11 to 7, 12 to 8, everything else (1..6 and above 12) to 4. The answer slot of message 0x52 (0x1F020) is set to 1. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid52_10be0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (size < 4) return 0;
    static const uint8_t kMap[13] = { 0, 4, 4, 4, 4, 4, 4, 1, 2, 3, 6, 7, 8 };
    const uint32_t status = bnSidU32(payload, 0);
    g_bn_6ff3f050 = status <= 0xC ? kMap[status] : 4;
    g_bn_6ff3eed8[0x52] = 1;
    return 1;
}
