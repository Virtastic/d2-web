// 1.13c D2Client+0xACD70 reassembleFragment: ESI = packet
// (+1 fragment length byte, +2 restart flag, +3 total size dword, +7 bytes). Appends the fragment to the buffer
// held in globals 0x11BBCC (pointer) / 0x11BBD0 (filled length); when the length reaches the total the buffer is
// handed to 0xAC530 and freed. Allocation and free go through the Fog thunks 0xBF7A / 0xBF86 (ECX = size or block,
// EDX = file, stack line, 0; ret 8).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cAcd70Env {
    uintptr_t (*alloc)(uint32_t size, uint32_t line);   // thunk 0xBF7A
    void (*free_)(uintptr_t blk, uint32_t line);        // thunk 0xBF86
    void (*flush)(const uint8_t* pk);                   // 0xAC530 (ESI = packet still live)
    uintptr_t* buf;                                     // 0x11BBCC
    uint32_t* len;                                      // 0x11BBD0
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_acd70(const D2cAcd70Env* e, const uint8_t* pk) {
    uint32_t total; memcpy(&total, pk + 3, 4);
    uintptr_t p = *e->buf;
    if (p == 0 || pk[2] != 0) {
        if (p != 0) { e->free_(p, 0x5b); *e->buf = 0; *e->len = 0; }
        p = e->alloc(total, 0x62);
        *e->buf = p;
    }
    uint32_t n = pk[1];
    uint8_t* d = (uint8_t*)(p + *e->len);
    for (uint32_t i = 0; i < n; ++i) d[i] = pk[7 + i];
    *e->len += n;
    if (total == *e->len) {
        e->flush(pk);
        e->free_(*e->buf, 0x71);
        *e->buf = 0; *e->len = 0;
    }
}
