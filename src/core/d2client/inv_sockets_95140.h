// Socket overlay of an item the cursor is over: where the sockets sit on its footprint.
// 1.13c D2Client+0x95140 (draw), data at D2Client+0xD0A98.
#pragma once
#include <stdint.h>

namespace d2client_sockets {
// 1.13c D2Client+0xD0A98: 48 entries of six (dx, dy) int32 pairs, indexed
// (height + 4 * (width + 2 * sockets) - 13); only the first `sockets` pairs of an entry are read.
static const int32_t kFootprint[48][12] = {
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 2, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 3, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 2, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 2, 2, 6, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 3, 1, 5, 0, 0, 0, 0, 0, 0},
  {1, 2, 1, 4, 1, 6, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 3, 1, 2, 3, 0, 0, 0, 0, 0, 0},
  {2, 1, 2, 3, 2, 5, 0, 0, 0, 0, 0, 0},
  {2, 2, 2, 4, 2, 6, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 3, 1, 5, 1, 7, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 3, 3, 1, 3, 3, 1, 0, 0, 0, 0},
  {1, 2, 3, 4, 1, 4, 3, 2, 0, 0, 0, 0},
  {2, 1, 2, 3, 2, 5, 2, 7, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 3, 1, 1, 3, 5, 1, 5, 3, 1, 0, 0},
  {2, 4, 1, 2, 3, 6, 1, 6, 3, 2, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 3, 1, 5, 3, 1, 3, 3, 3, 5},
  {1, 2, 1, 4, 1, 6, 3, 2, 3, 4, 3, 6},
};
// Retail draws on a 29 px cell (the grid record bytes +0x14/+0x15); the offsets below are what it adds
// to the item's drawing anchor: ((dx - 1) * cellW) / 2 and ((dy + 1) * cellH) / 2, truncating toward zero.
static const int32_t kCell = 29;
// Fills off[n] = {x, y} for n < sockets; returns false for a shape the table has no entry for.
static inline bool layout(int32_t sockets, int32_t w, int32_t h, int16_t (*off)[2]) {
    if (sockets < 1 || sockets > 6 || w < 1 || w > 2 || h < 1 || h > 4) return false;
    const int32_t* e = kFootprint[h + 4 * (w + 2 * sockets) - 13];
    for (int32_t n = 0; n < sockets; ++n) {
        off[n][0] = (int16_t)(((e[n * 2] - 1) * kCell) / 2);
        off[n][1] = (int16_t)(((e[n * 2 + 1] + 1) * kCell) / 2);
    }
    return true;
}
}  // namespace d2client_sockets
