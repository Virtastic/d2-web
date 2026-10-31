// 1.13c D2CMP+0x1740 (ECX = target link or 0, EAX = list, node, mode): move node's link (node itself,
// or list+4 when node is 0) next to target (list+4 when 0): mode 1 after it, otherwise before it.
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x1740
extern "C" void d2cmp_1740(uint8_t* m, uint32_t target, uint32_t list, uint32_t node, int32_t mode) {
    uint32_t link = node ? node : list + 4;
    unlink(m, link);
    if (target == 0) target = list + 4;
    if (mode != 1) {
        uint32_t p = rd(m, target);
        wr(m, link, p);
        wr(m, link + 4, rd(m, p + 4));
        wr(m, p + 4, node);
        wr(m, target, link);
        return;
    }
    wr(m, link, target);
    wr(m, link + 4, rd(m, target + 4));
    uint32_t n = rd(m, target + 4);
    int32_t off = (int32_t)rd(m, list);
    if ((int32_t)n < 0) { wr(m, ~n, link); wr(m, target + 4, node); return; }
    if (off < 0) off = (int32_t)(target - rd(m, rd(m, target) + 4));
    wr(m, n + (uint32_t)off, link);
    wr(m, target + 4, node);
}
