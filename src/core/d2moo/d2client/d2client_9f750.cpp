// Panel-state reset. Retail: no arguments, plain ret. Zeroes the words at 0x10B214/0x10B218/0x10B21C/0x10B228, loads the two handles 0x11BC0C/0x11BC10 from
// 0xBF6C0 (ESI = the module string at 0xD4570, then 0xD455C), then clears the two flag bytes (+0x14, +0x15 from the slot base 0xDEC72) of the
// [0xDECB0] button slots (stride 0x28).
// 1.13c D2Client+0x9F750
#include <cstdint>
#include <emscripten.h>

struct D2cRow9f750Env {
    uint32_t* st;                         // D2Client+0x10B214 (0x18 bytes): +0 state, +4, +8, +0x14
    uint32_t* h;                          // D2Client+0x11BC0C (2 words)
    uint8_t* tbl;                         // D2Client+0xDEC70: slots at +2 (stride 0x28), count (int32) at +0x40
    uint32_t (*cbf6c0)(uint32_t esi);     // 0xBF6C0, ESI = module string 0xD4570 / 0xD455C (as passed)
    uint32_t str0, str1;                  // the two ESI values
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f750(const D2cRow9f750Env* e) {
    e->st[0] = 0; e->st[2] = 0; e->st[1] = 0; e->st[5] = 0;
    e->h[0] = e->cbf6c0(e->str0);
    e->h[1] = e->cbf6c0(e->str1);
    const int32_t n = *(const int32_t*)(e->tbl + 0x40);
    for (int32_t i = 0; i < n; ++i) { e->tbl[0x16 + 0x28 * i + 1] = 0; e->tbl[0x16 + 0x28 * i] = 0; }
}
