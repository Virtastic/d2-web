// 1.13c D2Win+0x13DA0 -- textbox mouse handler: msg->[0] is the control record (type 1, flags
// gate bitmasks[0] and [2]); map the mouse point (x,y) to a character index through the wrapped
// text lines and move the caret (+0x25C) / selection (+0x54,+0x58); then forward to the control's
// callback (+0x34). Null / wrong-type record is fatal (strings 0x74E/0x74F -> Fog 10024, _exit).
// Stateful (current font, current object, key state): seams in D2WinTbEnv.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void d2win_exit(uint32_t code);

struct D2WinTbEnv {
    uint32_t mask0, mask2;                         // gdwBitMasks[0], [2]
    void (*setCurrent)(uint8_t* c);                // 0x17A90
    void (*setFont)(int32_t font);                 // 0x12FE0
    int32_t (*width)(const int16_t* s, int32_t n); // 0x12080
    void (*win2uni)(int16_t* out, const char* in, int32_t cap);
    int32_t (*lineHeight)();                       // font table byte +10
    bool (*shift)();                               // GetKeyState(0x10) < 0
    void (*clearSel)(uint8_t* c);                  // 0x13770
    uint32_t (*callback)(uint32_t cb, void* msg);
};

static int32_t rd(const uint8_t* p, int o) { int32_t v; std::memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, int o, int32_t v) { std::memcpy(p + o, &v, 4); }

uint32_t d2win_sub_13da0(uint8_t* c, uint32_t msgType, void* msg, int32_t x, int32_t y, const D2WinTbEnv& e) {
    if (!c || rd(c, 0) != 1) d2win_exit((uint32_t)-1);
    uint32_t flags = (uint32_t)rd(c, 8);
    if (!(e.mask2 & flags) || !(e.mask0 & flags)) return 0;
    e.setCurrent(c);
    int32_t w = rd(c, 20), h = rd(c, 24), px = rd(c, 0x40), py = rd(c, 0x44);
    bool drag = rd(c, 0x280) != 0;
    if (drag) {
        int32_t hi = w - px - 1; x = x < px ? px : x; if (x > hi) x = hi;
        hi = h - py - 1; y = y < py ? py : y; if (y > hi) y = hi;
    }
    if (!(x < -px || w - px <= x || y < -py || h - py <= y)) {
        e.setFont(rd(c, 0x270));
        int16_t us = '_', ub[2] = {0, 0};
        e.win2uni(ub, (const char*)&us, 2);
        int32_t un = 0; while (ub[un]) ++un;
        int32_t us_w = e.width(ub, un);
        char text[256]; { int16_t* t = (int16_t*)(c + 0x5C); int i = 0; for (; i < 255 && t[i]; ++i) text[i] = (char)t[i]; text[i] = 0; }
        int32_t lh = e.lineHeight(), maxw = w - px * 2, ly = 0, oldCur = 0, prevCur = -1;
        bool hit = false;
        const char* base = text; const char* p = text;
        if (*p) do {
            const char* ls = p; char line[256]; int16_t uni[256];
            std::memset(uni, 0, sizeof uni); std::memset(line, 0, sizeof line);
            char ch = *p; int32_t i = 0; bool wrap = false;
            for (;; ++p, ++i, ch = *p) {
                if (!ch || ch == '\r' || ch == '\n') break;
                line[i] = ch; e.win2uni(uni, line, 256);
                if (maxw < e.width(uni, i) + us_w) { wrap = true; break; }
            }
            if (ly <= y && y < ly + lh) {
                int32_t k = 1; e.win2uni(uni, line, 256);
                int32_t cw = e.width(uni, 1);
                while (cw < x && k <= i) cw = e.width(uni, ++k);
                prevCur = rd(c, 0x25C); wr(c, 0x25C, (int32_t)(ls - base) + k - 1); hit = true;
            }
            if (wrap) ly += lh;
            else if (*p == '\r') { p += (p[1] == '\n') ? 2 : 1; ly += lh; }  // bare '\n' neither advances: retail loops
        } while (*p);
        oldCur = hit ? prevCur : rd(c, 0x25C);
        if (!hit) { int32_t n = 0; while (((int16_t*)(c + 0x5C))[n]) ++n; wr(c, 0x25C, n); }
        if ((e.shift() || drag) && oldCur != -1) {
            if (rd(c, 0x54) == -1) wr(c, 0x54, oldCur);
            wr(c, 0x58, rd(c, 0x25C));
        } else e.clearSel(c);
    }
    if (msgType == 0x201) wr(c, 0x280, 1);
    uint32_t cb = (uint32_t)rd(c, 0x34);
    return cb ? e.callback(cb, msg) : 0;
}
