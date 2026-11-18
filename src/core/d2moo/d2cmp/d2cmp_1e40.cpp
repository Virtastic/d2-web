// 1.13c D2CMP+0x1E40 (EDI = table; stdcall key, a2, a3): insert a node for key. Lets 0x2040 grow the
// table, asks the table's factory (vtable +4) for a node in bucket (key & mask), moves the node's link
// to the tail of the table-wide list at +4, stores the key in the node.
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" uint32_t d2cmp_2040(uint8_t* m, uint32_t t, uint32_t idx, const D2CmpGuestEnv* e);
// 1.13c D2CMP+0x1E40
extern "C" uint32_t d2cmp_1e40(uint8_t* m, uint32_t t, uint32_t key, uint32_t a2, uint32_t a3,
                               const D2CmpGuestEnv* e) {
    uint32_t mask = rd(m, t + 0x24);
    if (d2cmp_2040(m, t, key & mask, e) != 0) mask = rd(m, t + 0x24);
    uint32_t node = e->vnew(m, t, rd(m, t + 0x1c) + (key & mask) * 0xc, a2, a3);
    uint32_t link = node ? rd(m, t + 4) + node : t + 8;
    unlink(m, link);
    uint32_t p = rd(m, t + 8);
    wr(m, link, p);
    wr(m, link + 4, rd(m, p + 4));
    wr(m, p + 4, node);
    wr(m, t + 8, link);
    wr(m, node, key);
    return node;
}
