// D2Client+0xA8110: fixed sequence of table walks (A4DA0) and A8030 calls over D2Client tables; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA8110Env {
    uint8_t* mem;                              // linear base for 32-bit retail addresses
    void (*walk)(uint32_t table);              // D2Client+0xA4DA0 (its EBX callback is bound by the host; unverified)
    void (*fn8030)(uint32_t table);            // D2Client+0xA8030 (unrecovered; convention unverified)
    uint32_t flag_6fbcc394;                    // DAT 6fbcc394 (RVA 0xCC394 relative to image base 0x6FB00000 -> 0xCC394)
};
extern "C" {
// 1.13c D2Client+0xA8110
EMSCRIPTEN_KEEPALIVE void d2c_a8110(const D2cA8110Env* e) {
    e->walk(0x6fbbac08);
    e->fn8030(0x6fbba008);
    e->fn8030(0x6fbb9e08);
    e->walk(0x6fbba608);
    e->walk(0x6fbba808);
    e->walk(0x6fbbaa08);
    e->walk(0x6fbbae08);
    e->fn8030(0x6fbb9c08);
    e->walk(0x6fbb9c08);
    e->walk(0x6fbbae08);
    e->walk(0x6fbba608);
    e->walk(0x6fbba808);
    e->walk(0x6fbb9c08);
    e->walk(0x6fbbae08);
    e->walk(0x6fbba608);
    if (e->flag_6fbcc394 == 0) {
        e->walk(0x6fbba608);
        e->walk(0x6fbba808);
    }
}
}
