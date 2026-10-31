// D2CMP Codec.cpp: build the DCC pixel buffer, one 4-colour entry per changed cell (DCC stage 1).
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13B70
// Retail: ESI = pixel-code stream, stack (box, cells, equal-cell, pixel-mask, encoding-type,
// raw-pixel, &pixbuf, &bitsUsed), ret 0x20; optional streams are 0 when absent.
// Per walker cell: no previous entry -> mask 0xF, no equal/mask reads; else an equal-cell bit
// set keeps the previous entry (no new entry); else a 4-bit mask. Mask bits 3..0 in turn: clear
// copies the previous entry's byte; set decodes a colour (encoding bit 1: 8 raw bits; else the
// last colour plus 4-bit steps while the step is 15). A colour equal to the last one ends the
// list: the colours decoded above it move down onto the lowest set positions, the rest of the
// set positions clear, and the clear positions below it copy the previous entry.
void d2cmp_13b70(DccBits* code, const DccBox* box, DccCell* cells, DccBits* eq, DccBits* mask,
                 DccBits* enc, DccBits* raw, DccPixBuf* pb, int32_t* bitsUsed) {
    uint8_t* out = pb->buf;
    int32_t bits0 = (int32_t)(code->cur - code->start) * 32 + code->pos;
    if (box->nframes != 0) {
        DccIter it = {};
        it.frame = -1;
        uint8_t* cur = out;
        while (d2cmp_13300(&it, box)) {
            DccCell* c = &cells[it.cell];
            uint8_t* prev = c->colors;
            uint32_t m = 0xf;
            if (prev != nullptr) {
                if (eq != nullptr && dcc_read(eq, 1) != 0) continue;
                m = dcc_read(mask, 4);
            }
            uint32_t encBit = 0;
            if (m != 0 && enc != nullptr) encBit = dcc_read(enc, 1);
            const uint32_t set = m | 0xfffffff0u;
            const intptr_t diff = prev - cur;
            uint8_t last = 0;
            int32_t i = 4;
            uint8_t* p = cur + 4;
            for (;;) {
                i--; p--;
                if (!(set & (1u << i))) {
                    *p = p[diff];
                    if (i == 0) break;
                    continue;
                }
                uint8_t v;
                if (encBit == 0) {
                    v = last;
                    uint32_t d;
                    do { d = dcc_read(code, 4); v = (uint8_t)(v + d); } while (d == 0xf);
                } else {
                    v = (uint8_t)dcc_read(raw, 8);
                }
                *p = v;
                if (last != v) { last = v; if (i == 0) break; continue; }
                int32_t stop = i, j = i, k = -1;
                for (;;) {
                    j++; while (!(set & (1u << j))) j++;
                    if (j > 3) break;
                    k++; while (!(set & (1u << k))) k++;
                    cur[k] = cur[j];
                }
                for (;;) {
                    k++; while (!(set & (1u << k))) k++;
                    if (k > 3) break;
                    cur[k] = 0;
                }
                if (stop != 0) {
                    uint8_t* q = cur + stop;
                    int32_t t = stop;
                    do { t--; q--; if (!(set & (1u << t))) *q = q[diff]; } while (t != 0);
                }
                break;
            }
            c->colors = cur;
            cur += 4;
        }
        out = cur;
    }
    *bitsUsed = (int32_t)(code->cur - code->start) * 32 - bits0 + code->pos;
    pb->len = (int32_t)(out - pb->buf);
}
