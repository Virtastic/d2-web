// 1.13c D2Launch+0xAE60  sub_AE60: frees the game-list chain at 0x25EC8 (node handle +0x338, next +0x34C read before the free; 9A24(handle, 1)
// then the Fog free 97FC(node, file 0x5EB84, line 0x52D, 0)), clears 0x25EC8/ED0/ED4/EE4, then the detail node at 0x25ECC: its handle is
// released only when +0x334 is 0, the node is freed (line 0x2A1, ECX re-read from the global) and the global cleared.
// g is the module data range from 0x24DB0.
#include <cstdint>
#include "d2launch_env.h"
extern "C" void d2l_ae60(const D2lSeams* s, uint32_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return g[(rva - 0x24DB0u) / 4]; };
    auto U32 = [](uint32_t p, uint32_t o) -> uint32_t { return *(const uint32_t*)(uintptr_t)(p + o); };
    for (uint32_t n = W(0x25EC8); n != 0;) {
        const uint32_t h = U32(n, 0x338), nx = U32(n, 0x34C);
        s->t9a24(h, 1);
        s->t97fc(n, 0x6FA5EB84u, 0x52D, 0);
        n = nx;
    }
    W(0x25EC8) = 0; W(0x25ED0) = 0; W(0x25ED4) = 0; W(0x25EE4) = 0;
    const uint32_t c = W(0x25ECC);
    if (c == 0) return;
    if (U32(c, 0x334) == 0) s->t9a24(U32(c, 0x338), 1);
    s->t97fc(W(0x25ECC), 0x6FA5EB84u, 0x2A1, 0);
    W(0x25ECC) = 0;
}
