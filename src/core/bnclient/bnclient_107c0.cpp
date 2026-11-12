// BNClient 0x107C0: the BNCS message 0x59 (SID_SETEMAIL) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x107C0 (11 B): handler(ECX = payload, EDX = size).
// Sets the e-mail-registration flag (0x1F668) to 1 and returns 1. Neither the payload nor the size is read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid59_107c0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    g_bn_6ff3f668 = 1;
    return 1;
}
