// 1.13c D2Game+0xE41D0: ECX = game, EDX = unit, one unused stack word. Void.
#include "d2game_h16.h"
namespace h16 {
void e41d0(const Env& e, uint32_t game, uint32_t unit)
{
    uint32_t head = rd32(game, 0x10f4);
    if (!head) return;
    uint32_t n = rd32(head, 0);
    if (!n) return;
    while (rd32(n, 0) != 0x11) { n = rd32(n, 0xf4); if (!n) return; }
    uint32_t sk = rd32(e.o10920(unit), 0x10 + rd8(game, 0x6d) * 4);
    if (e.o10174(sk, 0x13, 0) == 0 && e.o10174(sk, 0x13, 6) == 0 && e.o10174(sk, 0x13, 8) == 0 && e.o10174(sk, 0x13, 7) == 0) {
        uint32_t h = e.o10331(unit);
        if (h) {
            uint32_t k = e.o10826(h);
            if ((e.o10459(k) & 0xFF) == 2) e.o10003(sk, 0x13, 6);
        }
    }
}
}
