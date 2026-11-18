// BNClient 0x109A0: the BNCS message 0x3A (SID_LOGONRESPONSE2) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x109A0 (66 B): handler(ECX = payload, EDX = size).
// Stores the result dword in 0x1E120, then by result: 0 nothing; 1, 2, 3 raise error 0xD, 0xE, 0xF through 0xFCF0; 6 raises error 0x1B with the text at payload+4 through 0xFCA0; anything else stores error 0x12 in 0x1F1F4 if no error is pending. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid3a_109a0(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    const uint32_t result = bnSidU32(payload, 0);
    g_bn_6ff3e120 = result;
    switch (result) {
    case 0: break;
    case 1: bnSidCall(env, 0xFCF0, {}, { 0xD }); break;
    case 2: bnSidCall(env, 0xFCF0, {}, { 0xE }); break;
    case 3: bnSidCall(env, 0xFCF0, {}, { 0xF }); break;
    case 6: bnSidCall(env, 0xFCA0, {}, { 0x1B, (uintptr_t)(payload + 4) }); break;
    default: bnSidFail(0x12); break;
    }
    return 1;
}
