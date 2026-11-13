// 1.13c D2Game+0xC23C0 -- quest 0xD kill tally: counts the dying monster class down and opens the next stage when its group is gone
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc23c0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    const uint32_t t = e.imp(0xA532, e.rd32(a + 8));
    const auto dec = [&](uint32_t off) { e.wr32(data + off, e.rd32(data + off) - 1); };
    if (t == 0x20747368) dec(0x1c);
    else if (t == 0x2066736d) dec(0x18);
    else if (t == 0x20706976) dec(0x24);
    else if (t == 0x20786f62) dec(0x20);
    uint32_t eax = NE::lo(t, e.rd8(data + 0x2b));
    if (e.rd8(data + 0x2b)) return eax;
    eax = e.rd32(data + 0x1c);
    if (eax) return eax;
    bool go = false;
    eax = e.rd8(data + 0x29);
    if (eax) { eax = e.rd32(data + 0x20); go = !eax; }
    if (!go) {
        eax = e.rd32(data + 0x24);
        if (!eax) { eax = e.h(0xC2120, q); go = (eax & 0xff) != 0; }
        if (!go) {
            eax = NE::lo(eax, e.rd8(data + 0x28));
            if (!e.rd8(data + 0x28)) return eax;
            eax = e.rd32(data + 0x18);
            if (eax) return eax;
        }
    }
    eax = e.h(0xC1990, q, 0, 0, 0, 0, 0);
    e.wr8(data + 0x2b, 1);
    return eax;
}
}
