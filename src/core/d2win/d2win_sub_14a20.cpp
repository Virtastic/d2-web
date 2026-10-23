// 1.13c D2Win+0x14A20 (sub_14A20): edit-box draw (background, text, password mask, selection, blinking caret).
// Stateful (globals, GetTickCount, draw seams); callees are seams in the env, as d2win_sub_14140.
#include <cstdint>

struct D2Win14A20Env {
    uint32_t bitMask0, bitMask8;            // gdwBitMasks[0], [2]
    int32_t* focusBox;                      // DAT_6f9014b0
    int32_t capsOn;                         // DAT_6f9014c4
    uint32_t (*tick)();                     // GetTickCount
    void (*assertFail)(int32_t line);       // Ordinal_10265/10024 then exit
    void (*f14140)(int32_t** msg);          // {box, 0x201}
    void (*fill10041)(int32_t* st18);       // Ordinal_10041(st, x, y-1, -1, 5, 0); st[13]=param_1[1]
    void (*f2fe0)(int32_t);
    int32_t (*f37b0)();
    void (*f2940)(const uint16_t*, int32_t y, int32_t, int32_t color, int32_t);
    void (*f2de0)(const uint16_t*, int32_t x, int32_t y);
    int32_t (*f1fb0)(const uint16_t*);      // text width
    void (*f1e80)(const uint16_t*);         // text height (result unused)
    void (*f2080)(const uint16_t*, int32_t n);
    void (*edHook)(uint8_t, uint8_t, uint8_t); // FUN_6f8eed70(0x40,0x40,0x40), result unused
};

static int32_t ulen(const uint16_t* s) { int32_t n = 0; while (s[n]) n++; return n; }

uint32_t d2win_sub_14a20(int32_t* b, D2Win14A20Env& e) {
    if (!b) { e.assertFail(0x49c); return 0; }
    if (b[0] != 1) { e.assertFail(0x49e); return 0; }
    if (!(e.bitMask8 & (uint32_t)b[2]) || !(e.bitMask0 & (uint32_t)b[2])) return 1;
    if (!e.capsOn) b[0xa0] = 0;
    if (b[0xa0]) { int32_t* m[2] = {b, (int32_t*)0x201}; e.f14140(m); }
    uint16_t txt[600] = {0};
    if (b[1]) {
        int32_t st[18] = {0};
        st[13] = b[1];
        e.fill10041(st);
    }
    e.f2fe0(b[0x9c]);
    int32_t x = b[0x10] + b[3];
    int32_t y = (b[0x11] - b[6]) + 0x10 + b[4];
    int32_t n = (b[0x14] - b[0x13]) + 1;
    const uint16_t* src = (const uint16_t*)((uint8_t*)b + b[0x13] * 2 + 0x5c);
    for (int32_t i = 0; i < n && src[i]; i++) txt[i] = src[i];   // Unicode::strncpy
    int32_t len = ulen(txt);
    if (b[0x98] & 1) for (int32_t i = 0; i < len; i++) txt[i] = '*';
    if (!e.f37b0()) {
        e.f2940(txt, y, 0, b[0x9d], 0);
    } else {
        e.edHook(0x40, 0x40, 0x40);
        uint16_t pre[512] = {0}, mid[512] = {0}, post[512] = {0};
        int32_t a = b[0x15], c = b[0x16];
        int32_t lo = (a < c ? a : c) - b[0x13]; if (lo < 0) lo = 0;
        int32_t hi = (a < c ? c : a) - b[0x13] - lo; if (hi < 0) hi = 0;
        for (int32_t i = 0; i < lo && txt[i]; i++) pre[i] = txt[i];
        for (int32_t i = 0; i < hi && txt[lo + i]; i++) mid[i] = txt[lo + i];
        for (int32_t i = 0; (post[i] = txt[lo + hi + i]); i++) {}
        int32_t w = e.f1fb0(pre); e.f1e80(pre); e.f1fb0(mid); e.f1e80(mid);
        w += x;
        e.f2940(pre, y, 0, b[0x9d], 0);
        e.f2de0(mid, w, y);
        e.f2940(post, y, 0, b[0x9d], 0);
    }
    if (e.focusBox == b && ((e.tick() / 0xfa) & 1)) {
        uint16_t cur[2] = {'_', 0};
        if (b[0x97] - b[0x13]) e.f2080(txt, b[0x97] - b[0x13]);
        e.f2940(cur, y, 0, b[0x9d], 0);
    }
    return 1;
}
