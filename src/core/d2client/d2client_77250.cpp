// 1.13c D2Client+0x77250 (144 B, caller 0xC39E0): draw the Party screen. Eight draw steps in a fixed order, then the close-button tip.
// Order (1.13c disassembly): 0x750B0, 0x760F0, 0x76790, 0x755D0, 0x76050, 0x75D30, 0x76B20, 0x753D0 (no arguments).
// Tip: with mx = mouse x (0x11B828), my = mouse y (0x11B824), px = panel left (0x11B9A0), py = panel shift (0x11B9A4):
//   px + 0x116 <= mx <= px + 0x136 and 0x185 - py <= my <= 0x1A5 - py  ->  tip(string 0x102F) at (px + 0x125, 0x182 - py), centred flag 0, mode 1
//   (string fetch 0xD1EC with ECX = 0x102F, then the text draw 0xD384 with ECX = text, EDX = px + 0x125, stack 0x182 - py, 0, 1).
// Prior art: a 1.14d note names this screen (UI 0x16) as the party panel with a close button; the rectangle, string id and draw order are 1.13c's own.
#include <cstdint>

struct D2c77250Globals { int32_t px, py, mx, my; };      // 0x11B9A0, 0x11B9A4, 0x11B828, 0x11B824

struct D2c77250Env {
    void* ctx;
    void (*step)(void*, uint32_t rva);                      // the eight draw steps, by RVA
    uint32_t (*string)(void*, uint32_t id);                 // 0xD1EC (ECX = id)
    void (*text)(void*, uint32_t str, int32_t x, int32_t y, uint32_t centre, uint32_t mode);   // 0xD384 (ECX = str, EDX = x, stack y, centre, mode)
};

extern "C" void d2c_77250(const D2c77250Globals* g, const D2c77250Env* e) {
    static const uint32_t order[8] = { 0x750B0, 0x760F0, 0x76790, 0x755D0, 0x76050, 0x75D30, 0x76B20, 0x753D0 };
    for (uint32_t i = 0; i < 8; ++i) e->step(e->ctx, order[i]);
    const auto add = [](int32_t a, uint32_t b) { return int32_t(uint32_t(a) + b); };      // 32-bit lea / sub wrap, as retail
    const auto rsub = [](uint32_t a, int32_t b) { return int32_t(a - uint32_t(b)); };
    if (g->mx < add(g->px, 0x116) || g->mx > add(g->px, 0x136)) return;
    if (g->my < rsub(0x185, g->py) || g->my > rsub(0x1A5, g->py)) return;
    const uint32_t s = e->string(e->ctx, 0x102F);
    e->text(e->ctx, s, add(g->px, 0x125), rsub(0x182, g->py), 0, 1);
}
