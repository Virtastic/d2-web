// 1.13c D2Win+0x14790 (sub_14790, called only by 0x15450): vertical extent of the edit box's text.
// EAX = box (int32 view), arg = UTF-16 text. Word-wraps (per-char, no word breaks) at box[5]-2*box[0x10]
// using glyph advances + the width of '_'; every wrap and every CR / CRLF run adds one line height.
// Returns (end y - start y). A lone '\n' not preceded by '\r' never advances (retail spins; kept 1:1).
#include <cstdint>
#include <cstring>

struct D2WinSub14790Env {
    void (*setFont)(int32_t font);                           // 0x12FE0
    void (*win2Unicode)(uint16_t* dst, const char* src, int32_t n);
    int32_t (*unicode2Win)(char* dst, const uint16_t* src, int32_t n);
    int32_t (*textWidth)(const uint16_t* s, int32_t n);      // 0x12080
    const uint8_t* (*glyph)();                               // DAT_6f901508 (byte +3 = advance)
    uint8_t (*lineHeight)();                                 // font table[cur]->byte +10
    void (*fatal)(int32_t code);                             // Ordinal_10265/10024 + _exit(-1)
};

int32_t d2win_sub_14790(const int32_t* box, const uint16_t* text, const D2WinSub14790Env& e) {
    if (!box) { e.fatal(0x5ea); return 0; }
    if (box[0] != 1) { e.fatal(0x5ec); return 0; }
    e.setFont(box[0x9c]);
    const int32_t lh = e.lineHeight();
    int32_t y = lh - box[6] + box[0x11] + box[4];
    const int32_t y0 = y;
    uint16_t us[2] = {0, 0};
    e.win2Unicode(us, "_", 2);
    const int32_t uw = e.textWidth(us, 2);
    uint16_t w16[0x100] = {};
    char src[0x100];
    e.unicode2Win(src, text, 0x100);
    const int32_t maxw = box[5] - box[0x10] * 2;
    const char* p = src;
    for (;;) {
        if (*p == 0) return y - y0;
        char line[0x100] = {};
        int32_t n = 0;
        bool wrapped = false;
        for (char c = *p; c && c != '\r' && c != '\n'; c = *++p, ++n) {
            line[n] = c;
            e.win2Unicode(w16, line, 0x100);
            int32_t w = 0;
            for (int32_t i = 0; w16[i] && i < n; ++i)
                if (w16[i] != 10) w += e.glyph()[3];
            if (maxw < w + uw) { wrapped = true; break; }
        }
        e.win2Unicode(w16, line, 0x100);
        if (wrapped) y += lh;
        if (*p == '\r') {
            if (p[1] == '\n') {
                do {
                    if (p[1] != '\n') break;
                    p += 2; y += lh;
                } while (*p == '\r');
            } else {
                do { ++p; y += lh; } while (*p == '\r');
            }
        }
    }
}
