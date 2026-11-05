// 1.13c D2Win+0x14350 (stdcall, ret 0xC): edit-box caret to pixel. Arguments (ctl, *x, *y); the caret's position inside the box comes back.
// Decompile-derived (cached Ghidra decompile, checked against the disassembly: the decompile writes buf[0] back at the loop tail,
// which the machine code does not do; it only tests *p).
// Walks the control's text (the 16-bit text at +0x5C, narrowed to a 256-byte local) line by line. A line ends at NUL, CR or LF, or when
// the width of the line so far plus the width of "_" exceeds the box width (ctl[5] - 2*ctl[0x10]); the wrapping character starts the
// next line. A CR (or CR LF pairs) after the line advances one line height per break. The walk stops at NUL or when the y offset
// reaches the box height (ctl[6] - 2*ctl[0x11]). The first line position whose range holds the caret index (ctl[0x97], +0x25C) gives
// x = x0 + width(text before the caret on that line), y = that line's y; a caret straight after a CR / CRLF run takes that line.
// Unfound: both stay -1 and are returned unadjusted. Found: x -= ctl[3] + 1, y += (ctl[6]-ctl[4]) - 1, then each clamped to
// [pad, size - pad - 1] (x: ctl[0x10], ctl[5]; y: ctl[0x11], ctl[6]).
// A lone LF (0x0A) in the text never advances the walk (retail loops without progress); text is expected CR LF.
// Type check: null gives fatal 0x648, type != 1 gives fatal 0x64A (report then _exit(-1); a seam, never returns).
// Seams: setFont (0x12FE0, ECX = ctl[0x9C]), width (0x12080: text, n), glyph (DAT 0x21508: character to record, advance at +3),
// win2Unicode / unicode2Win (D2Lang), fontHeight (byte +10 of the current font record), fatal.
#include <cstdint>
#include <cstring>

struct D2Win14350Env {
    void (*fatal)(uint32_t code);
    void (*setFont)(uint32_t font);
    int32_t (*width)(const uint16_t* text, int32_t n);
    const uint8_t* (*glyph)(uint32_t ch);
    void (*win2Unicode)(uint16_t* dst, const char* src, int32_t n);
    void (*unicode2Win)(char* dst, const uint16_t* src, int32_t n);
    uint32_t (*fontHeight)();
};

// 1.13c D2Win+0x14350
extern "C" void d2win_14350(int32_t* c, int32_t* px, int32_t* py, const D2Win14350Env* e) {
    if (!c) e->fatal(0x648);
    else if (c[0] != 1) e->fatal(0x64a);
    uint16_t wide[256], us[2] = { 0, 0 };
    char buf[256], line[256];
    const char under[2] = { 0x5f, 0 };
    e->setFont((uint32_t)c[0x9c]);
    const int32_t x0 = c[3] + c[0x10];
    const int32_t lineH = (int32_t)e->fontHeight();
    const int32_t top = lineH - c[6] + c[4] + c[0x11];
    int32_t y = top;
    e->win2Unicode(us, under, 2);
    const int32_t underW = e->width(us, 2);
    e->unicode2Win(buf, (const uint16_t*)(c + 0x17), 0x100);
    const int32_t availW = c[5] - c[0x10] * 2, availH = c[6] - c[0x11] * 2;
    *px = -1; *py = -1;
    const int32_t caret = c[0x97];
    const char* p = buf;
    while (*p != 0 && y - top < availH) {
        const char* lineStart = p;
        memset(line, 0, sizeof line);
        int32_t n = 0;
        bool wrap = false;
        char ch = *p;
        while (ch != 0 && ch != '\r' && ch != '\n') {
            line[n] = ch;
            e->win2Unicode(wide, line, 0x100);
            int32_t w = 0;
            for (int32_t i = 0; wide[i] != 0 && i < n; ++i)
                if (wide[i] != 10) w += e->glyph(wide[i])[3];
            if (w + underW > availW) { wrap = true; break; }
            ch = *++p; ++n;
        }
        const int32_t rel = caret - (int32_t)(lineStart - buf);
        if (*px == -1 && rel >= 0 && rel <= n) {
            int32_t xx = x0;
            if (rel != 0) { e->win2Unicode(wide, line, 0x100); xx = e->width(wide, rel) + x0; }
            *px = xx; *py = y;
        }
        if (wrap) y += lineH;
        if (*p == '\r') {
            int32_t idx = (int32_t)(p - buf), acc = y - top;
            if (p[1] == '\n') {
                while (p[1] == '\n') {
                    acc += lineH; p += 2; idx += 2; y += lineH;
                    if (*px == -1 && caret == idx && acc < availH) { *px = x0; *py = y; }
                    if (*p != '\r') break;
                }
            } else {
                do {
                    acc += lineH; ++p; ++idx; y += lineH;
                    if (*px == -1 && caret == idx && acc < availH) { *px = x0; *py = y; }
                } while (*p == '\r');
            }
        }
    }
    if (*px != -1) {
        *px = *px - c[3] - 1;
        *py = *py + (c[6] - c[4]) - 1;
        int32_t lo = c[0x10], hi = c[5] - lo - 1, v = *px;
        if (v <= lo) v = lo;
        *px = v < hi ? v : hi;
        lo = c[0x11]; hi = c[6] - lo - 1; v = *py;
        if (v <= lo) v = lo;
        *py = v < hi ? v : hi;
    }
}
