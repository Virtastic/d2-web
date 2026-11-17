// The grid cell a mouse point lands on, as the inventory grid click handler decides it.
// 1.13c D2Client+0x96AA0 (the same two divisions open +0x98DC0's grid dispatch): the grid record carries
// the grid's left (+4), top (+0xC) and cell box width/height (+0x14/+0x15); the cell is the UNSIGNED
// quotient (mouse - origin) / box, so a point left of or above the grid wraps to a huge column and misses.
// D2Common #10801 then answers only for a cell inside the grid's columns x rows.
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
// 1 and out[0..1] = column,row when the point is inside the grid; 0 otherwise (box 0 is a miss, not a fault).
EMSCRIPTEN_KEEPALIVE int32_t d2_client_inv_grid_cell(int32_t left, int32_t top, int32_t boxW, int32_t boxH,
                                                     int32_t x, int32_t y, int32_t cols, int32_t rows, int32_t* out) {
    if (boxW <= 0 || boxH <= 0 || cols <= 0 || rows <= 0) return 0;
    const uint32_t cx = (uint32_t)(x - left) / (uint32_t)boxW;
    const uint32_t cy = (uint32_t)(y - top) / (uint32_t)boxH;
    if (cx >= (uint32_t)cols || cy >= (uint32_t)rows) return 0;
    if (out) { out[0] = (int32_t)cx; out[1] = (int32_t)cy; }
    return 1;
}
}
