// D2Win 0x14A20 (1.13c) (sub_14A20): edit-box draw (background, text, password mask, selection, blinking caret).
// Stateful (globals, GetTickCount, draw seams); callees are seams in the env, as d2win_sub_14140.
#include <cstdint>

struct D2Win14A20Env {
    uint32_t bitMask0, bitMask8;            // gdwBitMasks[0], [2]
    int32_t* focusBox;                      // DAT_6f9014b0
    int32_t capsOn;                         // DAT_6f9014c4
    uint32_t (*tick)();                     // GetTickCount
    void (*assertFail)(int32_t line);       // Ordinal_10265/10024 then exit
    void (*f14140)(int32_t** msg);          // stdcall(&{box, 0x201})
    // Ordinal_10041 (thunk 0x77CE), stdcall(st, x, y, -1, 5, 0); st = 0x12 dwords, st[13] = box[1]
    void (*fill10041)(int32_t* st18, int32_t x, int32_t y, int32_t a3, int32_t a4, int32_t a5);
    void (*f2fe0)(int32_t);                 // ecx = font
    int32_t (*f37b0)(int32_t* box);         // eax = box: a selection to draw
    // 0x12940: eax = x, stdcall(text, y, 0, color, 0)
    void (*f2940)(int32_t x, const uint16_t*, int32_t y, int32_t, int32_t color, int32_t);
    // 0x12DE0: ecx = text, edx = x, stdcall(y, shade, 5, color)
    void (*f2de0)(const uint16_t*, int32_t x, int32_t y, int32_t shade, int32_t mode, int32_t color);
    int32_t (*f1fb0)(const uint16_t*);      // eax = text: width
    void (*f1e80)(const uint16_t*);         // esi = text: height (result unused)
    int32_t (*f2080)(const uint16_t*, int32_t n);   // ecx = text, edx = n: width of the first n
    uint8_t (*edHook)(uint8_t, uint8_t, uint8_t);   // FUN_6f8eed70(0x40,0x40,0x40): nearest palette index
};

static int32_t ulen(const uint16_t* s) { int32_t n = 0; while (s[n]) n++; return n; }
static void uncpy(uint16_t* d, const uint16_t* s, int32_t n) {   // Unicode::strncpy: copy, NUL-pad to n
    int32_t i = 0;
    for (; i < n && s[i]; i++) d[i] = s[i];
    for (; i < n; i++) d[i] = 0;
}

uint32_t d2win_sub_14a20(int32_t* b, D2Win14A20Env& e) {  // 1.13c D2Win+0x14A20
    if (!b) { e.assertFail(0x49c); return 0; }
    if (b[0] != 1) { e.assertFail(0x49e); return 0; }
    if (!(e.bitMask8 & (uint32_t)b[2]) || !(e.bitMask0 & (uint32_t)b[2])) return 1;
    if (!e.capsOn) b[0xa0] = 0;
    if (b[0xa0]) { int32_t* m[2] = {b, (int32_t*)0x201}; e.f14140(m); }
    uint16_t txt[300] = {0};
    if (b[1]) {
        int32_t st[18] = {0};
        st[13] = b[1];
        e.fill10041(st, b[3], b[4] - 1, -1, 5, 0);
    }
    e.f2fe0(b[0x9c]);
    int32_t x = b[0x10] + b[3];
    int32_t y = (b[0x11] - b[6]) + 0x10 + b[4];
    uncpy(txt, (const uint16_t*)((uint8_t*)b + b[0x13] * 2 + 0x5c), (b[0x14] - b[0x13]) + 1);
    int32_t len = ulen(txt);
    if (b[0x98] & 1) for (int32_t i = 0; i < len; i++) txt[i] = '*';
    if (!e.f37b0(b)) {
        e.f2940(x, txt, y, 0, b[0x9d], 0);
    } else {
        uint8_t shade = e.edHook(0x40, 0x40, 0x40);
        uint16_t pre[256] = {0}, mid[256] = {0}, post[256] = {0};
        int32_t a = b[0x15], c = b[0x16];
        int32_t lo = (a < c ? a : c) - b[0x13]; if (lo < 0) lo = 0;
        int32_t hi = (a > c ? a : c) - b[0x13] - lo; if (hi < 0) hi = 0;
        uncpy(pre, txt, lo);
        uncpy(mid, txt + lo, hi);
        { int32_t i = 0; while ((post[i] = txt[lo + hi + i])) i++; }   // Unicode::strcpy
        int32_t wpre = e.f1fb0(pre); e.f1e80(pre);
        int32_t wmid = e.f1fb0(mid); e.f1e80(mid);
        e.f2940(x, pre, y, 0, b[0x9d], 0);
        e.f2de0(mid, x + wpre, y, shade, 5, b[0x9d]);
        e.f2940(x + wpre + wmid, post, y, 0, b[0x9d], 0);
    }
    if (e.focusBox == b && ((e.tick() / 0xfa) & 1)) {
        uint16_t cur[2] = {'_', 0};
        if (b[0x97] - b[0x13]) x += e.f2080(txt, b[0x97] - b[0x13]);
        e.f2940(x, cur, y, 0, b[0x9d], 0);
    }
    return 1;
}
