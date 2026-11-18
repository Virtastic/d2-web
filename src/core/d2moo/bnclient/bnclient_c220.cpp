// BNClient 0xC220: the bare BNCS packet FF 0x02, length 4 (header only), built in a 0x2004-byte frame.
// If the connection handle (DAT+0x3F654) is set and the busy word (DAT+0x3F660) is clear, 0x807E copies the
// header to the second half of the frame and 0x80FC sends it (ECX = handle, EDX = copy, 4 bytes); otherwise the
// pending word (DAT+0x3F644) is set to 1. Always returns 1.
#include <cstdint>
#include <emscripten.h>

typedef void (*BnBareCopyFn)(uint8_t* dst, const uint8_t* src, uint32_t n);                    // 1.13c BNClient+0x807E
typedef void (*BnBareQueueFn)(uint32_t handle, const uint8_t* packet, uint32_t n);             // 1.13c BNClient+0x80FC

// 1.13c BNClient+0x0C220
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_bare_c220(const uint32_t* handle, const uint32_t* busy,
                                                                uint32_t* pending, BnBareCopyFn copy, BnBareQueueFn queue) {
    uint8_t frame[8] = { 0xFF, 0x02, 4, 0 };
    if (*handle != 0 && *busy == 0) {
        copy(frame + 4, frame, 4);
        queue(*handle, frame + 4, 4);
        return 1;
    }
    *pending = 1;
    return 1;
}
