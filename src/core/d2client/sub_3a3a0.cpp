// D2Client+0x3A3A0: clears 8 state dwords, scans the 10-byte-stride table at 0x6FBCA2E0..0x6FBCA754,
// tail-calls the first matching case (0..4) handler. Env-seam style; no float.
#include <cstdint>
#include <emscripten.h>
struct D2c3A3A0Env {
    int32_t* g[8];                       // dwords at 6FBCA75C,6FBCA760,6FBC9E60,6FBCA764,6FBCA76C,6FBCA754,6FBCA768,6FBCA758 (in clear order)
    const uint8_t* table;                // bytes at 6FBCA2E0, (0x6FBCA754-0x6FBCA2E0)=0x474 bytes
    void (*handler[5])();                // jump-table targets for case 0..4 (via 6FAEA624+...)
};
extern "C" {
// 1.13c D2Client+0x3A3A0
EMSCRIPTEN_KEEPALIVE void d2c_3a3a0(const D2c3A3A0Env* e) {
    for (int i = 0; i < 8; i++) *e->g[i] = 0;
    for (const uint8_t* p = e->table; p < e->table + 0x474; p += 10) {
        int32_t id = *(const int32_t*)p;
        uint16_t w = *(const uint16_t*)(p + 4);
        if (w > 0xdf && id < 0x39) {
            uint32_t c = (uint32_t)w - 0x100;
            if (c < 5) { e->handler[c](); return; }
        }
    }
}
}
