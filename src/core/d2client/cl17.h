// Shared helpers for the monster-AI movement ports (one file per function, src/core/d2client/d2client_<rva>.cpp).
// Everything the retail code reaches through a call goes through cl::Env::H with the arguments the call site pushes.
#pragma once
#include <cstdint>
#include "cl_env.h"
namespace cl17 {
// unit +0 type; types 2, 4, 5 read the path block (+0x2c) directly, every other type goes through the position thunks
inline bool direct(uint32_t t) { int32_t s = (int32_t)t; return s == 2 || (s > 3 && s <= 5); }
inline uint32_t posX(const cl::Env& e, uint32_t u) {   // inline getter at 0x173F4 / 0x1751F: path+0xC or thunk 0xC3BE
    if (direct(e.R(u))) return e.R(e.R(u + 0x2c) + 0xc);
    uint32_t p = e.R(u + 0x2c);
    return p ? e.H(0xc3be, p) : 0;
}
inline uint32_t posY(const cl::Env& e, uint32_t u) {   // path+0x10 or thunk 0xC2EC
    if (direct(e.R(u))) return e.R(e.R(u + 0x2c) + 0x10);
    uint32_t p = e.R(u + 0x2c);
    return p ? e.H(0xc2ec, p) : 0;
}
// unit LCG: seed = lo * 0x6AC690C5 + hi (64 bit), lo = +0x20, hi = +0x24; returns the new lo
inline uint32_t lcg(const cl::Env& e, uint32_t u) {
    uint64_t m = (uint64_t)e.R(u + 0x20) * 0x6ac690c5u + e.R(u + 0x24);
    e.W(u + 0x24, (uint32_t)(m >> 32)); e.W(u + 0x20, (uint32_t)m);
    return (uint32_t)m;
}
inline uint32_t ptr32(const void* p) { return (uint32_t)(uintptr_t)p; }
// the six-word walk block handed to 0x4DC70: x, y, 0, ord10471(path) [0xD00C], s, ord11058(path)&0xFF [0xD006]
inline uint32_t walk(const cl::Env& e, uint32_t unit, uint32_t path, uint32_t x, uint32_t y, uint32_t s4,
                     uint32_t ecx, uint32_t edxUnit) {
    uint32_t b[6];
    b[0] = x; b[1] = y; b[2] = 0;
    b[3] = e.H(0xd00c, path);
    b[4] = s4;
    b[5] = e.H(0xd006, path) & 0xff;
    (void)unit;
    return e.H(0x4dc70, ptr32(b), 0, 0, 0, 0, 0, ecx, edxUnit);
}
}  // namespace cl17
namespace cl17 {
// 0x4C2C0 (leaf accessor, ECX/EDX preserved): a type-1 unit's data block + 0x34, else 0
inline uint32_t monField34(const cl::Env& e, uint32_t u) {
    if (!u || e.R(u) != 1) return 0;
    uint32_t d = e.R(u + 0x14);
    return d ? d + 0x34 : 0;
}
// the "state word = v" store every idle helper repeats: type-1 unit, data block (+0x14) present -> data[+0x30] = v
inline void setState(const cl::Env& e, uint32_t u, uint32_t v) {
    if (u && e.R(u) == 1) { uint32_t d = e.R(u + 0x14); if (d) e.W(d + 0x30, v); }
}
}  // namespace cl17
