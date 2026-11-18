// 1.13c D2Client+0x958A0, written from the disassembly: the item grid of one inventory panel (each placed item's cell frame and
// background colour, the hovered item's tooltip, then the cursor item's drop marker). One stack argument, the panel's unit (ret 4).
// g = the module globals from 0x11B824 (0x428 bytes: +0 0x11B824, +4 0x11B828, +0x40 0x11B864 mode, +0x3D8 0x11BBFC player,
// +0x404 0x11BC28, +0x410 0x11BC34, +0x414 0x11BC38 hovered item, +0x424 byte 0x11BC48 panel page); gl = the panel layouts at
// 0x10B288 (0x170 bytes; a layout's +4 left, +0xC top, byte +0x14 cell width, byte +0x15 cell height); gc = 0x119854.
// A layout is handed on to its callees by its retail address (0xB2000000 | rva), as module strings are.
#include "cl_env.h"
namespace {
constexpr uint32_t kInvCpp = 0xB20D476Cu, kCmnItems = 0xB20D478Cu;   // "..\\Source\\D2Client\\UI\\inv.cpp", "..\\Source\\D2Common\\CmnItems.h" (module strings)
}
// 1.13c D2Client+0x958A0
extern "C" void d2c_958a0(uint32_t unit, uint32_t g, uint32_t gl, uint32_t gc, const cl::Env& e) {
    e.H(0x908c0);
    const uint32_t self = e.R(g + 0x3d8);
    const uint32_t ty = unit ? e.R(unit) : 6;
    const uint32_t gcv = e.R(gc), page = e.R8(g + 0x424);
    uint32_t lay;
    if (ty == 0) {
        switch (page) {
        case 1: lay = 0x10b370; break;
        case 2: lay = 0x10b288; break;
        case 3: lay = 0x10b3c8; break;
        case 4: lay = gcv ? 0x10b2d0 : 0x10b398; break;
        default: lay = 0x10b3e0; break;
        }
    } else if (ty == 1) lay = 0x10b3b0;
    else return;
    const uint32_t L = gl + (lay - 0x10b288), tag = 0xB2000000u | lay;
    const uint32_t inv = e.R(unit + 0x60);
    if (!inv) return;
    const uint32_t k8a4 = e.H(0xc8a4, unit, page, gcv);   // stack: unit, ECX (CL = the page byte, the upper bytes residue), EDX = [0x119854]
    const auto px = [&](uint32_t x) { return e.R8(L + 0x14) * x + e.R(L + 4); };
    const auto py = [&](uint32_t y) { return e.R8(L + 0x15) * y + e.R(L + 0xc); };
    for (uint32_t cur = e.H(0xc4f0, inv); cur; cur = e.H(0xc568, cur)) {
        const uint32_t it = e.H(0xc526, cur);
        if (!it || e.H(0xc994, cur) != 1) continue;
        const uint32_t x = e.PD(it, 0xc3be, 0xc), y = e.PD(it, 0xc2ec, 0x10);
        uint8_t wh[2] = {};   // retail locals [esp+0x13] (first out) and [esp+0x12] (second out)
        e.H(0xc7f6, it, (uint32_t)(uintptr_t)&wh[0], (uint32_t)(uintptr_t)&wh[1], kCmnItems, 0x6da);
        if ((e.H(0xc92e, it) & 0xff) != page) continue;
        uint32_t flag;
        const bool hover = it == e.R(g + 0x414) && e.R(g + 0x404);
        if (hover) flag = (e.R(g + 0x40) == 8 && e.H(0xc856, it) == 0) ? 0 : 1;
        else {
            flag = 0;
            if (e.H(0xc898, it, self, 0, 0, 0) && !e.H(0xc1a8, it, 4, 0x53d, kInvCpp) && !e.H(0x81cb0, 0, 0, 0, 0, 0, it) && e.H(0xc724, it)) {
                if (!e.H(0xc1a8, it, 0x10, 0x541, kInvCpp)) flag = 4;
                else {
                    const uint32_t row = e.H(0xc1ba, e.R(it + 4));
                    flag = (e.R8(row + 0x12a) && e.H(0x8c370, 0, 0, 0, 0, 0, row)) ? 0 : 2;
                }
            }
        }
        e.H(0x8c600, x, flag, 0, 0, 0, wh[1], wh[0], y);   // AL = second out, CL = first out, EDX = y
        e.H(0x6b5a0, px(x), py(y));
        if (hover && e.H(0xc90a, it) && (e.H(0xc97c, it) & 0xff) && e.H(0xc1a8, it, 0x800, 0x51c, kInvCpp))
            e.H(0x95140, it, tag, px(x), py(y));
    }
    if (e.R(unit) == 1) return;
    if (!e.H(0xc202, e.R(self + 0x60))) return;
    const uint32_t a = e.R(g + 4), c = e.R(g);
    uint32_t r = 1;
    switch (e.R(g + 0x410) - 0xb) {
    case 0:
        if (page == 1) return;
        r = page == 2 ? e.H(0x8c520, 0, 0, 0, 0, 0, a, c) : e.H(0x8c580, 0, 0, 0, 0, 0, a, c);
        break;
    case 1:
    case 2: r = page == 4 ? e.H(0x8c4c0, 0, 0, 0, 0, 0, a, c) : e.H(0x8c580, 0, 0, 0, 0, 0, a, c); break;
    case 3: r = page == 3 ? e.H(0x8c440, 0, 0, 0, 0, 0, a, c) : e.H(0x8c580, 0, 0, 0, 0, 0, a, c); break;
    default: break;
    }
    if (r) e.H(0x8e440, inv, k8a4, tag);
}
