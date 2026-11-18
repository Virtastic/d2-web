// BNClient 0x108D0: the BNCS message 0x04 (SID_SERVERLIST) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x108D0 (27 B): handler(ECX = payload, EDX = size).
// Stores the string that starts at payload+4 under the registry value "Server List" of key "Configuration" (Storm registry string save, thunk 0x808A, flags 2) and returns 1. The first dword and the size are not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid04_108d0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    bnSidCall(env, 0x808A, {}, { (uintptr_t)kBnSidConfiguration, (uintptr_t)kBnSidServerList, 2, (uintptr_t)(payload + 4) });
    return 1;
}
