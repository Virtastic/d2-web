// D2CMP Codec.cpp: size a DCC direction's cell grid and count its frame cells.
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x14020
// Retail: ESI = box (after 13220). cellsW = (w+3)/4, cells = ((h+3)/4) * cellsW, total = walker steps.
void d2cmp_14020(DccBox* box) {
    int32_t cw = (box->w + 3) / 4;
    box->cells = ((box->h + 3) / 4) * cw;
    box->cellsW = cw;
    box->total = 0;
    if (box->nframes != 0) {
        DccIter it = {};
        it.frame = -1;
        while (d2cmp_13300(&it, box)) box->total++;
    }
}
