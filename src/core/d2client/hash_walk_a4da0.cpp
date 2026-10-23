// D2Client+0xA4DA0: walk a 0x80-bucket table of dword chains (next link at +0xE4); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA4DA0Env {
    uint8_t* mem;                              // linear base for 32-bit retail addresses
    void (*cb)(uint32_t node);                 // callee held in EBX by the caller (convention unverified)
};
extern "C" {
// 1.13c D2Client+0xA4DA0
EMSCRIPTEN_KEEPALIVE void d2c_a4da0(const D2cA4DA0Env* e, uint32_t table) {
    for (uint32_t i = 0; i < 0x80; i++) {
        uint32_t n = *(const uint32_t*)(e->mem + table + i * 4);
        while (n) {
            uint32_t cur = n;
            n = *(const uint32_t*)(e->mem + n + 0xE4);  // next read before the call, as retail
            e->cb(cur);
        }
    }
}
}
