// src/core/d2gfx/d2gfx_sbh_resize_block.cpp -- 1.13c D2Game.dll ___sbh_resize_block, reimplemented
// from build-wasm/ghidra-d2gfx/decomp/0x911D.c (alpha lane). Image base 0x6FC20000. The shrink/grow
// protocol is the same CRT small-block heap as d2gfx_sbh_free_block.cpp and the D2MCPClient
// 0x3A44 port (src/core/realm/mcp_sbh_alloc.cpp); the region record is param_1 (region), the control
// block at region[4], subregion index from (block - region[3]) >> 15.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// 1.13c D2Game+0x911D
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_911D(uint32_t region, uint32_t block, uint32_t requested) {
    uint32_t control = P32(region)[4];                             // heap control block
    uint32_t group = (block - P32(region)[3]) >> 0xf;              // subregion index
    uint32_t buckets = group * 0x204u + 0x144u + control;          // bucket descriptor array
    uint32_t req = (requested + 0x17u) & 0xfffffff0u;              // rounded footprint
    int32_t curSize = (int32_t)P32(block - 4)[0] - 1;              // current size (in-use bit off)
    uint32_t nextHdr = (uint32_t)((int32_t)P32(block - 4)[0] - 5 + (int32_t)block);
    uint32_t nextSize = P32(nextHdr)[0];

    if (curSize < (int32_t)req) {
        // Grow: the next block must be free and the pair must cover the request.
        if ((nextSize & 1) != 0 || (int32_t)(nextSize + (uint32_t)curSize) < (int32_t)req) return 0;
        uint32_t b = ((int32_t)nextSize >> 4) - 1;
        if (b > 0x3f) b = 0x3f;
        if (P32(nextHdr)[1] == P32(nextHdr)[2]) {                  // singleton in its bucket
            uint8_t* tally = P8(control + b + 4);
            if (b < 0x20) {
                uint32_t mask = ~(0x80000000u >> (b & 0x1f));
                P32(control + 0x44 + group * 4)[0] &= mask;
                if (--*tally == 0) P32(region)[0] &= mask;
            } else {
                uint32_t mask = ~(0x80000000u >> ((b - 0x20) & 0x1f));
                P32(control + 0xc4 + group * 4)[0] &= mask;
                if (--*tally == 0) P32(region)[1] &= mask;
            }
        }
        P32(P32(nextHdr)[2] + 4)[0] = P32(nextHdr)[1];             // unlink the next block
        P32(P32(nextHdr)[1] + 8)[0] = P32(nextHdr)[2];
        int32_t left = (int32_t)(nextSize + (uint32_t)(curSize - (int32_t)req));
        if (left > 0) {                                            // split off the remainder
            uint32_t b2 = ((uint32_t)left >> 4) - 1;
            if (b2 > 0x3f) b2 = 0x3f;
            uint32_t h = block - 4 + req;
            uint32_t list = buckets + b2 * 8;
            P32(h)[1] = P32(list + 4)[0];
            P32(h)[2] = list;
            P32(list + 4)[0] = h;
            P32(P32(h)[1] + 8)[0] = h;
            if (P32(h)[1] == P32(h)[2]) {                          // first of its bucket
                uint8_t* tally = P8(control + b2 + 4);
                uint8_t was = *tally;
                *tally = (uint8_t)(was + 1);
                if (b2 < 0x20) {
                    if (was == 0) P32(region)[0] |= 0x80000000u >> (b2 & 0x1f);
                    P32(control + 0x44 + group * 4)[0] |= 0x80000000u >> (b2 & 0x1f);
                } else {
                    if (was == 0) P32(region)[1] |= 0x80000000u >> ((b2 - 0x20) & 0x1f);
                    P32(control + 0xc4 + group * 4)[0] |= 0x80000000u >> ((b2 - 0x20) & 0x1f);
                }
            }
            P32(h)[0] = (uint32_t)left;
            P32(h + (uint32_t)left - 4)[0] = (uint32_t)left;
        }
        P32(block - 4)[0] = req + 1;
        P32(block - 8 + req)[0] = req + 1;
    } else if ((int32_t)req < curSize) {
        // Shrink: split off the tail, folding a free next block into it first.
        uint32_t left = (uint32_t)(curSize - (int32_t)req);
        P32(block - 4)[0] = req + 1;
        uint32_t h = block - 4 + req;
        uint32_t b = (left >> 4) - 1;
        if (b > 0x3f) b = 0x3f;
        P32(h - 4)[0] = req + 1;
        if ((nextSize & 1) == 0) {                                 // next block is free: unlink it
            uint32_t nb = ((int32_t)nextSize >> 4) - 1;
            if (nb > 0x3f) nb = 0x3f;
            if (P32(nextHdr)[1] == P32(nextHdr)[2]) {
                uint8_t* tally = P8(control + nb + 4);
                if (nb < 0x20) {
                    uint32_t mask = ~(0x80000000u >> (nb & 0x1f));
                    P32(control + 0x44 + group * 4)[0] &= mask;
                    if (--*tally == 0) P32(region)[0] &= mask;
                } else {
                    uint32_t mask = ~(0x80000000u >> ((nb - 0x20) & 0x1f));
                    P32(control + 0xc4 + group * 4)[0] &= mask;
                    if (--*tally == 0) P32(region)[1] &= mask;
                }
            }
            P32(P32(nextHdr)[2] + 4)[0] = P32(nextHdr)[1];
            P32(P32(nextHdr)[1] + 8)[0] = P32(nextHdr)[2];
            left += nextSize;
            b = (left >> 4) - 1;
            if (b > 0x3f) b = 0x3f;
        }
        uint32_t list = buckets + b * 8;
        P32(h)[2] = list;
        P32(h)[1] = P32(list + 4)[0];
        P32(list + 4)[0] = h;
        P32(P32(h)[1] + 8)[0] = h;
        if (P32(h)[1] == P32(h)[2]) {
            uint8_t* tally = P8(control + b + 4);
            uint8_t was = *tally;
            *tally = (uint8_t)(was + 1);
            if (b < 0x20) {
                if (was == 0) P32(region)[0] |= 0x80000000u >> (b & 0x1f);
                P32(control + 0x44 + group * 4)[0] |= 0x80000000u >> (b & 0x1f);
            } else {
                if (was == 0) P32(region)[1] |= 0x80000000u >> ((b - 0x20) & 0x1f);
                P32(control + 0xc4 + group * 4)[0] |= 0x80000000u >> ((b - 0x20) & 0x1f);
            }
        }
        P32(h)[0] = left;
        P32(h + left - 4)[0] = left;
    }
    return 1;
}
