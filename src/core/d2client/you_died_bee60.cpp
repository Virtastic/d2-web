// D2Client+0xBEE60: "you died" overlay draw (hardcore / instance banners + death text). Env-seam style.
// Ordinals are unverified (D2Win/D2Lang imports), named by role only.
#include <cstdint>
#include <emscripten.h>
struct D2cBee60Env {
    int32_t* player;          // DAT_6fbcbbfc (unit/ui struct; null -> return)
    int32_t scrW;             // DAT_6fb8bc4c
    uint8_t* gameInfo;        // DAT_6fbcb980 (flag byte at +0x1eb)
    uint8_t flag390;          // DAT_6fbcc390
    void** cel9dc;            // DAT_6fbcb9dc youdiedhardcore cel
    void** cel9e0;            // DAT_6fbcb9e0 youdiedinst cel
    int32_t* drawn9d8;        // DAT_6fbcb9d8
    void (*loadCel)(void** slot, const char* name);   // FUN_6fadb420 via sprintf "%s\\UI\\%s\\<name>"
    void (*drawCel)(void* cel, int32_t x, int32_t a, int32_t b, int32_t c); // Ordinal_10138 (unverified)
    int32_t (*getStat)(int32_t* u, int32_t stat, int32_t z);                 // Ordinal_10973
    void (*fmtText)(int32_t stat);                                            // Unicode::sprintf/strcpy path
    void (*drawText)(int32_t x, int32_t a, int32_t b);                        // Ordinal_10184/10028/10150 (unverified)
    void (*drawText2)(int32_t x);                                             // second text block (+0x30)
};
extern "C" {
// 1.13c D2Client+0xBEE60
EMSCRIPTEN_KEEPALIVE void d2c_bee60(const D2cBee60Env* e) {
    int32_t* p = e->player;
    if (!p) return;
    if (!(p[0x31] & 0x10000u)) {
        int32_t m = p[4];
        if (p[0] == 0) { if (m != 0 && m != 0x11) { *e->drawn9d8 = 0; return; } }
        else if (p[0] != 1) { *e->drawn9d8 = 0; return; }
        else if (m != 0 && m != 0xc) { *e->drawn9d8 = 0; return; }
    }
    int32_t h = e->scrW / 2;
    if (!*e->cel9dc) e->loadCel(e->cel9dc, "youdiedhardcore");
    if (*e->cel9dc) e->drawCel(*e->cel9dc, h - 0x5e, 1, 5, 1);
    int32_t x = h - 0x2e;
    *e->drawn9d8 = 1;
    if (!*e->cel9e0) e->loadCel(e->cel9e0, "youdiedinst");
    if (*e->cel9e0) e->drawCel(*e->cel9e0, x, 1, 5, 1);
    bool skip = false;
    if (!(e->gameInfo[0x1eb] & 4)) {
        int32_t s = e->getStat(p, 0xaf, 0);
        if (!s) skip = true; else e->fmtText(s);
    } else e->fmtText(-1);
    if (!skip) { x = h + 2; e->drawText(x, 1, 1); }
    if (e->flag390 && !(e->gameInfo[0x1eb] & 4)) e->drawText2(x + 0x30);
}
}
