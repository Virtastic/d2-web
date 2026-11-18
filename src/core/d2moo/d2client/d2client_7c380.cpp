// 1.13c D2Client+0x7C380 (437 B): draw the scroll panel. A fill rect (Ordinal_10014), four scroin cells (frames 0..3) and the
// buy/sell button (frame 10) through Ordinal_10041 with an 18-dword draw context {frame at +0, cellfile at +0x34}; the two
// cellfiles are loaded once (0x2B420, EAX = path, one stack word 0) into DAT +0x11BFB0 / +0x11B9C4.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c7C380Env {
    void (*rect)(int32_t x, int32_t y, int32_t w, int32_t h, int32_t a4, int32_t a5);                          // Ordinal_10014 (0xD0BA, stdcall 6)
    void (*draw)(const uint32_t* ctx, int32_t x, int32_t y, int32_t c, int32_t mode, int32_t z);               // Ordinal_10041 (0xD096, stdcall 6)
    uint32_t (*load)(const char* path);                                                                        // 0x2B420
};

static void cat(char* d, const char* a, const char* b) { std::strcpy(d, a); std::strcat(d, b); }   // wsprintfA("%s<fmt tail>", "DATA\\GLOBAL")

extern "C" EMSCRIPTEN_KEEPALIVE
void d2c_port_7c380(const D2c7C380Env* e, uint32_t* scroin /*+0x11BFB0*/, uint32_t* buysell /*+0x11B9C4*/,
                    int32_t w /*+0xDBC48*/, int32_t h /*+0xDBC4C*/, int32_t px /*+0x11B9A0*/, int32_t py /*+0x11B9A4*/) {
    char path[284];
    e->rect(px, py - 0xe0 + h, w / 2, h - 0x30, 0, 5);
    if (!*scroin) { cat(path, "DATA\\GLOBAL", "\\UI\\MENU\\scroin"); *scroin = e->load(path); }
    uint32_t ctx[18] = {};
    ctx[13] = *scroin;
    const int32_t ya = py - 0x6d + h, yb = py - 0x31 + h;
    ctx[0] = 0; e->draw(ctx, px, ya, -1, 5, 0);
    ctx[0] = 1; e->draw(ctx, px + 0x100, ya, -1, 5, 0);
    ctx[0] = 2; e->draw(ctx, px, yb, -1, 5, 0);
    ctx[0] = 3; e->draw(ctx, px + 0x100, yb, -1, 5, 0);
    if (!*buysell) { cat(path, "DATA\\GLOBAL", "\\ui\\panel\\buysellbtn"); *buysell = e->load(path); }
    ctx[13] = *buysell;
    ctx[0] = 10; e->draw(ctx, px + 0x115, py - 0x3d + h, -1, 5, 0);
}
