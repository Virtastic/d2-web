// BNClient 0x108B0: the BNCS message 0x09 (SID_GETADVLISTEX) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x108B0 (14 B): handler(ECX = payload, EDX = size).
// Hands the payload (stack) and its size (EAX) to 0xB5F0 (the game-list answer parser) and returns 1; the callee's result is dropped.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid09_108b0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    bnSidCall(env, 0xB5F0, { size }, { (uintptr_t)payload });
    return 1;
}
