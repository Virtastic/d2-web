// 1.13c D2Client+0x67FD0 -- a list control's draw method (ECX = the control).
// The control is a large object: +0 its method table, +0x8 the rows (stride 0x300E: six 0x400-wchar strings, then a colour
// dword and a font dword), and from +0x180708 its layout words. Module globals read: 0x1154EC (the cel record's +0x34), the
// two words at 0x11B824 / 0x11B828 (pointer position). Every callee goes through the Env.
#include <cstring>
#include "cl_cmd.h"
namespace {
constexpr uint32_t kX0 = 0x180708, kY0 = 0x18070C, kW = 0x180710, kH = 0x180714, kUp = 0x180720, kDown = 0x180724,
    kFirst = 0x180730, kSel = 0x180738, kFont = 0x180744, kHl = 0x18074A, kBx = 0x18075A, kBy = 0x18075E, kN = 0x180762,
    kLine = 0x180766, kGap = 0x18076A, kHit = 0x18076E, kTick = 0x180772;
}
extern "C" void d2c_67fd0(uint32_t self, const cl::Env& e) {
    using clc::G;
    const int32_t x0 = (int32_t)e.R(self + kX0), y0 = (int32_t)e.R(self + kY0);
    uint32_t clip[4] = { (uint32_t)x0, (uint32_t)y0, e.R(self + kW) + (uint32_t)x0 - 1, e.R(self + kH) + (uint32_t)y0 - 1 };
    e.H(0xbe4c0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)clip);
    if (e.blk) e.blk(e.ctx, 0xbe4c0, (uint32_t)(uintptr_t)clip, 4);
    const uint32_t saved = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, e.R(self + kFont));
    uint32_t rec[18] = {};
    const uint32_t p = (uint32_t)(uintptr_t)rec;
    rec[13] = G(e, 0x1154EC);
    auto draw = [&](uint32_t y) {
        e.H(0xd096, p, e.R(self + kBx), y, 0xffffffffu, 0, 5, 0);
        if (e.blk) e.blk(e.ctx, 0xd096, p, 18);
    };
    rec[0] = (e.R(self + kUp) != 0) * 2u + 9u;
    draw(e.R(self + kBy));
    int32_t i = 0;
    uint32_t off = 0;
    while (i < (int32_t)e.R(self + kN)) {
        rec[0] = 0xd;
        draw(off + e.R(self + kBy) + 0xc);
        ++i; off += 0xc;
    }
    rec[0] = (e.R(self + kDown) != 0) * 2u + 8u;
    draw(e.R(self + kBy) + (uint32_t)i * 0xc);
    const uint32_t cnt = e.H(0x67bf0, 0, 0, 0, 0, 0, 0, 0, self);
    int32_t idx = (int32_t)e.R(self + kFirst);
    uint32_t k = 0, j = 0;
    if (idx < (int32_t)((uint32_t)idx + cnt)) {
        uint32_t row = (uint32_t)idx * 0x300eu + self + 0x3012u;
        while (idx < 0x80) {
            uint32_t col = e.R(row - 4);
            if (e.R(self + kSel) == (uint32_t)idx && e.R(self + kHl) != 0) col = 3;
            uint32_t font = e.R(row);
            if (font == 0xe) font = e.R(self + kFont);
            e.H(0xd3a8, 0, 0, 0, 0, 0, 0, font);
            static const uint32_t so[6] = { 0x300a, 0x280a, 0x200a, 0x180a, 0x100a, 0x80a };
            for (int s = 0; s < 6; ++s) {
                const uint32_t str = row - so[s];
                if ((e.R(str) & 0xffffu) == 0) continue;
                const uint32_t y = e.R(self + kGap) * k + e.R(self + kLine) * j + e.R(self + kY0) + 0x19;
                e.H(0x67af0, str, e.R(self + kX0) + 0x20, y, col, 0, 0, 0, 0, self);
                ++j;
            }
            row += 0x300e; ++k; ++idx;
            if (!(idx < (int32_t)(e.R(self + kFirst) + cnt))) break;
        }
    }
    e.H(e.R(e.R(self) + 0x30) & 0xFFFFFFu, 0, 0, 0, 0, 0, 0, self);
    const uint32_t my = G(e, 0x11B824) & 0xffffu, mx = G(e, 0x11B828) & 0xffffu;
    uint32_t mb[4] = { 0, 0, 0, (my << 16) | mx };
    e.W(self + kTick, e.R(self + kTick) + 1);
    const int32_t bx = (int32_t)e.R(self + kBx), by = (int32_t)e.R(self + kBy);
    const int32_t X = (int32_t)mx, Y = (int32_t)my;
    auto hit = [&](uint32_t slot) {   // the click-repeat counter: every tenth tick (and the wrap to 0) is skipped
        const uint32_t c = e.R(self + kHit) + 1;
        e.W(self + kHit, c);
        if (c != 0 && c % 10 != 0) {
            const uint32_t t = e.R(e.R(self) + slot) & 0xFFFFFFu;
            e.H(t, (uint32_t)(uintptr_t)mb, 0, 0, 0, 0, 0, self);
            if (e.blk) e.blk(e.ctx, t, (uint32_t)(uintptr_t)mb, 4);
        }
        return c != 0 && c % 10 != 0;
    };
    if (X > bx) {
        if (X < bx + 0x10 && Y > by - 0xc && Y < by && e.R(self + kUp) == 1) {
            if (hit(0x1c)) { e.H(0xd3a8, 0, 0, 0, 0, 0, 0, saved); return; }
        } else if (X < bx + 0x10) {
            const int32_t ey = (int32_t)(uint32_t)(by + (int32_t)e.R(self + kN) * 0xc);
            if (Y > ey - 0xc && Y < ey && e.R(self + kDown) == 1) hit(0x20);
        }
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, saved);
}
