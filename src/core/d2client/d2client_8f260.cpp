// 1.13c D2Client+0x8F260 -- the panel-4 hover tooltip (no arguments; cdecl). Gate: mouse x (0x10B2A4) in [0x10B408, 0x10B40C] and mouse y
// (0x10B430) in [y0, 0x10B414], y0 = 0x11B9A4 + screenH(0xDBC4C) - 0x6D. Then the first of the 0x0E11C8 buttons (stride 20 from 0x10B238) whose
// state (dword +0) is 1, whose x window (left = table[0xE11D0 + (i + 4 * 0xE11C4) * 4] + 0x11B9A0) holds x strictly (left < x < left + 0x2D) and whose
// y is strictly between y0 and y1 = 0x11B9A4 + screenH - 0x41 gets the tooltip; no other button is looked at. Id word (+8): 0x276F draws
// sprintf(256 chars, buf, D2Lang(0x276F)) with 0x46150's record (+4) and the player (0x11BBFC), 0xC99A(0, self, rec4, byte 0x11C390, 0x11973B, 0),
// nothing when either is null; else it draws the D2Lang string of the id word. The draw is D2Win 0xD384(ECX text, EDX left; stack y - 100, 0, 0).
// (The Unicode ctor loop and sprintf are import slots the capture pins; they leave no call record.)
#include "cl_env.h"
namespace {
constexpr uint32_t G = 0x6FAB0000u;
}
extern "C" void d2c_8f260(const cl::Env& e) {
    const int32_t mx = (int32_t)e.R(G + 0x10b2a4);
    if (mx < (int32_t)e.R(G + 0x10b408) || mx > (int32_t)e.R(G + 0x10b40c)) return;
    const int32_t sh = (int32_t)e.R(G + 0xdbc4c), b4 = (int32_t)e.R(G + 0x11b9a4);
    const int32_t y0 = b4 + sh - 0x6d;
    int32_t my = (int32_t)e.R(G + 0x10b430);
    if (my < y0 || my > (int32_t)e.R(G + 0x10b414)) return;
    const int32_t n = (int32_t)e.R(G + 0xe11c8);
    for (int32_t i = 0; i < n; i++) {
        if (e.R(G + 0x10b238 + (uint32_t)i * 20) != 1) continue;
        const int32_t left = (int32_t)e.R(G + 0xe11d0 + (uint32_t)(i + (int32_t)e.R(G + 0xe11c4) * 4) * 4) + (int32_t)e.R(G + 0x11b9a0);
        if (mx <= left || mx >= left + 0x2d) continue;
        if ((int32_t)e.R(G + 0x10b430) <= y0 || (int32_t)e.R(G + 0x10b430) >= (int32_t)e.R(G + 0x11b9a4) + (int32_t)e.R(G + 0xdbc4c) - 0x41) continue;
        const uint32_t id = e.R(G + 0x10b240 + (uint32_t)i * 20) & 0xffff;
        const uint32_t y = (uint32_t)((int32_t)e.R(G + 0x11b9a4) + (int32_t)e.R(G + 0xdbc4c) - 0x64);
        if (id == 0x276f) {
            volatile char16_t buf[0x100];
            for (int k = 0; k < 0x100; k++) buf[k] = 0;
            const uint32_t self = e.R(G + 0x11bbfc);
            const uint32_t rec = e.H(0x46150);
            if (!self || !rec) return;
            e.Xs(0);
            e.H(0xc99a, 0, self, e.R(rec + 4), e.R8(G + 0x11c390), e.R(G + 0x11973b));
            e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x276f);
            const uint32_t b = (uint32_t)(uintptr_t)buf;
            e.H(0xd384, y, 0, 0, 0, 0, 0, b, (uint32_t)left);
            return;
        }
        const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id);
        e.H(0xd384, y, 0, 0, 0, 0, 0, s, (uint32_t)left);
        return;
    }
}
