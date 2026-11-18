// 1.13c D2Client+0xA3840, written from the disassembly: draw a list/menu window. ESI = window block w, no stack arguments, plain ret.
// Callees (thunk RVAs): 0xD0BA fill rect (stdcall 6), 0xBE4C0 border (EAX = {l,t,r,b}), 0xD096 cel draw (stdcall 6, s0 = 18-dword
// draw context), 0xD3A8 font select (ECX), 0xD37E text (ECX = row+0x68, EDX = x, stdcall y/colour/0). Both cel draws of the
// selection marker pass the SAME context block (the decompile's second &uStack_60 is the same stack slot). The pinned module
// globals ride in e.gl: +0 selection cel (0x6FBCB9C8), +4 marker tick (0x6FBBB20C). A list node's object vtable slot 1 is a code
// pointer base+rva (0x6FAB0000 + rva); ECX = the object.
#include "cl_env.h"
extern "C" void d2cl_a3840(const cl::Env& e, uint32_t w) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    if (!w) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0x499)); e.H(0x3664, 0xffffffffu); return; }
    const uint32_t g = e.gl;
    uint32_t blk[18] = {};
    const uint32_t x = e.R(w + 0x2c), y = e.R(w + 0x30), h = e.R(w + 0x38);
    e.W(w + 0x54, e.R(w + 0x54) + 1);
    const uint32_t right = e.R(w + 0x34) + x - 1;
    uint32_t rect[4] = {x, y, right, y + h - 1};
    const auto draw = [&](uint32_t px, uint32_t py) {
        e.H(0xd096, P(blk), px, py, 0xffffffffu, 5);
        if (e.blk) e.blk(e.ctx, 0xd096, P(blk), 18);
    };
    if (e.R(w + 0xc) == 0) {
        e.Xs(1);
        e.H(0xd0ba, x, y, right + 1, y + h, 0);
        e.H(0xbe4c0, 0, 0, 0, 0, 0, P(rect));
        if (e.blk) e.blk(e.ctx, 0xbe4c0, P(rect), 4);
    } else if (e.R(w + 8)) {
        blk[13] = e.R(w + 8);
        draw(x, y + h);
    }
    for (uint32_t n = e.R(w + 0xb08); n; n = e.R(n + 4)) {
        const uint32_t o = e.R(n);
        if (o) e.H(e.R(e.R(o) + 4) - 0x6FAB0000u, 0, 0, 0, 0, 0, 0, o);
    }
    const uint32_t font = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 1);
    uint32_t acc = 0;
    const int32_t rows = (int32_t)e.R(w + 0x50);
    for (int32_t i = 0; i < rows; ++i) {
        const uint32_t it = w + 0x168 + 0x110 * (uint32_t)i;
        uint32_t col = e.R(it + 4);
        e.H(0xd3a8, 0, 0, 0, 0, 0, 0, e.R(it));
        const uint32_t rx = e.R(it - 0xc) + e.R(w + 0x2c);
        const uint32_t ry = e.R(it - 0x10) + acc + e.R(w + 0x30);
        if (e.R(w + 0x44) == (uint32_t)i) {
            const uint32_t mode = e.R(w + 0x5c);
            if (mode == 1) col = 3;
            if (mode == 2) {
                const uint32_t tick = e.R(g + 4);
                blk[13] = e.R(g);
                blk[0] = (uint32_t)((int32_t)tick % 7);
                draw(rx - 0x18, ry + 4);
                draw(e.R(it - 8) + rx + 2, ry + 4);
                e.W(g + 4, tick + 1);
            }
        }
        e.H(0xd37e, ry, col, 0, 0, 0, 0, it - 0x100, rx);
        acc += e.R(it - 0x10);
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, font);
}
