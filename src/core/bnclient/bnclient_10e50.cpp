// BNClient 0x10E50: the BNCS message 0x4C (SID_REQUIREDWORK) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10E50 (29 B): handler(ECX = payload, EDX = size).
// When the payload is present, its first byte is nonzero and the size is nonzero, starts the download of the file it names through 0xD5C0 with kind 0x80000006. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid4c_10e50(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (payload && payload[0] != 0 && size != 0) bnSidCall(env, 0xD5C0, { 0, (uintptr_t)payload, 0x80000006 });
    return 1;
}
