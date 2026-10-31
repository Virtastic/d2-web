// D2CMP DCC codec shared shapes (Codec.cpp). Native pointers replace retail dwords; offsets noted.
#pragma once
#include <cstdint>

// Bit reader: +0 start, +4 cursor, +8 unused, +C bit position (always < 32).
struct DccBits { const uint32_t* start; const uint32_t* cur; uint32_t _8; int32_t pos; };

// Frame record, stride 0x2C (global array at 1.13c D2CMP .data 0x6FF10FD8).
struct DccFrame {
    int32_t var0, w, h, xoff, yoff;   // +0..+10
    uint32_t optBytes;                // +14
    const uint8_t* opt;               // +18
    uint32_t coded;                   // +1C
    uint8_t* pix;                     // +20
    int32_t stride;                   // +24
    int32_t bottomUp;                 // +28
};

// Direction box written by 13220 / 14020. hdr: +8 directions, +C frames per direction.
struct DccBox {
    const int32_t* hdr;               // +0
    DccFrame* frames;                 // +4
    int32_t xmin, ymin, w, h;         // +8..+14
    int32_t dir, nframes;             // +18, +1C
    int32_t total, cells, cellsW;     // +20, +24, +28
};

// Cell walker state, 15 dwords (+0..+38); zeroed with frame = -1 before the first step (see 13300).
struct DccIter {
    int32_t frame, w, h, x, y;        // [0] frame index, [1..2] size, [3..4] origin in the box
    uint8_t* pix;                     // [5] frame pixels (frame+0x20)
    int32_t stride;                   // [6] frame+0x24
    int32_t cellRow, cell;            // [7] first cell of the row, [8] current cell
    int32_t cx, cy;                   // [9], [10] position inside the frame
    int32_t rowOff, off;              // [B] row pixel offset, [C] cell pixel offset
    int32_t cw, ch;                   // [D], [E] current cell size
};

// Cell record, stride 0x14 (global array at .data 0x6FEF5860): last pixel-buffer entry, frame
// destination, frame stride, cell width, cell height.
struct DccCell { uint8_t* colors; uint8_t* dst; int32_t stride, w, h; };

// Pixel-buffer cursor and filled length (two adjacent dwords in retail).
struct DccPixBuf { uint8_t* buf; int32_t len; };

typedef void (*D2CmpFatalFn)(uint32_t code);   // Ordinal_10265 -> Ordinal_10024 -> _exit(-1)

// .rdata 0x6FE2D4B0: mask[n] = (1<<n)-1, mask[32] = ~0.
static inline uint32_t dcc_mask(int32_t n) { return n >= 32 ? 0xffffffffu : ((1u << n) - 1u); }

// The reader every Codec.cpp routine inlines (1.13c D2CMP+0x134D0 is the out-of-line copy).
static inline uint32_t dcc_read(DccBits* b, int32_t n) {
    int32_t pos = b->pos, room = 32 - pos;
    if (room < n) {
        uint32_t lo = dcc_mask(room) & (b->cur[0] >> pos);
        b->cur++;
        b->pos = n - room;
        return lo | ((dcc_mask(n - room) & b->cur[0]) << room);
    }
    uint32_t v = dcc_mask(n) & (b->cur[0] >> pos);
    b->pos = pos + n;
    if (pos + n == 32) { b->cur++; b->pos = 0; }
    return v;
}

uint32_t d2cmp_134d0(int32_t n, DccBits* b);
void d2cmp_13170(uint8_t* outA, const uint8_t* flags, uint8_t* outB);
void d2cmp_13a30(const uint32_t* bits256, uint8_t* outA, uint8_t* outB);
void d2cmp_13220(const int32_t* hdr, int32_t dir, DccFrame* frames, DccBox* box, D2CmpFatalFn fatal);
int32_t d2cmp_13300(DccIter* it, const DccBox* box);
void d2cmp_14020(DccBox* box);
int32_t d2cmp_13620(uint8_t* out, const DccFrame* f, const uint8_t* hdr, void (*log)(const char*));
void d2cmp_13780(DccBits* code, DccBits* eq, const DccCell* prev, DccCell* cell, uint8_t** cursor);
void d2cmp_13b70(DccBits* code, const DccBox* box, DccCell* cells, DccBits* eq, DccBits* mask,
                 DccBits* enc, DccBits* raw, DccPixBuf* pb, int32_t* bitsUsed);
