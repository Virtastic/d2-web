// BNClient 0x108A0: the BNCS message 0x20 (SID_ANNOUNCEMENT) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x108A0 (6 B): handler(ECX = payload, EDX = size).
// Returns 1; nothing is read or written.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid20_108a0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    return 1;
}
