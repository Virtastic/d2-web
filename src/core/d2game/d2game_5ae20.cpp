// 1.13c D2Game+0x5AE20 -- player join: restores the reward state of the quest from the flags
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5ae20(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    uint32_t eax;
    if (!e.rd8(q + 9)) {
        e.wr8(q + 0xc, 5);
        const uint32_t pd = e.imp(0xA4C0, e.rd32(a + 0xc));
        const uint32_t list = e.rd32(pd + 0x1c + 4 * e.rd8(e.rd32(a) + 0x6d));
        uint32_t wp = 0;
        eax = e.imp(0xAB02, 0x7b, e.stk(&wp));
        if (!eax) return eax;
        eax = e.imp(0xAAD2, list, wp);
        if (!eax) e.wr8(data + 0x89, 1);
        return eax;
    }
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    if ((eax = e.fl(buf, fid, 1))) return eax;
    if (e.fl(buf, 0x26, 3)) { e.wr8(data + 0x87, 1); e.wr8(q + 0xb, 2); e.cb(e.rd32(q + 4), 0x590C0); return e.h(0x35EB0, 3); }
    if (!(eax = e.fl(buf, fid, 2))) return eax;
    e.wr8(data + 0x87, 1); e.wr8(q + 0xb, 1);
    e.cb(e.rd32(q + 4), 0x590C0);
    return e.h(0x35EB0, 2);
}
}
