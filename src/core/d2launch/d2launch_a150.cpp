// 1.13c D2Launch+0xA150  sub_A150: releases the detail node at 0x25ECC (handle only when +0x334 is 0; Fog free line 0x2A1), then resets the
// game-list page controls: 996A on 0x25878, 0x25704, 0x25988; 9862(.., 0) on 0x25708, 0x25980, 0x2570C, 0x25730, 0x2587C; 987A(0x25AB0, 0).
// 980E nonzero: 9862(0x25998, 0) and a tail jump to 9862(0x25AAC, 0); zero: a tail jump to 987A(0x25990, 0). g is the data range from 0x24DB0.
#include <cstdint>
#include "d2launch_env.h"
extern "C" void d2l_a150(const D2lSeams* s, uint32_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return g[(rva - 0x24DB0u) / 4]; };
    auto U32 = [](uint32_t p, uint32_t o) -> uint32_t { return *(const uint32_t*)(uintptr_t)(p + o); };
    const uint32_t c = W(0x25ECC);
    if (c != 0) {
        if (U32(c, 0x334) == 0) s->t9a24(U32(c, 0x338), 1);
        s->t97fc(W(0x25ECC), 0x6FA5EB84u, 0x2A1, 0);
        W(0x25ECC) = 0;
    }
    s->t996a(W(0x25878)); s->t996a(W(0x25704)); s->t996a(W(0x25988));
    s->t9862(W(0x25708), 0); s->t9862(W(0x25980), 0); s->t9862(W(0x2570C), 0); s->t9862(W(0x25730), 0); s->t9862(W(0x2587C), 0);
    s->t987a(W(0x25AB0), 0);
    if (s->t980e() != 0) { s->t9862(W(0x25998), 0); s->t9862(W(0x25AAC), 0); }
    else s->t987a(W(0x25990), 0);
}
