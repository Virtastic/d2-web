// 1.13c D2Client+0x49D80: builds the 0x6FBC9767 pop-up (title key = the stack argument, EAX = the slot index): one line per entry the 0x6FBC3B10
// object lists (D2Lang key looked up through the 0x6FB91400 word-pair table), the gold/box lines for the two states that want them, then the
// closing line 0xD48. Callees go through D2cHost in retail's call order. wpp = window 0x6FBC9679..0x6FBC976D, tbp = window 0x6FBA37BC.. (22-byte
// rows, byte 0 of the row is read), g3b10 = the dword at 0x6FBC3B10, tp = window 0x6FB91400..0x6FB91C40, plp = the dword at 0x6FBCBBFC (player unit, [+0x60] = its list).
#include <emscripten.h>
#include "d2client_hw.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2h_49d80(D2cHost h, uint32_t idx, uint32_t key, uint8_t* wpp, uint8_t* tbp, const uint32_t* g3b10, uint8_t* tp, uint8_t* plp) {
    Win w = { wpp, 0x6fbc9679 }, t = { tp, 0x6fb91400 }, pw = { plp, 0x6fbcbbfc };
    auto fatal = [&](uint32_t k) {   // Ordinal_10265(k), Ordinal_10024(fmt, str), _exit(-1): never returns in retail
        uint32_t s = hc(h, 0xbf92, 0, 0, 0, 0, 0, k);
        hc(h, 0xbf80, 0, 0, 0, 0, 0, 0x6fb8359f, s); hc(h, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    };
    auto line = [&](uint32_t colour, uint32_t k, uint32_t spacing) {   // D2Lang lookup, then the 0xA36F0 menu line (callback and selectable word unlogged)
        uint32_t text = hc(h, 0xd1ec, 0, k);
        hc(h, 0xa36f0, colour, w.rd(0x6fbc9767), 0, 0, 0, text, spacing, 0, 1);
    };
    if (w.rd(0x6fbc9767) != 0) { fatal(0x76f); return; }
    hc(h, 0x46880);
    uint32_t box = hc(h, 0xa3e40, 0, 0, 0, 0, 0, 0, 0, 0xB2000000u | 0x49250, 0xB2000000u | 0x48650);
    w.wr(0x6fbc9767, box);
    if (box == 0) { fatal(0x529); return; }
    st((void*)(uintptr_t)box, 0x5c, 1);
    line(4, key, 0x15);
    const uint32_t g = *g3b10;
    if (g != 0) {
        uint32_t cnt = hc(h, 0xce38, 0, 0, 0, 0, 0, g);
        w.wr(0x6fbc9679, idx);
        if (tbp[idx * 0x16] == 0) line(0, 0xd47, 0xf);
        line(0, 0xd43, 0xf);
        for (int32_t i = 0; i < (int32_t)cnt; ++i) {
            uint32_t ax = hc(h, 0xcdf6, 0, 0, 0, 0, 0, g, (uint32_t)i) & 0xffff;
            int32_t c, e;
            if ((uint16_t)t.rd(0x6fb91590) > ax) { c = 0; e = 0x64; } else { c = 0x64; e = (int32_t)t.rd(0x6fb91c3c); }
            uint32_t k = 0xe8c;
            for (; c < e; ++c) {
                if ((uint16_t)(t.rd(0x6fb91400 + (uint32_t)c * 4) & 0xffff) == ax) { k = (t.rd(0x6fb91400 + (uint32_t)c * 4) >> 16) | ((uint32_t)c & 0xffff0000u); break; }
            }
            line(0, k, 0xf);
        }
        uint32_t m = w.rd(0x6fbc9725);
        if (m == 0xc9) { line(0, 0xd40, 0xf); m = w.rd(0x6fbc9725); }
        if (m == 0x109 || m == 0xf4 || m == 0xf5 || m == 0x208 || m == 0xf6) {
            uint32_t pl = pw.rd(0x6fbcbbfc);
            uint32_t lst = pl ? ld((const void*)(uintptr_t)pl, 0x60) : 0;
            if (lst) {
                for (uint32_t it = hc(h, 0xc4f0, 0, 0, 0, 0, 0, lst); it; it = hc(h, 0xc568, 0, 0, 0, 0, 0, it)) {
                    uint32_t x = hc(h, 0xc526, 0, 0, 0, 0, 0, it);
                    if (x && hc(h, 0xc820, 0, 0, 0, 0, 0, x) == 0x20786f62) { line(0, 0x8b7, 0xf); break; }
                }
            }
        }
    }
    line(0, 0xd48, 0xf);
}
