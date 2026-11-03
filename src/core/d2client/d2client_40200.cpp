// 1.13c D2Client+0x40200: random-walk path generator. ECX = unit, EDX = seed word (fastcall, no stack args). Clamps the unit's path to 0x4D
// nodes, seeds the unit's 64-bit LCG (unit+0x20) through 0xC3B2, then writes one int16 {x, y} pair per node into the path buffer the path
// object hands back through 0xCD18: the direction steps through the table pair at D2Client+0xD64E0 (Y, 8 dwords) / +0xD6500 (X), turning
// sign and magnitude (global +0xF9F64 plus rand%3) every +0xF9F68 nodes. The start point is the path object's +0xC / +0x10 for unit types 2
// and 4..5, else 0xC3BE / 0xC2EC of the path object (0 when the unit has none).
#include <utility>
#include "d2c_seam.h"
extern "C" void d2c_40200(const D2cSeam* s, uint8_t* u, uint32_t seedArg, const uint8_t* dirTbl, int32_t mag0, int32_t stepMod) {
    if (!u) return;
    auto rd = [](const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; };
    int32_t n = (int32_t)(*s)(0xc430, 0, 0, 0, 0, 0, (uintptr_t)u);
    if (n >= 0x4e) { (*s)(0xc4a2, 0, 0, 0, 0, 0, (uintptr_t)u, 0x4d); (*s)(0xc2c8, 0, 0, 0, 0, 0, (uintptr_t)u, 0x4d); n = 0x4d; }
    const uint32_t h = rd(u, 0x2c);
    (*s)(0xc3b2, 0, (uintptr_t)(u + 0x20), seedArg);
    (*s)(0xc38e, 0, 0, 0, 0, 0, h, 10);
    (*s)(0xc3b8, 0, 0, 0, 0, 0, h, (uint32_t)n);
    uint32_t pb = 0;
    (*s)(0xcd18, 0, 0, 0, 0, 0, h, (uintptr_t)&pb);
    const int32_t t = (int32_t)rd(u, 0);
    const bool direct = t == 2 || (t > 3 && t <= 5);
    int32_t x = direct ? (int32_t)rd((const uint8_t*)(uintptr_t)rd(u, 0x2c), 0xc) : (rd(u, 0x2c) ? (int32_t)(*s)(0xc3be, 0, 0, 0, 0, 0, rd(u, 0x2c)) : 0);
    int32_t y = direct ? (int32_t)rd((const uint8_t*)(uintptr_t)rd(u, 0x2c), 0x10) : (rd(u, 0x2c) ? (int32_t)(*s)(0xc2ec, 0, 0, 0, 0, 0, rd(u, 0x2c)) : 0);
    const uint32_t hh = rd(u, 0x2c);
    const uint32_t d1 = (uint32_t)(*s)(0xc340, 0, 0, 0, 0, 0, rd(u, 0x2c));
    const uint32_t d2 = (uint32_t)(*s)(0xc31c, 0, 0, 0, 0, 0, rd(u, 0x2c), d1);
    uint32_t dir = (uint32_t)(*s)(0xcc82, 0, 0, 0, 0, 0, (uintptr_t)u, d2) & 0xff;
    auto next = [&]() {   // 64-bit LCG {lo,hi} at unit+0x20: lo' = (lo*M + hi) lo word, and the high word chain exactly as the asm's mul/adc pair
        uint32_t lo = rd(u, 0x20), hi = rd(u, 0x24);
        uint64_t v = (uint64_t)lo * 0x6ac690c5u + hi;
        const uint32_t lo1 = (uint32_t)v, hi1 = (uint32_t)(v >> 32);
        uint64_t w = (uint64_t)lo1 * 0x6ac690c5u + hi1;
        std::memcpy(u + 0x20, &w, 8);
        return std::pair<uint32_t, uint32_t>(lo1, (uint32_t)w);
    };
    auto step = [&]() {   // the in-loop re-roll: one step, {lo,hi} = lo*M + hi
        uint64_t v = (uint64_t)rd(u, 0x20) * 0x6ac690c5u + rd(u, 0x24);
        std::memcpy(u + 0x20, &v, 8);
        return (uint32_t)v;
    };
    auto r0 = next();
    int32_t sgn = (int32_t)(r0.first & 1) * 2 - 1;
    int32_t mag = (int32_t)(r0.second % 3u) + mag0;
    for (int32_t i = 0; i < n; i++) {
        dir += (uint32_t)(mag * sgn);
        const uint32_t k = (uint32_t)(*s)(0xcc88, 0, 0, 0, 0, 0, dir & 0xffffff3fu) & 0xff;
        x += (int32_t)rd(dirTbl, 0x20 + k * 4); y += (int32_t)rd(dirTbl, k * 4);
        uint8_t* b = (uint8_t*)(uintptr_t)pb;
        const int16_t px = (int16_t)x, py = (int16_t)y;
        std::memcpy(b + i * 4, &px, 2); std::memcpy(b + i * 4 + 2, &py, 2);
        if (i % stepMod == 0) { sgn = -sgn; mag = (int32_t)(step() % 3u) + mag0; }
    }
    (*s)(0xce86, 0, 0, 0, 0, 0, hh, (uint32_t)n);
}
