// 1.13c D2Client+0x8E440: place the cursor item into the hovered inventory grid. EBX.bl = mode (0xFF: none), EDI = unit,
// three stack words a1 a2 a3 (ret 0xC). Globals: 11BC28 (cursor-item flag), 11BC34 (open panel), 11B828/11B824 (mouse x/y),
// E0EB8/E0EBC (grid cell x/y), DBC48/DBC4C (screen width/height). Callees: D2Common 10114 (thunk C7F6: item cell size into two
// byte out-pointers, CmnItems.h:0x6DA), 11148 (C952, 8 words), 10967 (C9A6, 8 words: the compiler hoists the `push 1` meant for
// 0x8C600 as its last word), 10695 (C1BA), D2Client 0x1660/0x1630 (ECX = item) and 0x8C600 (AL, CL, EDX, ESI + two words).
// Byte locals: w (cell width, CL) and h (AL); the third path reuses the a1/a2 argument slots as 10114's out-bytes.
#include <cstdint>
#include "cl_env.h"
namespace {
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
}
extern "C" void d2c_8e440(uint32_t mode, uint32_t unit, uint32_t a1, uint32_t a2, uint32_t a3, const cl::Env& e) {
    if ((mode & 0xFFu) == 0xFFu || !unit || !e.R(G(0x11BC28))) return;
    volatile uint8_t sz[4] = {};                 // [2] = height (AL), [3] = width (CL): 10114 fills them
    volatile uint32_t found = 0;                 // C9A6's out-pointer (the item under the cursor)
    volatile uint32_t o8 = 0, oc = 0;            // C952's two out-pointers
    volatile uint32_t s1 = a1, s2 = a2;          // the argument slots the third path hands to 10114
    e.H(0xC7F6, unit, P(&sz[3]), P(&sz[2]), G(0xD478C), 0x6DA);
    const int32_t x = (int32_t)e.R(G(0xE0EB8));
    if (x < 0 || (int32_t)e.R(G(0xE0EBC)) < 0) return;
    const int32_t panel = (int32_t)e.R(G(0x11BC34)), half = (int32_t)e.R(G(0xDBC48)) / 2;
    if (panel == 1 && (int32_t)e.R(G(0x11B828)) < half) return;
    if ((int32_t)e.R(G(0x11B824)) >= (int32_t)e.R(G(0xDBC4C)) - 0x27) return;
    if (panel == 0x13 && (int32_t)e.R(G(0x11B828)) < half) return;
    const uint32_t y = e.R(G(0xE0EBC));
    {
        const uint32_t w[8] = { a1, unit, (uint32_t)x, y, a2, P(&oc), P(&o8), mode };
        const uint32_t r = e.H(0xC952, w[0], w[1], w[2], w[3], w[4]);
        if (e.blk) e.blk(e.ctx, 0xC952, P(w), 8);
        if (r) { e.H(0x8C600, e.R(G(0xE0EB8)), 1, 0, 0, 0, sz[2], sz[3], e.R(G(0xE0EBC)), 0, a3); return; }
    }
    const uint32_t x2 = e.R(G(0xE0EB8)), y2 = e.R(G(0xE0EBC));
    {
        const uint32_t w[8] = { a1, unit, x2, y2, a2, P(&found), mode, 1 };
        const uint32_t r = e.H(0xC9A6, w[0], w[1], w[2], w[3], w[4]);
        if (e.blk) e.blk(e.ctx, 0xC9A6, P(w), 8);
        if (!r && (!found || e.R(e.H(0xC1BA, e.R(found + 4)) + 0x80) != 0x20786F62u)) {   // 'box '
            e.H(0x8C600, e.R(G(0xE0EB8)), 0, 0, 0, 0, sz[2], sz[3], e.R(G(0xE0EBC)), 0, a3);
            return;
        }
    }
    e.H(0xC7F6, found, P(&s1), P(&s2), G(0xD478C), 0x6DA);
    const uint32_t cx = e.H(0x1660, 0, 0, 0, 0, 0, 0, found);
    const uint32_t cy = e.H(0x1630, 0, 0, 0, 0, 0, 0, found);
    e.H(0x8C600, cy, 3, 0, 0, 0, s2 & 0xFFu, s1 & 0xFFu, cx, 0, a3);
}
