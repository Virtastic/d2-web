// 1.13c D2Game+0x5F5E0 -- message handler of the quest 0x25: start, the ice item, the scroll and the class reward
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
static const uint32_t rewardN[7] = { 5, 5, 5, 5, 5, 5, 7 };
static const uint32_t reward[3][7][7] = {
    { { 0x20316d61, 0x20326d61, 0x20336d61, 0x20346d61, 0x20356d61 },
      { 0x2031626f, 0x2032626f, 0x2033626f, 0x2034626f, 0x2035626f },
      { 0x2031656e, 0x2032656e, 0x2033656e, 0x2034656e, 0x2035656e },
      { 0x20316170, 0x20326170, 0x20336170, 0x20346170, 0x20356170 },
      { 0x20316162, 0x20326162, 0x20336162, 0x20346162, 0x20356162 },
      { 0x20317264, 0x20327264, 0x20337264, 0x20347264, 0x20357264 },
      { 0x2072746b, 0x20627277, 0x20667861, 0x20736563, 0x20776c63, 0x206c7462, 0x20726b73 } },
    { { 0x20366d61, 0x20376d61, 0x20386d61, 0x20396d61, 0x20616d61 },
      { 0x2036626f, 0x2037626f, 0x2038626f, 0x2039626f, 0x2061626f },
      { 0x2036656e, 0x2037656e, 0x2038656e, 0x2039656e, 0x2061656e },
      { 0x20366170, 0x20376170, 0x20386170, 0x20396170, 0x20616170 },
      { 0x20366162, 0x20376162, 0x20386162, 0x20396162, 0x20616162 },
      { 0x20367264, 0x20377264, 0x20387264, 0x20397264, 0x20617264 },
      { 0x20726139, 0x20627739, 0x20667839, 0x20736339, 0x20776c39, 0x20777439, 0x20727139 } },
    { { 0x20626d61, 0x20636d61, 0x20646d61, 0x20656d61, 0x20666d61 },
      { 0x2062626f, 0x2063626f, 0x2064626f, 0x2065626f, 0x2066626f },
      { 0x2062656e, 0x2063656e, 0x2064656e, 0x2065656e, 0x2066656e },
      { 0x20626170, 0x20636170, 0x20646170, 0x20656170, 0x20666170 },
      { 0x20626162, 0x20636162, 0x20646162, 0x20656162, 0x20666162 },
      { 0x20627264, 0x20637264, 0x20647264, 0x20657264, 0x20667264 },
      { 0x20726137, 0x20627737, 0x20667837, 0x20736337, 0x20776c37, 0x20777437, 0x20727137 } },
};
uint32_t f5f5e0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), m18 = NE::rd16(a + 0x18), m14 = NE::rd16(a + 0x14), game0 = e.rd32(a), game = e.rd32(q + 4);
    const uint32_t pd = e.imp(0xA4C0, e.rd32(a + 0xc)), buf = e.rd32(pd + 0x10 + 4 * e.rd8(game0 + 0x6d));
    uint32_t eax = m18;   // the upper half is an unset stack word in the original on the paths that fall through to the last test
    if (m14 == 0x201 && m18 == 0x4e94) {
        e.wr8(data + 0xac, 1); e.wr8(q + 0xc, 2);
        e.say(game, 2);
        e.cb(game, 0x58110);
        return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
    }
    if (m14 == 0x201 && m18 == 0x4e9f) {
        eax = e.h(0xE660, e.rd32(a + 0xc), 0, 0, game0, 0, 0x20656369);
        if (eax) return eax;
        eax = e.rd32(data + 0xd8);
        if (eax) return eax;
        eax = e.h(0x36470, 0, 0x20656369, e.rd32(a + 0xc), 0, 0, e.rd32(a), 2, 1);
        if (!eax) return eax;
        e.wr32(data + 0xd8, e.rd32(data + 0xd8) + 1);
        uint8_t pkt[8] = { 0x5d, 0x21, 1, 0, 0, 0, 0, 0 };
    const uint32_t pl = e.rd32(a + 0xc);
    const uint32_t cli = pl && !e.rd32(pl) ? e.rd32(e.imp(0xA4C0, pl) + 0x9c) : 0;
        eax = e.h(0x8ACD0, e.stk(pkt), 0, 0, 0, 0, cli);
        e.wr8(data + 0x101, 1);
        return eax;
    }
    if (m14 == 0x201 && m18 == 0x4ea4) {
        if (e.fl(buf, 0x25, 1) && !e.fl(buf, 0x25, 0)) {
            eax = e.h(0x36470, 0, 0x20327274, e.rd32(a + 0xc), 0, 0, e.rd32(a), 2, 1);
            if (!eax) return eax;
            e.setf(buf, 0x25, 8);
            uint8_t pkt[8] = { 0x5d, 0x21, 2, 0, 0, 0, 0, 0 };
    const uint32_t pl = e.rd32(a + 0xc);
    const uint32_t cli = pl && !e.rd32(pl) ? e.rd32(e.imp(0xA4C0, pl) + 0x9c) : 0;
            e.h(0x8ACD0, e.stk(pkt), 0, 0, 0, 0, cli);
            if (e.fl(buf, 0x25, 9)) { e.clrf(buf, 0x25, 1); e.setf(buf, 0x25, 0); }
            else if (e.rd8(q + 9) && e.rd8(q + 0xb) < 6) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 6); e.cb(game, 0x591D0); }
            if (e.rd32(data + 0x84) == 1) {
                e.wr8(data + 0xe3, 0);
                const uint32_t r = e.cb(game, 0x5DDF0, 1, 0);
                if (!e.rd8(data + 0xe3)) e.h(0x1EC40, NE::lo(r, 4), 0x20f, 0, game);
                e.wr8(data + 0xad, 1); e.wr32(data + 0x84, 2);
                e.h(0x5C690, 0, 0, 0, 0, 0, q, data);
            }
            return e.h(0x34C00, e.rd32(a + 0xc), 0, 0, 0, 0, e.rd32(a));
        }
        const uint32_t fid = e.rd32(q + 0xe0);
        eax = e.fl(buf, fid, 0);
        if (!eax) { eax = e.fl(buf, fid, 0xf); if (!eax) return eax; }
        eax = NE::lo(eax, e.rd8(data + 0x102));
        if (e.rd8(data + 0x102)) return eax;
        eax = e.fl(buf, 0x25, 8); if (!eax) return eax;
        eax = e.fl(buf, fid, 7); if (eax) return eax;
        eax = e.h(0xE660, e.rd32(a + 0xc), 0, 0, e.rd32(a), 0, 0x20327274); if (eax) return eax;
        eax = e.h(0x36470, 0, 0x20327274, e.rd32(a + 0xc), 0, 0, e.rd32(a), 2, 1); if (!eax) return eax;
        e.wr8(data + 0x102, 1);
        return eax;
    }
    if (m14 == 0x200 && m18 == 0x4ea8) {
        eax = e.fl(buf, 0x25, 1); if (!eax) return eax;
        eax = e.fl(buf, 0x25, 0); if (eax) return eax;
        const uint32_t lvl = e.imp(0xA3E2, e.rd32(a + 0xc), 0xc, 0);
        uint32_t t = 0;
        const uint32_t d = e.rd8(game0 + 0x6d);
        if (d == 1) { if (e.rd8(game0 + 0x6a) == 3 || lvl > 0x2d) t = 1; }
        else if (d == 2) { if (e.rd8(game0 + 0x6a) == 3 || lvl > 0x41) t = 2; }
        eax = e.fl(buf, 0x25, 0xa); if (eax) return eax;
        const uint32_t pl0 = e.rd32(a + 0xc), cls = pl0 ? e.rd32(pl0 + 4) : 0xffffffffu;
        const uint32_t seed = e.h(0x33D90, e.rd32(a));
        const uint32_t idx = e.h(0x1180, rewardN[cls], seed);
        const uint32_t code = reward[t][cls][idx];
        eax = e.h(0xE5D0, e.rd32(a + 0xc), 0, 0, 0, 0, 0);
        const uint32_t item = e.h(0x36470, eax, code, e.rd32(a + 0xc), 0, 0, e.rd32(a), 6, 1);
        if (!item) return item;
        uint8_t pkt[8] = { 0x5d, 0x21, 0x10, 0, 0, 0, 0, 0 };
    const uint32_t pl = e.rd32(a + 0xc);
    const uint32_t cli = pl && !e.rd32(pl) ? e.rd32(e.imp(0xA4C0, pl) + 0x9c) : 0;
        const uint32_t rec = e.imp(0xA53E, e.rd32(item + 4));
        if (rec && (int16_t)NE::rd16(rec + 0x124) > 0) NE::wr16(e.stk(pkt) + 4, NE::rd16(rec + 0x124));
        e.h(0x8ACD0, e.stk(pkt), 0, 0, 0, 0, cli);
        e.setf(buf, 0x25, 9);
        if (e.fl(buf, 0x25, 8)) { e.clrf(buf, 0x25, 1); e.setf(buf, 0x25, 0); }
        return e.setf(buf, 0x25, 0xa);
    }
    if (m18 != 0x4ea3) return eax;
    eax = NE::lo(eax, e.rd8(q + 9));
    if (!e.rd8(q + 9)) return eax;
    eax = NE::lo(eax, e.rd8(q + 0xc));
    if (e.rd8(q + 0xc) <= 3) { e.h(0x35EB0, 4); eax = e.h(0x5EC30, 0, 0, 0, 0, 0, q); }
    if (e.rd8(q + 0xb) >= 3) return eax;
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3);
    return e.cb(game, 0x591D0);
}
}
