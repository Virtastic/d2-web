// 1.13c D2Win+0x15A80 -- draw the text of an edit-box control (type 1): word-wrapped lines, the
// selection highlight rectangles, and the blinking "_" caret. Transcribed from the cached decompile;
// the x register of each 0x12940 call checked against the retail disassembly (x0, or x0 + width of
// the text before the caret). Retail globals and callees arrive through the env, preserving order.
// Control layout (dword index): 0 type(1), 2 flags, 3/4 x/y, 5/6 w/h, 0x10/0x11 padding,
// 0x15/0x16 selection (-1 none), 0x17 UTF-16 text, 0x97 caret, 0x9c font, 0x9d palette shift, 0xa0 flag.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2WinEditEnv {
    void (*fatal)(uint32_t id);                           // Ordinal_10265 + 10024 + _exit(-1)
    const uint32_t* bitMasks;                             // gdwBitMasks
    const int32_t* g14c4; const void* g14b0;              // 0x6F9014C4 / 0x6F9014B0 (focused ctl)
    void (*sub_15a10)(int32_t* ctl);                      // sends {ctl, 0x201} to 0x13DA0
    uint32_t (*tickCount)();
    void (*setFont)(int32_t font);                        // 0x12FE0
    const uint32_t* fontIndex; const uint8_t* const* fontTable;  // 0x6F8FFD34 / 0x6F901518 (stride 0x14)
    const uint8_t* (*glyph)(uint16_t ch);                 // 0x6F901508, width at +3
    uint8_t (*palIndex)(int32_t a, int32_t b, int32_t c, int32_t d); // Ordinal_10049(pal, 0x100,0x40,0x40,0x40)
    void (*fillArea)(int32_t l, int32_t t, int32_t r, int32_t b, int32_t color, int32_t gamma); // 10014
    void (*win2Unicode)(uint16_t* dst, const char* src, int32_t n);
    void (*unicode2Win)(char* dst, const uint16_t* src, int32_t n);
    int32_t (*widthN)(const uint16_t* s, int32_t n);      // 0x12080
    int32_t (*maxWidth)(const uint16_t* s);               // 0x11FB0
    void (*drawText)(int32_t x, const uint16_t* s, int32_t y, int32_t cw, int32_t ps, int32_t centered); // 0x12940
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_15a80(int32_t* c, const D2WinEditEnv* e) {
    if (!c) e->fatal(0x510);
    if (c[0] != 1) e->fatal(0x512);
    if (!(e->bitMasks[2] & c[2]) || !(e->bitMasks[0] & c[2])) return 1;
    if (!*e->g14c4) c[0xa0] = 0;
    if (c[0xa0]) e->sub_15a10(c);
    e->setFont(c[0x9c]);

    const int32_t fh = e->fontTable[*e->fontIndex][10];
    const int32_t x0 = c[3] + c[0x10];
    int32_t y = fh + c[4] - c[6] + c[0x11];
    const int32_t yStart = y;
    const int32_t color = e->palIndex(0x100, 0x40, 0x40, 0x40);
    uint16_t us[4] = {0}; const char under[2] = {'_', 0};
    e->win2Unicode(us, under, 2);
    const int32_t curW = e->widthN(us, 2);
    bool blink = false;
    if (e->g14b0 == c) blink = (e->tickCount() / 0xfa) & 1;
    bool drawn = false;

    char tw[256]; uint16_t wb[256]; char line[256];
    std::memset(wb, 0, sizeof wb);
    e->unicode2Win(tw, (const uint16_t*)(c + 0x17), 0x100);
    const int32_t availW = c[5] - c[0x10] * 2, availH = c[6] - c[0x11] * 2;
    char* p = tw; char* ls = tw;
    if (!tw[0]) {
        if (!blink) return 1;
        e->drawText(x0, us, y, 0, c[0x9d], 0);
        drawn = true;
        if (!tw[0]) return 1;
    }
    auto seg = [&](const char* s, int32_t n) {   // line[0..n) = s, widen into wb
        std::strncpy(line, s, n); line[n] = 0;
        e->win2Unicode(wb, line, 0x100);
    };
    for (;;) {
        if (availH <= y - yStart) return 1;
        std::memset(line, 0, sizeof line);
        char ch = *p; int32_t n = (int32_t)(p - ls); bool wrap = false;
        for (; ch && ch != '\r' && ch != '\n'; ch = *++p, ++n) {
            line[n] = ch;
            e->win2Unicode(wb, line, 0x100);
            int32_t w = 0;
            for (int32_t i = 0; wb[i] && i < n; ++i)
                if (wb[i] != 10) w += e->glyph(wb[i])[3];
            if (availW < w + curW) { wrap = true; break; }
        }
        if (c[0] != 1) e->fatal(0x19a);
        int32_t s0 = c[0x15], s1 = c[0x16];
        if (s0 != -1 && s0 != s1) {
            int32_t lo = s0 < s1 ? s0 : s1, hi = s0 < s1 ? s1 : s0;
            int32_t o0 = (int32_t)(ls - tw), o1 = (int32_t)(p - tw);
            if (lo < o0 && hi >= o1) {           // whole line selected
                seg(ls, n);
                int32_t w = e->maxWidth(wb);
                e->fillArea(x0, y - fh, w + x0, y, color, 5);
            } else if (lo < o0 ? (hi >= o0 && hi < o1) : lo < o1) {   // partial
                int32_t a = lo > o0 ? lo : o0, b = hi < o1 ? hi : o1, sx = x0, pre = a - o0;
                if (pre) { seg(ls, pre); sx = e->maxWidth(wb) + x0; }
                seg(ls + pre, b - a);
                int32_t w = e->maxWidth(wb);
                e->fillArea(sx, y - fh, w + sx, y, color, 5);
            }
        }
        seg(ls, n);
        e->drawText(x0, wb, y, 0, c[0x9d], 0);
        int32_t k = c[0x97] - (int32_t)(ls - tw);
        if (blink && !drawn && k >= 0 && k <= n) {
            int32_t cx = x0 + (k ? e->widthN(wb, k) : 0);
            e->drawText(cx, us, y, 0, c[0x9d], 0);
            drawn = true;
        }
        if (wrap) y += fh;
        if (*p == '\r') {
            int32_t off = (int32_t)(p - tw), acc = y - yStart;
            bool lf = p[1] == '\n';
            do {
                if (lf && p[1] != '\n') break;
                acc += fh; p += lf ? 2 : 1; off += lf ? 2 : 1; y += fh;
                if (blink && !drawn && c[0x97] == off && acc < availH)
                    e->drawText(x0, us, y, 0, c[0x9d], 0);
            } while (*p == '\r');
        }
        ls = p;
        if (!*p) return 1;
    }
}
