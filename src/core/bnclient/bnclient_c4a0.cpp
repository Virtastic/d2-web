// BNClient 0xC4A0: the bare BNCS packet FF 0x10, length 4 (header only), built in a 0x2004-byte frame.
// If the connection handle (DAT+0x3F654) is set and the busy word (DAT+0x3F660) is clear, 0x807E copies the
// header and 0x80FC sends it (ECX = handle, EDX = copy, 4); a negative send result returns 0. Otherwise the
// pending word (DAT+0x3F644) is set to 1. Returns 1.
#include <cstdint>
#include <emscripten.h>

typedef void (*BnC4a0CopyFn)(uint8_t* dst, const uint8_t* src, uint32_t n);                 // 1.13c BNClient+0x807E
typedef int32_t (*BnC4a0QueueFn)(uint32_t handle, const uint8_t* packet, uint32_t n);       // 1.13c BNClient+0x80FC

// 1.13c BNClient+0x0C4A0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_bare_c4a0(const uint32_t* handle, const uint32_t* busy,
                                                                uint32_t* pending, BnC4a0CopyFn copy, BnC4a0QueueFn queue) {
    uint8_t frame[8] = { 0xFF, 0x10, 4, 0 };
    if (*handle != 0 && *busy == 0) {
        copy(frame + 4, frame, 4);
        if (queue(*handle, frame + 4, 4) < 0) return 0;
        return 1;
    }
    *pending = 1;
    return 1;
}
