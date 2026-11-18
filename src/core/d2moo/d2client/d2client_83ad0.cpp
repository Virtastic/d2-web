// 1.13c D2Client+0x83ad0 -- item packet "place in inventory". EAX = packet (no stack arguments): byte +2 length, item id dword +4,
// payload from +8 decoded by 0xCA90 into a stack block (+4 word x, +6 word y, +8 byte, +0xC dword flags, +0x10 byte).
// player = *(D2Client+0x11BBFC) (null is fatal). The client item hash (D2Client+0x10AE08, 128 buckets, chain +0xE4, id +0xC, type +0)
// is walked for the id (a hit whose type is not 4 is fatal). A hit is updated in place (0x82820 with EAX = block byte 8, ESI = item,
// EDI = 0, three zero words, then 0xCA24 with the block byte +0x10 stored into the low byte of an uninitialised stack word); a miss makes
// the item with 0x835C0 (EBX = 0; packet, block, 0, 1). Then x/y (0xCA06/0xCA48), the inventory (player+0x60, null is fatal) placement
// test 0xCA6C(inv, item, 2) and insert 0xCA84(inv, item, 0x1630(item)), 0xCA30 -> 0x81FF0 (ESI = item, EDI = player, one zero word),
// flag 0x400 of the block dword -> 0xCA42(item, 0x400, 1), cursor clear 0xC7D8(inv, 0) and 0xC202(inv) into 0x11B834 with
// 0x11B858 = 0, 0x11B854 = 5, 0x11B864 = 4 or 1; the item's page (static-path types 2/4/5 read path+0xC, else 0xC3BE(path) or 0)
// below 4 sets byte 0x11C484[page]; WM_MOUSEMOVE to the 0xD0B4 window with lParam = word 0x11B824 << 16 | word 0x11B828 (IAT 0xCF118),
// 0x81AA0 (ESI = item, EDI = player) and 0x47AB0 when the hover state (0x11978E != 0, byte 0x119781 == 0) names the item id (0x119786).
#include "cl_env.h"
// hash/ui/page/hov: where retail's D2Client+0x10AE08, +0x11B820, +0x11C484 and +0x119780 live.
extern "C" void d2c_83ad0(uint32_t pkt, uint32_t hash, uint32_t ui, uint32_t page, uint32_t hov, const cl::Env& e) {
    struct { uint32_t hash, ui, page, hov; } const g{ hash, ui, page, hov };
    uint32_t buf[6] = {};
    const uint32_t bp = (uint32_t)(uintptr_t)buf;
    e.H(0xca90, pkt + 8, e.R8(pkt + 2) - 8, 0, bp);
    const uint32_t pl = e.self;
    if (!pl) return;                                           // fatal 0x8EF in retail
    const uint32_t id = e.R(pkt + 4);
    uint32_t node = 0;
    for (uint32_t n = e.R(g.hash + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) { if (e.R(n) != 4) return; node = n; break; }   // fatal 0x5A in retail
    const uint8_t* b = (const uint8_t*)buf;
    uint32_t it;
    if (node) {
        e.H(0x82820, 0, 0, 0, 0, 0, b[8], 0, 0, 0, node);
        e.Edj(0x82820, 0);
        e.H(0xca24, node, b[0x10]);                            // upper bytes: stack residue (masked in the vector)
        it = node;
    } else {
        it = e.H(0x835c0, pkt, bp, 0, 1);
    }
    e.H(0xca06, it, buf[1] & 0xffff);
    e.H(0xca48, it, buf[1] >> 16);
    const uint32_t inv = e.R(pl + 0x60);
    if (!inv) return;                                          // fatal 0x8FF in retail
    if (e.H(0xca6c, inv, it, 2)) {
        const uint32_t pg = e.H(0x1630, 0, 0, 0, 0, 0, 0, it);
        e.H(0xca84, inv, it, pg);
    }
    if (e.H(0xca30, it, pl)) { e.H(0x81ff0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it); e.Edj(0x81ff0, pl); }
    if (buf[3] & 0x400) e.H(0xca42, it, 0x400, 1);
    e.H(0xc7d8, inv, 0);
    const uint32_t cur = e.H(0xc202, inv);
    e.W(g.ui + 0x14, cur);                                     // 0x11B834
    e.W(g.ui + 0x38, 0);                                       // 0x11B858
    e.W(g.ui + 0x34, 5);                                       // 0x11B854
    e.W(g.ui + 0x44, cur ? 4 : 1);                             // 0x11B864
    const int32_t ty = (int32_t)e.R(it);
    uint32_t k;
    if (ty == 2 || (ty > 3 && ty <= 5)) k = e.R(e.R(it + 0x2c) + 0xc);
    else { const uint32_t p = e.R(it + 0x2c); k = p ? e.H(0xc3be, p) : 0; }
    if (k < 4) { const uint32_t a = g.page + k; e.W(a & ~3u, (e.R(a & ~3u) & ~(0xffu << 8 * (a & 3))) | (1u << 8 * (a & 3))); }
    const uint32_t lp = (e.R(g.ui + 4) & 0xffff) << 16 | (e.R(g.ui + 8) & 0xffff);   // 0x11B824, 0x11B828
    const uint32_t hw = e.H(0xd0b4);
    e.H(0xcf118, hw, 0x200, 0, lp);
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it);
    e.Edj(0x81aa0, pl);
    if (e.R(g.hov + 0xe) && !e.R8(g.hov + 1) && e.R(it + 0xc) == e.R(g.hov + 6)) e.H(0x47ab0);
}
