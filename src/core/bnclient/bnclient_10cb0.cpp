// BNClient 0x10CB0: the BNCS message 0x3E (SID_LOGONREALMEX) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10CB0 (16 B): handler(ECX = payload, EDX = size).
// Hands the payload (stack) and its size (EBX) to 0xB920 (the realm-logon answer parser) and returns 1; the callee's result is dropped.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid3e_10cb0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    bnSidCall(env, 0xB920, { 0, 0, 0, size }, { (uintptr_t)payload });
    return 1;
}
