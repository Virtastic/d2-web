// 1.13c D2Client+0xBCEA0: the colour and resist rules of one stats-panel cell, shared by the
// port and the browser exports so there is one copy.
#pragma once
#include <cstdint>
namespace d2cl_cell {
// colour 3 (blue) above the base, 1 (red) below, else the colour already set
static inline uint32_t cmpColour(int32_t v, int32_t base, uint32_t col) {
    if (v > base) return 3;
    if (v < base) return 1;
    return col;
}
// resist cell: `adj` is the signed difficulty/penalty already chosen by the caller; `maxStat` is the max-resist stat.
// Returns the colour; the drawn value is left in *val.
static inline uint32_t resistCell(int32_t v, int32_t maxStat, uint32_t col, int32_t adj, int32_t* val) {
    int32_t cap = maxStat;
    v = (int32_t)((uint32_t)v + (uint32_t)adj);
    cap = (int32_t)((uint32_t)cap + 0x4b); if (cap >= 0x5f) cap = 0x5f;
    if (v <= -100) v = -100;
    if (v >= cap) {
        v = cap;
        if (col != 3) { *val = v; return 4; }
    }
    if (v < 0) col = 1;
    *val = v;
    return col;
}
}
