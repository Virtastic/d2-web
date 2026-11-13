// 1.13c D2Game+0xE71D0 -- quest event handler: the 0x4b talk step opens state 2 when the quest flags are clear, the 0x62/0x64/0x66 steps advance q[0xb] and run the state callbacks
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe71d0(NE& e, uint32_t z, uint32_t q, uint32_t a)
{
    uint32_t eax = z;
    const uint32_t game = e.rd32(a);
    const uint32_t w18 = e.rd32(a + 0x18);
    const auto st = [&]() { eax = e.h(0x338D0, game, 0, 0, 0, 0, 0x11, 0xd); eax = e.h(0x35EB0, eax == 1 ? 4 : 5); };
    const auto cbk = [&](uint32_t rva) { eax = e.cb(e.rd32(q + 4), rva, 0, 0); };
    if (e.rd32(a + 0x14) == 0x4b) {
        if (NE::rd16(q + 0x9c)) { const uint32_t u = e.rd32(a + 0xc); eax = e.h(0x33830, u ? e.rd32(u + 0xc) : ~0u, q + 0x1c); }
        eax = NE::lo(eax, e.rd8(q + 0xc));
        const uint32_t c = e.rd8(q + 0xc);
        if (c > 1 && c <= 3) {
            const uint32_t buf = e.qbuf(game, e.rd32(a + 0xc));
            eax = e.fl(buf, e.rd32(q + 0xe0), 0); if (eax == 1) return eax;
            eax = e.fl(buf, e.rd32(q + 0xe0), 0xb); if (eax == 1) return eax;
            if (e.rd8(q + 0xb) < 3) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2); }
            st(); cbk(0xE3560);
        }
    }
    uint32_t ecx = 0;
    if ((int32_t)w18 >= 0x62 && e.rd8(q + 9) && !e.rd8(q + 0xc)) { e.wr8(q + 0xc, 1); e.wr32(q + 0xa8, 0xB2000000u | 0xE6910); }
    if (w18 == 0x64) {
        const uint32_t b = e.rd8(q + 0xb);
        eax = NE::lo(eax, b);
        if (b == 2 || b == 0) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3); cbk(0xE5BE0); ecx = 1; }
        if (e.rd8(q + 0xc) == 1) { st(); cbk(0xE3560); return eax; }
        if (!ecx) return eax;
        cbk(0xE3560); return eax;
    }
    if (w18 == 0x66) {
        const uint32_t b = e.rd8(q + 0xb);
        eax = NE::lo(eax, 4);
        if (b != 4) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 4); cbk(0xE5BE0); }
        eax = NE::lo(eax, e.rd8(q + 0xc));
        const uint32_t c = e.rd8(q + 0xc);
        if (c != 4 && c != 5) st();
        cbk(0xE3560);
    }
    return eax;
}
}
