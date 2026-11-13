// Unit position reads shared by the open-f1 ports: types 2 and 4..5 are static (position straight out of the path record at
// unit+0x2c), others go through the path getters A454 (x) and A3B8 (y), or read 0 when the path is missing.
#pragma once
#include "d2game_seam4e.h"
static inline bool f1Static(uint32_t t) { const int32_t s = (int32_t)t; return s == 2 || (s > 3 && s <= 5); }
static inline uint32_t f1PosX(uint32_t unit, const D2Seam* e) {
    const uint32_t path = d2rd32(unit + 0x2c);
    if (f1Static(d2rd32(unit))) return d2rd32(path + 0xc);
    return path ? e->call(0xA454, 0, 0, 0, 0, 0, path) : 0;
}
static inline uint32_t f1PosY(uint32_t unit, const D2Seam* e) {
    const uint32_t path = d2rd32(unit + 0x2c);
    if (f1Static(d2rd32(unit))) return d2rd32(path + 0x10);
    return path ? e->call(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
}
