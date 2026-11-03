// 1.13c D2Client+0xA9C90 (410 B, ret 4): rebuild the 48x48 light mask around the player.
// DAT 0x11BBFC (unit handle) != 0: unit = C1F0(handle). unit == 0 or stack param != 0: A8560(x1=0,y1=0,x2=0x2F,y2=0x2F, colour 0) and done.
// Else C3DC(unit,&arr,&count) lists the neighbours; the unit's own colour (A9A20(1, unit) gives alpha oa, r, g, b; colour = b<<24|g<<16|r<<8|alpha) fills
// the whole mask; then each neighbour != unit: its colour the same way, C5B0(neighbour,&rect{x,y,w,h}); dx = x - DAT 0x104DF0, dy = y - DAT 0x104DF4;
// rejected when dx > 0x30, dy > 0x30, w+dx < 0 or h+dy < 0; the box (max(dx,0), max(dy,0)) .. (min(w+dx,0x2F), min(h+dy,0x2F)) is filled via A8560.
// A8560: EAX = y2, ECX = y1, EDX = colour, EDI = x2, stack x1 (the seam passes EDI in the esi slot).
#include <emscripten.h>
#include "d2client_a9_env.h"
struct D2cA9c90G { uint32_t unitH; int32_t org_x; int32_t org_y; };   // RVAs 0x11BBFC, 0x104DF0, 0x104DF4
static void fill(const D2cA9Env* e, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t col) { e->call(e->ctx, 0xA8560, y2, y1, col, x2, x1, 0, 0, 0, 0); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_a9c90(uint32_t param, const D2cA9c90G* g, const D2cA9Env* e) {
    uint32_t unit = 0;
    if (g->unitH) unit = e->call(e->ctx, 0xC1F0, 0, 0, 0, 0, g->unitH, 0, 0, 0, 0);
    if (!unit || param) { fill(e, 0, 0, 0x2f, 0x2f, 0); return; }
    uint32_t arrP = 0; int32_t count = 0;
    e->call(e->ctx, 0xC3DC, 0, 0, 0, 0, unit, d2c_hp(&arrP), d2c_hp(&count), 0, 0);
    uint32_t oa = 0; uint8_t r = 0, gg = 0, b = 0;
    auto colour = [&](uint32_t u) {
        e->call(e->ctx, 0xA9A20, 1, d2c_hp(&r), 0, u, d2c_hp(&oa), d2c_hp(&r), d2c_hp(&gg), d2c_hp(&b), 0);
        return ((uint32_t)b << 24) | ((uint32_t)gg << 16) | ((uint32_t)r << 8) | oa;
    };
    fill(e, 0, 0, 0x2f, 0x2f, colour(unit));
    const uint32_t* arr = (const uint32_t*)(uintptr_t)arrP;
    for (int32_t i = 0; i < count; ++i) {
        const uint32_t n = arr[i];
        if (n == unit) continue;
        const uint32_t col = colour(n);
        int32_t rc[4] = { 0, 0, 0, 0 };
        e->call(e->ctx, 0xC5B0, 0, 0, 0, 0, n, d2c_hp(rc), 0, 0, 0);
        const int32_t dx = rc[0] - g->org_x, w2 = rc[2] + dx;
        if (dx > 0x30 || w2 < 0) continue;
        const int32_t dy = rc[1] - g->org_y, h2 = rc[3] + dy;
        if (dy > 0x30 || h2 < 0) continue;
        fill(e, dx < 0 ? 0 : (uint32_t)dx, dy < 0 ? 0 : (uint32_t)dy, w2 < 0x30 ? (uint32_t)w2 : 0x2f, h2 < 0x30 ? (uint32_t)h2 : 0x2f, col);
    }
}
