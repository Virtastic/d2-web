// 1.13c D2Client+0x9FE50: EDI = y, stack x (ret 4). Click against the two-slot button table at 0xDEC60 (0x28 bytes per slot, count at +0x50):
// a slot that is armed (+0x27) and holds the point (x in [+0x12,+0x16], y in [+0x1A,+0x1E]) runs 88A70 (EBX=1, four zero words) and then
// 9F830 (slot 0) or 9FD70; every armed flag (+0x26, +0x27) is cleared afterwards. Returns 1 when any slot took the click.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cA9fe50Env {
    void (*f88a70)();   // D2Client+0x88A70, EBX = 1, four zero words
    void (*f9f830)();   // D2Client+0x9F830
    void (*f9fd70)();   // D2Client+0x9FD70
};
extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2c_9fe50(const D2cA9fe50Env* e, uint8_t* tb, int32_t x, int32_t y) {
    auto i32 = [&](uint32_t o) { int32_t v; memcpy(&v, tb + o, 4); return v; };
    uint32_t ret = 0;
    for (int32_t i = 0; i < i32(0x50); ++i) {
        uint32_t b = (uint32_t)i * 0x28;
        if (tb[b + 0x27] != 0 && i32(b + 0x12) <= x && x <= i32(b + 0x16) && i32(b + 0x1a) <= y && y <= i32(b + 0x1e)) {
            e->f88a70();
            if (i == 0) e->f9f830(); else e->f9fd70();
            ret = 1;
        }
    }
    for (int32_t i = 0, n = i32(0x50); n > 0 && i < n; ++i) { tb[(uint32_t)i * 0x28 + 0x26] = 0; tb[(uint32_t)i * 0x28 + 0x27] = 0; }
    return ret;
}
}
