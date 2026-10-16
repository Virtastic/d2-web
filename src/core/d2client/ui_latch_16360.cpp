// 1.13c D2Client+0x16360: mouse-state latch. ESI=msg; globals passed in explicitly.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
struct Latch { uint32_t a824, a828; uint8_t pad[0x28]; uint32_t i854, i858, pad2, tick860, state864; };  // 0x11b824..
static_assert(offsetof(Latch, i854) == 0x30 && offsetof(Latch, state864) == 0x40, "layout");
extern "C" void d2_client_16360(uint8_t* msg, Latch* g, const uint8_t* tbl, uint32_t tick) {
    g->a828 = *(uint16_t*)(msg + 0xc);
    g->a824 = *(uint16_t*)(msg + 0xe);
    g->tick860 = tick;
    if (g->state864 == 5) { *(uint32_t*)(msg + 0x18) = 0; return; }
    uint32_t v = *(const uint32_t*)(tbl + g->i854 * 0x1c);
    *(uint32_t*)(msg + 0x18) = 0;
    if (v) { g->state864 = 5; g->i854 = 4; g->i858 = 0; }
}
