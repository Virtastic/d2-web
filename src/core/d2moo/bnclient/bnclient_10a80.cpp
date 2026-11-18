// BNClient 0x10A80: the BNCS message 0x00 (SID_NULL) entry of the receive dispatch table (0x1E128 + 0*4).
#include <cstdint>
#include <emscripten.h>

// 1.13c BNClient+0x10A80 (6 B): handler(ECX = payload, EDX = size). Keep-alive with no payload
// semantics: nothing is read, nothing is sent, the dispatcher is told the message was handled (1).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid_null_10a80(const uint8_t* payload, uint32_t size) {
    (void)payload;
    (void)size;
    return 1;
}
