// BNClient 0x116E0: the BNCS message 0x33 (SID_GETFILETIME) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x116E0 (59 B): handler(ECX = payload, EDX = size).
// Starts the download of the file named at payload+16 through 0xE120(id, flags, 0, 0, name, 0), where id is the first dword and flags is 3 for the ids 0x80000005 and 0x80000006 and 5 for every other id. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid33_116e0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    const uint32_t id = bnSidU32(payload, 0);
    const uint32_t flags = (id == 0x80000005u || id == 0x80000006u) ? 3 : 5;
    bnSidCall(env, 0xE120, {}, { id, flags, 0, 0, (uintptr_t)(payload + 16), 0 });
    return 1;
}
