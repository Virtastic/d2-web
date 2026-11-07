// D2Client+0xBEE60: "you died" overlay draw (hardcore / instance banners + death text). Env-seam style.
// Register conventions read from the 1.13c disassembly: cel draw = ECX cel, EDX x, stack y,1,5,1; text draw = ECX wide string,
// EDX x, stack y,1,1; font select ECX=2; string width ECX=string; string fetch ECX=id.
#include <cstdint>
#include <emscripten.h>
struct D2cBee60Env {
    int32_t* player;          // DAT_6fbcbbfc (null: return)
    int32_t scrW;             // DAT_6fb8bc48 (width, cel x = W/2)
    int32_t scrH;             // DAT_6fb8bc4c (height, banner y = H/2 - 0x5e)
    int32_t hudW;             // DAT_6fba9e14 (text centring width)
    uint8_t* gameInfo;        // DAT_6fbcb980 (flag byte at +0x1eb)
    uint8_t flag390;          // DAT_6fbcc390
    void** cel9dc;            // DAT_6fbcb9dc youdiedhardcore cel
    void** cel9e0;            // DAT_6fbcb9e0 youdiedinst cel
    int32_t* drawn9d8;        // DAT_6fbcb9d8
    void (*loadCel)(void** slot, const char* name);   // sprintf "%s\\UI\\%s\\<name>" + 0x2B420
    void (*drawCel)(void* cel, int32_t x, int32_t y, int32_t a, int32_t b, int32_t c); // 0x6fabd372
    int32_t (*getStat)(int32_t* u, int32_t stat, int32_t z);                 // 0x6fabc1c6 (stat 0xaf)
    void (*fmtDeath)(int32_t stat);    // Unicode sprintf(0x100, buf, string 0x13e6, stat) into the line buffer
    void (*fmtHost)();                 // Unicode strcpy(buf, string 0x13e8) into the line buffer
    void (*setFont)(int32_t f);        // 0x6fabd3a8
    int32_t (*lineWidth)();            // 0x6fabd35a on the line buffer
    void (*drawLine)(int32_t x, int32_t y, int32_t a, int32_t b);            // 0x6fabd37e on the line buffer
    int32_t (*width2)();               // 0x6fabd35a on string 0x13e7
    void (*drawLine2)(int32_t x, int32_t y, int32_t a, int32_t b);           // 0x6fabd37e on string 0x13e7
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
    int32_t y = e->scrH / 2 - 0x5e;
    if (!*e->cel9dc) e->loadCel(e->cel9dc, "youdiedhardcore");
    if (*e->cel9dc) e->drawCel(*e->cel9dc, e->scrW / 2, y, 1, 5, 1);
    y += 0x30;
    *e->drawn9d8 = 1;
    if (!*e->cel9e0) e->loadCel(e->cel9e0, "youdiedinst");
    if (*e->cel9e0) e->drawCel(*e->cel9e0, e->scrW / 2, y, 1, 5, 1);
    bool skip = false;
    if (!(e->gameInfo[0x1eb] & 4)) {
        int32_t s = e->getStat(p, 0xaf, 0);
        if (!s) skip = true; else e->fmtDeath(s);
    } else e->fmtHost();
    if (!skip) {
        y += 0x30;
        e->setFont(2);
        e->drawLine(e->hudW / 2 - e->lineWidth() / 2, y, 1, 1);
    }
    if (e->flag390 && !(e->gameInfo[0x1eb] & 4)) {
        e->setFont(2);
        int32_t w = e->width2();
        y += 0x30;
        e->drawLine2(e->hudW / 2 - w / 2, y, 1, 1);
    }
}
}
