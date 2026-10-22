// src/core/realm/bnclient_batch_4f11.cpp -- UNMAPPED 1.13c BNClient.dll alpha-lane row (implementation +
// marker only), transcribed from build-wasm/ghidra-bnclient/decomp/0x4F11.c (image base 0x6FF20000).
// Same CRT small-block heap layout as bnclient_batch_4a3c.cpp (bn_sbh_free_block).
#include <cstdint>
#include <emscripten.h>

static inline uint32_t* Q32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  Q8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// Drop a free block from its bucket list; clear the bucket bit/tally when it was the last one.
static void sbh_rs_unlink(uint32_t* region, uint32_t heap, uint32_t sub, uint32_t rec, uint32_t bucket) {
    if (Q32(rec)[1] == Q32(rec)[2]) {
        uint32_t m = ~(0x80000000u >> (bucket & 0x1f));
        uint32_t* w = Q32(heap + (bucket < 0x20 ? 0x44 : 0xc4) + sub * 4);
        *w &= m;
        if (--*Q8(bucket + 4 + heap) == 0) region[bucket < 0x20 ? 0 : 1] &= m;
    }
    *Q32(Q32(rec)[2] + 4) = Q32(rec)[1];
    *Q32(Q32(rec)[1] + 8) = Q32(rec)[2];
}

// Link a free block at the head of its bucket list; set the bucket bit/tally when it is the first one.
static void sbh_rs_link(uint32_t* region, uint32_t heap, uint32_t sub, uint32_t desc, uint32_t blk, uint32_t bucket) {
    uint32_t d = desc + bucket * 8;
    Q32(blk)[1] = Q32(d)[1];
    Q32(blk)[2] = d;
    Q32(d)[1] = blk;
    *Q32(Q32(blk)[1] + 8) = blk;
    if (Q32(blk)[1] == Q32(blk)[2]) {
        uint8_t old = (*Q8(bucket + 4 + heap))++;
        if (old == 0) region[bucket < 0x20 ? 0 : 1] |= 0x80000000u >> (bucket & 0x1f);
        *Q32(heap + (bucket < 0x20 ? 0x44 : 0xc4) + sub * 4) |= 0x80000000u >> (bucket & 0x1f);
    }
}

static uint32_t sbh_rs_bucket(int32_t size) {
    uint32_t b = (uint32_t)((size >> 4) - 1);
    return b > 0x3f ? 0x3f : b;
}

// 1.13c BNClient+0x04F11 (735 B, caller 0x2275): ___sbh_resize_block(region, block, size). Returns 1 when
// the block now has the rounded size ((size+0x17)&~0xF), 0 when growing is impossible in place (next
// block in use or too small). Growing absorbs part of the free next block; shrinking splits the tail
// off and merges it with a free next block.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_sbh_resize_block(uint32_t region, uint32_t block, int32_t size) {
    uint32_t* r = Q32(region);
    uint32_t heap = r[4];
    uint32_t sub = (uint32_t)(block - r[3]) >> 15;
    uint32_t desc = sub * 0x204 + 0x144 + heap;
    uint32_t want = ((uint32_t)size + 0x17) & 0xfffffff0u;
    int32_t cur = (int32_t)*Q32(block - 4) - 1;
    uint32_t nx = *Q32(block - 4) - 5 + block;          // next block header
    uint32_t nsz = *Q32(nx);

    if (cur < (int32_t)want) {
        if ((nsz & 1) || (int32_t)(nsz + cur) < (int32_t)want) return 0;
        sbh_rs_unlink(r, heap, sub, nx, sbh_rs_bucket((int32_t)nsz));
        int32_t rest = (int32_t)(nsz + (cur - want));
        if (rest > 0) {
            uint32_t t = block - 4 + want;
            sbh_rs_link(r, heap, sub, desc, t, sbh_rs_bucket(rest));
            *Q32(t) = rest;
            *Q32(t + rest - 4) = rest;
        }
        *Q32(block - 4) = want + 1;
        *Q32(block - 8 + want) = want + 1;
    } else if ((int32_t)want < cur) {
        int32_t rest = cur - (int32_t)want;
        uint32_t t = block - 4 + want;
        *Q32(block - 4) = want + 1;
        *Q32(t - 4) = want + 1;
        if ((nsz & 1) == 0) {
            sbh_rs_unlink(r, heap, sub, nx, sbh_rs_bucket((int32_t)nsz));
            rest += (int32_t)nsz;
        }
        sbh_rs_link(r, heap, sub, desc, t, sbh_rs_bucket(rest));
        *Q32(t) = rest;
        *Q32(t + rest - 4) = rest;
    }
    return 1;
}
