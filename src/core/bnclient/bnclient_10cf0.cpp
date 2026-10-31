// BNClient 0x10CF0: the BNCS message 0x25 (SID_PING) entry of the receive dispatch table (0x1E128 + 0x25*4).
#include <cstdint>
#include <emscripten.h>

// 0x0EB00 frame sender: FF, id, u16 size+4, then `size` payload bytes (ECX = size, EDI = payload, id on
// the stack). Returns the socket send's result; the handler ignores it.
typedef int32_t (*Bn10cf0SendFrameFn)(uint32_t size, uint8_t id, const void* payload);

// 1.13c BNClient+0x10CF0 (19 B): handler(ECX = payload, EDX = size). Echoes the whole received payload
// back to the server as message 0x25, unchanged and with the received length, then returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid_ping_10cf0(const uint8_t* payload, uint32_t size,
                                                                 Bn10cf0SendFrameFn send_frame) {
    send_frame(size, 0x25, payload);
    return 1;
}
