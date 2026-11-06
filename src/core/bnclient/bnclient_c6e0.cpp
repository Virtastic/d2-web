// BNClient 0xC6E0: clears DAT+0x3EE58 and DAT+0x3EFD8, then sends the bare BNCS packet FF 0x40, length 4, under
// the same gate as 0xC4A0 (handle DAT+0x3F654 set, busy DAT+0x3F660 clear; else pending DAT+0x3F644 = 1). A
// negative send result returns 0 at once; otherwise 0x10D90(0x40) runs and DAT+0x3F884 is returned.
#include <cstdint>
#include <emscripten.h>

typedef void (*BnC6e0CopyFn)(uint8_t* dst, const uint8_t* src, uint32_t n);                 // 1.13c BNClient+0x807E
typedef int32_t (*BnC6e0QueueFn)(uint32_t handle, const uint8_t* packet, uint32_t n);       // 1.13c BNClient+0x80FC
typedef void (*BnC6e0WaitFn)(uint32_t id);                                                   // 1.13c BNClient+0x10D90

struct BnC6e0Globals { uint32_t* handle; uint32_t* busy; uint32_t* pending; uint32_t* g3ee58; uint32_t* g3efd8; uint32_t* result; };

// 1.13c BNClient+0x0C6E0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_bare_c6e0(const BnC6e0Globals* g, BnC6e0CopyFn copy,
                                                                BnC6e0QueueFn queue, BnC6e0WaitFn wait) {
    *g->g3ee58 = 0;
    *g->g3efd8 = 0;
    uint8_t frame[8] = { 0xFF, 0x40, 4, 0 };
    if (*g->handle != 0 && *g->busy == 0) {
        copy(frame + 4, frame, 4);
        if (queue(*g->handle, frame + 4, 4) < 0) return 0;
    } else {
        *g->pending = 1;
    }
    wait(0x40);
    return *g->result;
}
