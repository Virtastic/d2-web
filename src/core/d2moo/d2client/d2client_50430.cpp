// 1.13c D2Client+0x50430: the right hint panel twin of 0x501F0 (string 0xF92, mirrored x). Clears the 0x11C308 flag when the mode is not 2
// and a blocking window is up. No arguments, no result.
// Globals (RVA): W 0xDBC48, H 0xDBC4C, G414 0x11C414, G300 / G304 (cell words), G308 0x11C308 (in/out), G824 / G828 (mouse),
// AD84 0xFAD84, AD88 0xFAD88, AD90 0xFAD90, ADA4 0xFADA4, ADB0 0xFADB0, ADD8 0xFADD8 (window flags).
// Seams as 0x501F0 with 0x4FF10 (no args) and 0x4FF60 (EAX, ECX, ESI) in place of 0x4FE70 / 0x4FEC0.
#include <cstdint>
namespace d2c50430 {
struct G { int32_t w, h; uint32_t ad84, ad88, ad90, ada4, adb0, add8, g824, g828, g300, g304, g308, g414; };
struct Env {
    void* c;
    uint32_t (*mode)(void*);
    uint32_t (*cmp21)(void*, const uint32_t* frame);
    int32_t (*cmp60)(void*, uint32_t v);
    const void* (*lang)(void*, uint32_t id);
    int32_t (*twidth)(void*, const void* s);
    void (*draw150)(void*, const void* s, int32_t x, int32_t y);
    void (*draw85)(void*, const void* s, int32_t x, int32_t y);
    void (*cell)(void*, const uint32_t* f, int32_t x, int32_t y, int32_t col, int32_t dm, int32_t pal);
    uint32_t (*hit)(void*);                                                         // 0x4FF10
    uint32_t (*hit2)(void*, uint32_t eax, uint32_t ecx, uint32_t esi);              // 0x4FF60
};
static int32_t half(int32_t v) { return (v - (v >> 31)) >> 1; }
// 1.13c D2Client+0x50430
void sub_50430(const Env& e, G& g) {
    const uint32_t m414 = g.g414;
    if (e.mode(e.c) != 2 && (m414 == 3 || g.ad88 || (g.add8 && (g.ad84 || g.ad90)) || g.adb0)) { g.g308 = 0; return; }
    const int32_t ebx = m414 == 2 ? half(g.w) + 0x28 : 0x28;
    uint32_t f[18];
    for (int i = 0; i < 18; i++) f[i] = 0;
    f[13] = g.g304;
    const int32_t w = e.cmp60(e.c, e.cmp21(e.c, f));
    const int32_t tw = e.twidth(e.c, e.lang(e.c, 0xF92));
    if (e.mode(e.c) != 2) {
        const int32_t x = half(w) - half(tw) + ebx + 1;
        e.draw150(e.c, e.lang(e.c, 0xF92), x, g.h - 0x8E);
        e.cell(e.c, f, ebx, g.h - 0x69, -1, 5, 0);
    }
    uint32_t f2[18];
    for (int i = 0; i < 18; i++) f2[i] = 0;
    f2[13] = g.g300;
    if (e.mode(e.c) == 2) f2[0] = g.g308 && e.hit(e.c) ? 1 : 0;
    else f2[0] = g.g308 && e.hit2(e.c, m414, g.g828, g.g824) ? 1 : 0;
    e.cmp60(e.c, e.cmp21(e.c, f2));
    e.twidth(e.c, e.lang(e.c, 0xF92));
    if (e.mode(e.c) != 2) { e.cell(e.c, f2, ebx + 3, g.h - 0x6D, -1, 5, 0); return; }
    if (e.hit(e.c) && g.ada4 == 0) e.draw85(e.c, e.lang(e.c, 0xF92), half(g.w) - 0xB3, g.h - 0x32);
    e.cell(e.c, f2, half(g.w) - 0xC2, g.h - 8, -1, 5, 0);
}
}
