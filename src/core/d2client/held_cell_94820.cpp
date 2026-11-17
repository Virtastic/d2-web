// The cell a held item's top-left lands on when it is put down at a pointer, scalar front end of D2Client+0x94820.
// The rule is d2_client_94820 (d2client_cells.cpp, VERIFIED). Its two callouts are answered from the item's own numbers:
//  - Ordinal_10114 (via thunk 0xC7F6): the item's footprint in cells (invwidth, invheight);
//  - 0x6E930: the item's inventory cel pixel width then height, which it reads from D2CMP #10060 / #10000 of the
//    cel record #10005 resolves (the played inventory art is drawn at cells x cell box, inventory-panel.mjs).
// area: columns, rows, left, top, cell box width, height of the grid.
#include <cstdint>
#include <cstring>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

struct D2c94820Env {
    void (*itemSize)(uint32_t item, uint8_t* w, uint8_t* h);
    void (*itemOffset)(uint32_t item, uint32_t* a, uint32_t* b);
    void (*fatal)();
};
extern "C" void d2_client_94820(const void* area, uint32_t item, int32_t x, int32_t y, uint32_t* ox, uint32_t* oy, const D2c94820Env* e);

namespace {
struct HeldItem { uint8_t w, h; uint32_t pw, ph; };
HeldItem gHeld;
void itemSize(uint32_t, uint8_t* w, uint8_t* h) { *w = gHeld.w; *h = gHeld.h; }
void itemOffset(uint32_t, uint32_t* a, uint32_t* b) { *a = gHeld.pw; *b = gHeld.ph; }
void fatal() {}
}

extern "C" {
// out[0..1] = the cell; returns 1. A 0 item footprint is the retail fatal path and returns 0.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_held_cell(int32_t cols, int32_t rows, int32_t left, int32_t top, int32_t cellW, int32_t cellH,
                                                 int32_t itemW, int32_t itemH, int32_t pixW, int32_t pixH,
                                                 int32_t x, int32_t y, int32_t* out) {
    if (itemW <= 0 || itemH <= 0 || cellW <= 0 || cellH <= 0) return 0;
    uint8_t area[0x18]; memset(area, 0, sizeof area);
    area[0] = (uint8_t)cols; area[1] = (uint8_t)rows;
    memcpy(area + 4, &left, 4); memcpy(area + 0xC, &top, 4);
    area[0x14] = (uint8_t)cellW; area[0x15] = (uint8_t)cellH;
    gHeld = { (uint8_t)itemW, (uint8_t)itemH, (uint32_t)pixW, (uint32_t)pixH };
    static const D2c94820Env env = { itemSize, itemOffset, fatal };
    uint32_t ox = 0, oy = 0;
    d2_client_94820(area, 1, x, y, &ox, &oy, &env);
    out[0] = (int32_t)ox; out[1] = (int32_t)oy;
    return 1;
}
}
