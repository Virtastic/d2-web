// 1.13c D2Launch+0x9BF0  sub_9BF0: game-list selection paint. Selection 0x24DB0 resets to 0 when the count 0x25ED0 is 0; the row
// i = sel - first(0x25ED4) is highlighted through 9A1E(list 0x25AB0, colour 0x25 even / 0x135 odd, caption 0x25710[i]) when 0 <= i < rows(0x24DBC),
// then 987A(list, highlighted). The chain at 0x25EC8 (next +0x34C) is walked sel steps. No node: three 9862(.., 0) and 987A(list, 0);
// a node: 9862(0x25998, 1), 9862(0x25998, 0) when flag bit 0x20 at +0x324, 9862(0x25AAC, 0x25EC4 == 0), 9862(0x25AAC, 0) in mode 1 with
// 0x25EE4 set, then 996A and 98CE(EDX = node, 0) on 0x25878. Returns 1. g is the data range from 0x24DB0.
#include <cstdint>
#include "d2launch_env.h"
extern "C" uint32_t d2l_9bf0(const D2lSeams* s, uint32_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return g[(rva - 0x24DB0u) / 4]; };
    uint32_t n = W(0x25EC8);
    if (W(0x25ED0) == 0) W(0x24DB0) = 0;
    const int32_t i = (int32_t)(W(0x24DB0) - W(0x25ED4));
    uint32_t hi = 0;
    if (i >= 0 && i < (int32_t)W(0x24DBC)) {
        s->t9a1e(W(0x25AB0), (i & 1) ? 0x135u : 0x25u, W(0x25710 + 4u * (uint32_t)i));
        hi = 1;
    }
    s->t987a(W(0x25AB0), hi);
    for (uint32_t k = 0; n != 0 && W(0x24DB0) != k; ++k) n = *(const uint32_t*)(uintptr_t)(n + 0x34C);
    s->t9862(W(0x25708), 1);
    s->t9862(W(0x25980), 1);
    if (n == 0) {
        s->t9862(W(0x25998), 0); s->t9862(W(0x25708), 0); s->t9862(W(0x25980), 0);
        s->t987a(W(0x25AB0), 0);
        return 1;
    }
    s->t9862(W(0x25998), 1);
    if (*(const uint8_t*)(uintptr_t)(n + 0x324) & 0x20) s->t9862(W(0x25998), 0);
    s->t9862(W(0x25AAC), W(0x25EC4) == 0 ? 1u : 0u);
    if (W(0x25AD4) == 1 && W(0x25EE4) != 0) s->t9862(W(0x25AAC), 0);
    s->t996a(W(0x25878));
    s->t98ce(W(0x25878), n, 0);
    return 1;
}
