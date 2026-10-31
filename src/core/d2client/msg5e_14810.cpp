// 1.13c D2Client+0x14810 -- builds the 6-byte client message 0x5E and hands it to the sender 0x143E0.
#include "client_host.h"
#include <emscripten.h>

// Message bytes: [0] = 0x5E, [1] = b, [2..5] = a little-endian dword (the EDX register argument).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_msg5e_build(uint8_t* out6, uint32_t dword, uint8_t b) {
    out6[0] = 0x5E;
    out6[1] = b;
    out6[2] = (uint8_t)dword; out6[3] = (uint8_t)(dword >> 8); out6[4] = (uint8_t)(dword >> 16); out6[5] = (uint8_t)(dword >> 24);
}

// fastcall(ecx unused, edx = dword) + one stack byte b, ret 4. 0x143E0 gets EBX = 6 and one stack word, the buffer.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_14810(D2cHost h, uint32_t edx, uint8_t b, uint8_t* buf6, uint32_t bufTok) {
    d2_client_msg5e_build(buf6, edx, b);
    D2cCall c = { 0x143E0, 0, 0, 0, 6, 0, { bufTok, 0, 0, 0 } };
    h(&c);
}
