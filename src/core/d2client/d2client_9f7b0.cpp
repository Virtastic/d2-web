// Panel button press. Retail: stdcall (x, y), ret 8. Clears the flags (+0x14, +0x15 from the slot base 0xDEC72, stride 0x28, count [0xDECB0]) of every slot,
// then each slot whose box {left +0, right +4, top +8, bottom +0xC} contains (x, y) (signed, inclusive) calls 0x88A70 with four zero words (stdcall) and
// sets both flags to 1. The count is re-read after each call.
// 1.13c D2Client+0x9F7B0
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cRow9f7b0Env {
    uint8_t* tbl;                                                         // D2Client+0xDEC70: slots at +2, count (int32) at +0x40
    void (*c88a70)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);       // 0x88A70
};
static int32_t rdi_9f7b0(const uint8_t* p) { int32_t v; std::memcpy(&v, p, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f7b0(int32_t x, int32_t y, const D2cRow9f7b0Env* e) {
    int32_t n = rdi_9f7b0(e->tbl + 0x40);
    for (int32_t i = 0; i < n; ++i) { e->tbl[0x16 + 0x28 * i + 1] = 0; e->tbl[0x16 + 0x28 * i] = 0; }
    for (int32_t i = 0; i < n; ++i) {
        const uint8_t* s = e->tbl + 2 + 0x28 * i;
        if (x < rdi_9f7b0(s) || x > rdi_9f7b0(s + 4) || y < rdi_9f7b0(s + 8) || y > rdi_9f7b0(s + 0xC)) continue;
        e->c88a70(0, 0, 0, 0);
        n = rdi_9f7b0(e->tbl + 0x40);
        e->tbl[0x16 + 0x28 * i + 1] = 1; e->tbl[0x16 + 0x28 * i] = 1;
    }
}
