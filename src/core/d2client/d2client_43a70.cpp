// 1.13c D2Client+0x43A70, written from the disassembly: the end-of-game ("Congratulations") screen draw. One stack word (unused), no register input,
// returns 0 or 1. Callees go through Env::H with retail's registers and stack words; the module globals are pinned blobs:
//   gA = 0x123488 {+0 endgameok sheet, +4 menu sheet}, gB = 0x119854 (hi-res flag), gC = 0xDBC48 {+0 width, +4 height}, gD = 0x11C410 (button down),
//   gE = 0x11C38C (leave), gF = 0xF8C9C (cleared on leaving).
// 0xD0A2() nonzero skips the draw. Else 0xD0A8(1,0,0,0) zero returns 0. The menu sheet (0x12348C) is loaded once through three 0x3146 name formats, 0x2B420
// and 0xD396 (the 8/9-byte name copy out of .rdata is a pure local and is not observable), the endgameok sheet (0x123488) through one. A 0x48-byte cel block
// (frame at +0, sheet at +0x34) is drawn by 0xD096 (ptr, x, y, -1, 5, 0): 8 tiles of the menu sheet laid out from the screen size, or 12 on the fixed 4x3 grid
// when 0x119854 is set; then 0xD3A8(3), 0x43710, 0x42C70, 0xD3A8(1), the endgameok button (frame = 0x11C410 != 0) at (W/2-0x30, H-0x50), the text 0x13EE
// through 0x42C10 / 0xD1EC / 0xD37E, 0x16A90 and 0xD08A. If 0x11C38C is set both sheets are released (0xC18A, 0xBF86), the slots and 0xF8C9C cleared, returns 1.
#include "cl_env.h"
namespace {
inline uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
}
// 1.13c D2Client+0x43A70
extern "C" uint32_t d2c_43a70(uint32_t gA, uint32_t gB, uint32_t gC, uint32_t gD, uint32_t gE, uint32_t gF, const cl::Env& e) {
    const uint32_t kStr = 0x6fb8357c;
    if (!e.H(0xd0a2)) {
        if (!e.H(0xd0a8, 1, 0, 0, 0, 0, 0)) return 0;
        if (!e.R(gA + 4)) {
            char nm[16] = {}; char b1[0x100] = {}, b2[0x100] = {}, b3[0x100] = {};
            e.H(0x3146, P(b1), 0x6fb841c4, kStr, P(nm));
            e.W(gA + 4, e.H(0x2b420, 0, 0, 0, 0, 0, P(b1)));
            e.H(0x3146, P(b2), 0x6fb86194, kStr, P(nm));
            e.H(0x3146, P(b3), 0x6fb8617c, kStr, P(nm));
            e.H(0xd396, P(b2), P(b3));
        }
        if (!e.R(gA)) {
            char b1[0x100] = {};
            e.H(0x3146, P(b1), 0x6fb86164, kStr);
            e.W(gA, e.H(0x2b420, 0, 0, 0, 0, 0, P(b1)));
        }
        uint32_t b[18] = {};
        b[13] = e.R(gA + 4);
        e.H(0xd3a2);
        const auto draw = [&](uint32_t frame, uint32_t x, uint32_t y) {
            b[0] = frame;
            e.H(0xd096, P(b), x, y, 0xffffffffu, 5);
            if (e.blk) e.blk(e.ctx, 0xd096, P(b), 18);
        };
        const int32_t W = (int32_t)e.R(gC), H = (int32_t)e.R(gC + 4);
        if (!e.R(gB)) {
            const uint32_t hw = (uint32_t)(W / 2);
            draw(0, 0, 0x100); draw(1, 0x100, 0x100);
            draw(2, 0, (uint32_t)H - 1); draw(3, 0x100, (uint32_t)H - 1);
            draw(4, hw, 0x100); draw(5, hw + 0x100, 0x100);
            draw(6, hw, (uint32_t)H); draw(7, hw + 0x100, (uint32_t)H);
        } else {
            static const uint32_t yy[3] = {0x100, 0x200, 0x258};
            for (uint32_t row = 0; row < 3; ++row)
                for (uint32_t col = 0; col < 4; ++col) draw(row * 4 + col, col * 0x100, yy[row]);
        }
        e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 3);
        e.H(0x43710);
        e.H(0x42c70);
        e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 1);
        const uint32_t down = e.R(gD);
        uint32_t b2[18] = {};
        b2[13] = e.R(gA);
        b2[0] = down != 0;
        e.H(0xd096, P(b2), (uint32_t)(W / 2 - 0x30), (uint32_t)(H - 0x50), 0xffffffffu, 5);
        if (e.blk) e.blk(e.ctx, 0xd096, P(b2), 18);
        // EAX is still W/2 here (the push of it is the second stack word); the hook logs EAX
        uint32_t esi = e.H(0x42c10, 0x13ee, (uint32_t)(W / 2), 0, 0, 0, (uint32_t)(W / 2));
        const uint32_t k = e.R(gD) ? 2u : 0u;
        const uint32_t y = (uint32_t)H + k - 0x58;
        esi -= k;
        const uint32_t t = e.H(0xd1ec, y, 0, 0, 0, 0, 0, 0x13ee);
        e.H(0xd37e, 0, 0, 0, 0, 0, 0, t, esi);
        e.H(0x16a90);
        e.H(0xd08a);
    }
    if (!e.R(gE)) return 0;
    uint32_t s = e.R(gA + 4);
    if (s) {
        e.H(0xc18a, s);
        e.H(0xbf86, 0x7a, 0, 0, 0, 0, 0, s, 0x6fb86ab0);
        e.W(gA + 4, 0);
    }
    s = e.R(gA);
    if (s) {
        e.H(0xc18a, s);
        e.H(0xbf86, 0x7a, 0, 0, 0, 0, 0, s, 0x6fb86ab0);
        e.W(gA, 0);
    }
    e.W(gF, 0);
    return 1;
}
