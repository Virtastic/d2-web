// 1.13c D2Client+0x59830 -- exit-countdown poll (no arguments, returns 0/1). Module words (RVA from base 0x6FAB0000):
// state 0x11C2CC, stamp 0x1177F4, 0xF8C9C, 0x11C404, 0x11C3A4. State 7 and a stamp other than -1 older than 30000 ms (unsigned
// compare, GetTickCount at call time): 0x8CA20(), state := 0, 0x59740(0) (stdcall, one word), 0x14660(), then
// 0xF8C9C := 0, 0x11C404 := 1, 0x11C3A4 := 1. Returns 1 for state 7, 0 otherwise.
#include <cstdint>
struct D2c59830State { uint32_t state, stamp, w8c9c, w404, w3a4; };
struct D2c59830Env { uint32_t (*tick)(); void (*c8ca20)(); void (*c59740)(uint32_t); void (*c14660)(); };
extern "C" uint32_t d2c_59830r(D2c59830State* s, const D2c59830Env& e) {
    if (s->state != 7) return 0;
    if (s->stamp != 0xFFFFFFFFu && e.tick() > s->stamp + 30000u) {
        e.c8ca20();
        s->state = 0;
        e.c59740(0);
        e.c14660();
        s->w8c9c = 0; s->w404 = 1; s->w3a4 = 1;
    }
    return 1;
}
