// 1.13c D2Client+0x3E490: stash-grid cell draw entry. EAX = x, stack (ret 0x18) = a0 (int16* item id word), a1, a2, a3 (mode), a4 (y), a5 (flag).
// Resolves the item's per-class record slot (7 slots of 0x22 bytes at D2Client+0xF4970, filled lazily by 0xBF6C0, else the word at +0x11C41C),
// folds the mode word when the mouse (+0x11B828, +0x11B824) is inside the cell [x, x+0x30] x [y-0x30, y], builds the 0x48-byte request for
// 0xD0E4, then draws through 0x3E0C0 (a5 != 0) or 0x3DCF0 and finishes with 0x3D3C0(x, y) with EBX = a2.
#include "d2c_seam.h"
extern "C" void d2c_3e490(const D2cSeam* s, int32_t x, const int16_t* p, uint32_t a1, uint32_t a2, int32_t a3, int32_t y, uint32_t a5,
                          uint8_t* slots, const int32_t* mouse /* {+0x11B824, +0x11B828} */, uint32_t fallback) {
    auto rd = [](const uint8_t* q, uint32_t o) { uint32_t v; std::memcpy(&v, q + o, 4); return v; };
    const int32_t k = (int32_t)(*s)(0xce92, 0, 0, 0, 0, 0, (uint64_t)(int64_t)(int32_t)*p);
    uint32_t sel;
    if (k < 0 || k >= 7) sel = fallback;
    else {
        const uint32_t off = (uint32_t)k * 0x22;
        if (!rd(slots, off)) { const uint32_t v = (uint32_t)(*s)(0xbf6c0, 0, 0, 0, 0, s->mod(0xF4974 + off)); std::memcpy(slots + off, &v, 4); }
        sel = rd(slots, off);
    }
    const uint32_t item = (uint32_t)(*s)(0x17f0, (uint64_t)(int64_t)(int32_t)*p);
    if (!item) return;
    const uint32_t cls = *(const uint8_t*)(uintptr_t)(item + 7);
    const int32_t my = mouse[0], mx = mouse[1];
    if (!(mx < x || mx > x + 0x30 || my < y - 0x30 || my > y)) a3 = a3 == 0 ? 0 : a3 == 1 ? 1 : a3 == 4 ? 4 : 1;
    uint32_t req[18] = {};
    req[0] = cls; req[13] = sel; req[16] = 0;
    (*s)(0xd0e4, 0, 0, 0, 0, 0, (uintptr_t)req, (uint32_t)x, (uint32_t)y, 0xffffffffu, 5);
    const uint64_t r = (*s)(a5 ? 0x3e0c0 : 0x3dcf0, (uint32_t)y, (uint32_t)x, 0, 0, 0, (uintptr_t)p, a1);
    (*s)(0x3d3c0, r, 0, 0, a2, 0, (uint32_t)x, (uint32_t)y);
}
