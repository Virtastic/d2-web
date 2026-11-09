// Shared prefix of D2Client 0x7DAA0 / 0x7DC00 (the two unit name-label drawers), from the 1.13c disassembly.
// ECX = unit; id = [unit+0xA4]; the screen position (0xC21A, stdcall unit, &pos) is shifted by the pinned label offsets
// (0x113AD4 x, 0x113AD8 y) and the mode word 0x11C414 (1: left a quarter of the width, 2: right, 3: nothing); the label
// must lie on screen (0xDBC48 w, 0xDBC4C h; signed). `dy` is the constant added to y (-10 / -0x1E).
#pragma once
#include <cstdint>
#include "cl_env.h"
namespace cl {
inline bool labelPos(const Env& e, uint32_t unit, uint32_t ga, uint32_t gm, uint32_t gs, int32_t dy, int32_t pos[2], uint32_t& id) {
    if (!unit) return false;
    id = e.R(unit + 0xa4);
    if (!id) return false;
    e.H(0xc21a, unit, (uint32_t)(uintptr_t)pos);
    int32_t y = pos[1] + (dy - (int32_t)e.R(ga + 4));
    int32_t x = pos[0] - (int32_t)e.R(ga);
    const int32_t w = (int32_t)e.R(gs);
    const uint32_t m = e.R(gm);
    if (m == 3) { pos[0] = x; pos[1] = y; return false; }
    const int32_t q = (int32_t)(w + ((uint32_t)(w >> 31) & 3u)) >> 2;
    if (m == 1) x += -q; else if (m == 2) x += q;
    pos[0] = x; pos[1] = y;
    return y > 0 && y < (int32_t)e.R(gs + 4) && x > -100 && x < (int32_t)((uint32_t)w + 100u);
}
// the shared tail: 200-wchar scratch (the Unicode default ctor 0xCEB00 only zeroes a wchar: not a recorded call), strlen/clamp 199, strncpy, then 0x7CA40(eax=&pos, ecx=id, edx=unit, [buf])
inline void labelDraw(const Env& e, uint32_t unit, uint32_t id, int32_t pos[2], uint32_t src, uint32_t n, uint16_t* buf, uint32_t cap) {
    for (uint32_t i = 0; i < cap; ++i) buf[i] = 0;
    uint32_t len = e.H(0xceab0, 0, 0, 0, 0, 0, 0, src);
    if ((int32_t)len >= (int32_t)cap) len = cap - 1;
    e.H(0xceaa8, len, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)buf, src);
    (void)n;
    e.H(0x7ca40, (uint32_t)(uintptr_t)buf, 0, 0, 0, 0, (uint32_t)(uintptr_t)pos, id, unit);
    if (e.blk) e.blk(e.ctx, 0x7ca40, (uint32_t)(uintptr_t)pos, 2);
}
}  // namespace cl
