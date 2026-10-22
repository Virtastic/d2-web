// 1.13c D2Win+0x14140 (sub_14140): textbox mouse-down/drag handler (caret placement, selection, double-click).
// Stateful (globals, GetTickCount, widget struct, callbacks); NAMED only, no vector.
#include <cstdint>

struct D2WinMsg { int32_t* box; int32_t msg; };

struct D2Win14140Env {
    int32_t mouseX, mouseY;                 // DAT_6f901488 / DAT_6f90148c
    uint32_t bitMask0, bitMask8;            // gdwBitMasks[0], [2]
    int32_t* lastBox; uint32_t lastTick;    // DAT_6f9a9d04 / 08
    uint32_t (*tick)();
    bool (*shift)();                        // GetKeyState(VK_SHIFT) < 0
    void (*assertFail)(int32_t line);       // Ordinal_10265/10024 then exit
    void (*f7a90)(int32_t*);
    void (*f3720)(int32_t*);
    void (*f2fe0)(int32_t);
    void (*f3ac0)();
    void (*f3770)(int32_t*);
    int32_t (*f2080)(int16_t*, int32_t);
    int32_t (*ustrlen)(int16_t*);
};

uint32_t d2win_sub_14140(int32_t** p, D2Win14140Env& e) {
    int32_t* b = p[0];
    if (!b) { e.assertFail(0x6d0); return 0; }
    if (b[0] != 1) { e.assertFail(0x6d1); return 0; }
    if (!(e.bitMask8 & (uint32_t)b[2]) || !(e.bitMask0 & (uint32_t)b[2])) return 0;
    e.f7a90(b);
    int32_t w = b[0x10];
    int32_t x = e.mouseX - (b[3] + w);
    int32_t h = b[6] - b[0x11];
    int32_t y = e.mouseY + (h - b[4]);
    int32_t r = b[5];
    bool in = !(x < -w || r - w <= x || y <= -b[0x11] || h <= y);
    uint32_t t = e.tick();
    if (in && b[0xa0] == 0 && b == e.lastBox && t - e.lastTick < 300) {
        e.f3720(b);
    } else {
        if (b[0xa0] != 0 || in) {
            int32_t n = 1;
            e.f2fe0(b[0x9c]);
            int16_t* s = (int16_t*)((uint8_t*)b + b[0x13] * 2 + 0x5c);
            if (b[0xa0] != 0) {
                uint32_t c;
                if (x < -b[0x10]) {
                    c = (uint32_t)(b[0x97] - 1) & (uint32_t)(((b[0x97] - 1) < 1) - 1);
                } else if (x < r - w) {
                    goto scan;
                } else {
                    int32_t l = e.ustrlen((int16_t*)(b + 0x17));
                    c = (uint32_t)(l - 1);
                    if ((int32_t)(b[0x97] + 1) < (int32_t)(l - 1)) c = (uint32_t)(b[0x97] + 1);
                }
                b[0x97] = (int32_t)c;
                e.f3ac0();
            }
        scan:
            int32_t v = e.f2080(s, 1);
            while (v < x && n <= e.ustrlen(s)) { n++; v = e.f2080(s, n); }
            int32_t old = b[0x97];
            b[0x97] = b[0x13] - 1 + n;
            if (e.shift() || b[0xa0] != 0) {
                if (b[0x15] == -1) b[0x15] = old;
                b[0x16] = b[0x97];
            } else e.f3770(b);
        }
        if (p[1] == (int32_t*)0x201) b[0xa0] = 1;
    }
    e.lastBox = b; e.lastTick = t;
    if (!b[0xd]) return 0;
    return ((uint32_t(*)(int32_t**))(uintptr_t)(uint32_t)b[0xd])(p);
}
