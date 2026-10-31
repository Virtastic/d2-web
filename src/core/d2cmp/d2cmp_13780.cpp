// D2CMP Codec.cpp: paint one DCC cell into its frame (DCC stage 2).
#include <cstring>
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13780
// Retail: ECX = pixel-code stream, EDX = equal-cell stream (may be 0), EAX = the cell's previous
// record, stack (cell, &pixel-buffer cursor), ret 8.
// Equal-cell bit set (only read when the stream exists and a previous record does): copy the
// previous record's pixels if the sizes match (through a 5-byte-stride scratch; nothing when the
// destinations coincide), else clear the cell; the cursor does not move.
// Otherwise: colour count from the 4-entry buffer (c1 == c0 -> 1 colour, c2 == c1 -> 2, else 4),
// fill or read 0/1/2-bit indices per pixel, and advance the cursor by one entry (4 bytes).
void d2cmp_13780(DccBits* code, DccBits* eq, const DccCell* prev, DccCell* cell, uint8_t** cursor) {
    if (eq != nullptr && prev->colors != nullptr && d2cmp_134d0(1, eq) != 0) {
        if (cell->w == prev->w && cell->h == prev->h) {
            const uint8_t* src = prev->dst;
            if (cell->dst == src) return;
            uint8_t tmp[28];
            uint8_t* t = tmp;
            for (int32_t n = cell->h; n > 0; n--) { memcpy(t, src, (size_t)(uint32_t)cell->w); t += 5; src += prev->stride; }
            uint8_t* d = cell->dst;
            t = tmp;
            for (int32_t y = 0; y < cell->h; y++) { memcpy(d, t, (size_t)(uint32_t)cell->w); t += 5; d += cell->stride; }
            return;
        }
        uint8_t* d = cell->dst;
        for (int32_t y = 0; y < cell->h; y++) { memset(d, 0, (size_t)(uint32_t)cell->w); d += cell->stride; }
        return;
    }
    uint8_t* d = cell->dst;
    int32_t bits = 0;
    for (int32_t i = 1; ; ) {
        if (cell->colors[i] == cell->colors[i - 1]) break;
        bits++;
        i = 1 << bits;
        if (!(i + 1 < 5)) break;
    }
    if (bits == 0) {
        uint8_t c = (*cursor)[0];
        for (int32_t y = 0; y < cell->h; y++) { memset(d, c, (size_t)(uint32_t)cell->w); d += cell->stride; }
    } else {
        for (int32_t y = 0; y < cell->h; y++) {
            for (int32_t x = 0; x < cell->w; x++) d[x] = (*cursor)[dcc_read(code, bits)];
            d += cell->stride;
        }
    }
    *cursor += 4;
}
