// 1.13c D2Client+0x50100: draw the bottom hint text and its frame when the screen mode (D2gfx 10031) is 2.
// No arguments, no result. Reads the globals W = 0xDBC48, H = 0xDBC4C (screen size), A = 0xFADA4 (a suppress
// flag) and C = 0x11C300 (the frame's cell word). Seams: D2gfx 10031 (mode), 0x4FF10 (mouse inside the hint rect), D2Lang 10003 (ecx = id),
// D2Win 10085 (ecx = string, edx = x, stack y, 0, 1) and D2gfx 10041 (frame record, x, y, -1, 5, 0).
#include <cstdint>
namespace d2c50100 {
struct G { int32_t w, h; uint32_t flag, cell; };
struct Env {
    void* c;
    uint32_t (*mode)(void*);                                            // D2gfx 10031
    uint32_t (*hit)(void*);                                             // 0x4FF10
    const void* (*str)(void*, uint32_t id);                             // D2Lang 10003
    void (*text)(void*, const void* s, int32_t x, int32_t y);           // D2Win 10085 (stack: y, 0, 1)
    void (*cell)(void*, const uint32_t* frame, int32_t x, int32_t y, int32_t color, int32_t drawMode, int32_t pal);   // D2gfx 10041
};
static int32_t half(int32_t v) { return (v - (v >> 31)) >> 1; }   // cdq; sub eax,edx; sar eax,1
// 1.13c D2Client+0x50100
void sub_50100(const Env& e, const G& g) {
    if (e.mode(e.c) != 2) return;
    uint32_t f[18];
    for (int i = 0; i < 18; i++) f[i] = 0;
    f[13] = g.cell;
    f[0] = 2;
    if (e.hit(e.c) && g.flag == 0) {
        const int32_t y = g.h - 0x32;
        const void* s = e.str(e.c, 0xF92);
        e.text(e.c, s, half(g.w) - 0xB3, y);
    }
    e.cell(e.c, f, half(g.w) - 0xC2, g.h - 8, -1, 5, 0);
}
}
