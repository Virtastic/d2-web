// 1.13c D2Client+0x501F0: draw the left hint panel (two cell frames and the 0xF93 string) of the 640/800 HUD; clears the 0x11C30C flag
// when the mode is not 2 and a blocking window is up. No arguments, no result.
// Globals (RVA): W 0xDBC48, H 0xDBC4C, mode flag G414 0x11C414, G300 0x11C300 / G304 0x11C304 (cell words), G30C 0x11C30C (in/out), G824 0x11B824,
// G828 0x11B828 (mouse), AD84 0xFAD84, AD90 0xFAD90, ADA4 0xFADA4, ADB0 0xFADB0, ADD8 0xFADD8 (window flags).
// Seams: D2gfx 10031, D2CMP 10021 / 10060, D2Lang 10003, D2Win 10028 / 10150 / 10085, D2gfx 10041, 0x4FE70 (no args) and 0x4FEC0 (EAX, ECX, EDI).
#include <cstdint>
namespace d2c501f0 {
struct G { int32_t w, h; uint32_t ad84, ad90, ada4, adb0, add8, g824, g828, g300, g304, g30c, g414; };
struct Env {
    void* c;
    uint32_t (*mode)(void*);                                                        // D2gfx 10031
    uint32_t (*cmp21)(void*, const uint32_t* frame);                                // D2CMP 10021 (stdcall, 1 arg)
    int32_t (*cmp60)(void*, uint32_t v);                                            // D2CMP 10060 (stdcall, 1 arg)
    const void* (*lang)(void*, uint32_t id);                                        // D2Lang 10003 (ecx = id)
    int32_t (*twidth)(void*, const void* s);                                        // D2Win 10028 (ecx = string)
    void (*draw150)(void*, const void* s, int32_t x, int32_t y);                    // D2Win 10150 (ecx, edx = x; stack y, 0, 0)
    void (*draw85)(void*, const void* s, int32_t x, int32_t y);                     // D2Win 10085 (ecx, edx = x; stack y, 0, 1)
    void (*cell)(void*, const uint32_t* f, int32_t x, int32_t y, int32_t col, int32_t dm, int32_t pal);   // D2gfx 10041
    uint32_t (*hit)(void*);                                                         // 0x4FE70
    uint32_t (*hit2)(void*, uint32_t eax, uint32_t ecx, uint32_t edi);              // 0x4FEC0
};
static int32_t half(int32_t v) { return (v - (v >> 31)) >> 1; }   // cdq; sub eax,edx; sar eax,1
// 1.13c D2Client+0x501F0
void sub_501F0(const Env& e, G& g) {
    const uint32_t m414 = g.g414;
    if (e.mode(e.c) != 2 && (m414 == 3 || g.ad90 || g.adb0 || (g.add8 && g.ad84))) { g.g30c = 0; return; }
    const int32_t ebx = (m414 == 1 ? g.w - half(g.w) : g.w) - 0x49;
    uint32_t f[18];
    for (int i = 0; i < 18; i++) f[i] = 0;
    f[13] = g.g304;
    const int32_t w = e.cmp60(e.c, e.cmp21(e.c, f));
    const int32_t tw = e.twidth(e.c, e.lang(e.c, 0xF93));
    if (e.mode(e.c) != 2) {
        const int32_t x = half(w) - half(tw) + ebx + 1;
        e.draw150(e.c, e.lang(e.c, 0xF93), x, g.h - 0x8E);
        e.cell(e.c, f, ebx, g.h - 0x69, -1, 5, 0);
    }
    uint32_t f2[18];
    for (int i = 0; i < 18; i++) f2[i] = 0;
    f2[13] = g.g300;
    if (e.mode(e.c) == 2) f2[0] = g.g30c && e.hit(e.c) ? 1 : 0;
    else f2[0] = g.g30c && e.hit2(e.c, m414, g.g828, g.g824) ? 1 : 0;
    e.cmp60(e.c, e.cmp21(e.c, f2));
    e.twidth(e.c, e.lang(e.c, 0xF93));
    if (e.mode(e.c) != 2) { e.cell(e.c, f2, ebx + 3, g.h - 0x6D, -1, 5, 0); return; }
    if (e.hit(e.c) && g.ada4 == 0) e.draw85(e.c, e.lang(e.c, 0xF93), half(g.w) + 0xB2, g.h - 0x32);
    e.cell(e.c, f2, half(g.w) + 0xA3, g.h - 8, -1, 5, 0);
}
}
