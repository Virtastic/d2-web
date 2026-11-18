#include <cstdint>
#include <cstring>
// 1.13c Storm+0x1A9E0: heap block carve: write header for used block, trim/free tail.
// Addresses are 32-bit offsets into `mem` (retail pointers). Implicit regs: EAX=heap, ESI=block.
template <class T> static T rd(uint8_t* m, uint32_t a) { T v; memcpy(&v, m + a, sizeof v); return v; }
template <class T> static void wr(uint8_t* m, uint32_t a, T v) { memcpy(m + a, &v, sizeof v); }
extern "C" void Storm_Sub1A9E0(uint8_t* mem, uint32_t heap, uint32_t oldSize, uint32_t newSize, uint32_t blk,
                               uint32_t flagA /*DAT_6fc43554*/, uint32_t flagB /*DAT_6fc44d5c*/) {
    // 1A6F0: size class -> total (iTotal), tail pad (pad), trailer flag
    uint32_t big = (flagA != 0 || newSize >= 0xfe88) ? 1 : 0;
    uint32_t trailer = (flagB != 0 && !big) ? 1 : 0;
    uint32_t sz = big ? 4 : newSize;
    int32_t base = (trailer ? 2 : 0) + 8 + (int32_t)sz;
    int32_t total = base + ((-base) & 7);
    int32_t pad = total - base;
    uint8_t oldFlags = rd<uint8_t>(mem, blk + 3);
    // 1A970: release slack [pad, size) of block
    uint32_t bsz = rd<uint16_t>(mem, blk), end = blk + bsz, slack = bsz - pad;
    if (end == rd<uint32_t>(mem, heap + 0x20)) wr<uint32_t>(mem, heap + 0x20, blk + pad);
    else if (slack > 0xf) {
        uint32_t f = blk + pad;
        wr<uint16_t>(mem, f, (uint16_t)slack); wr<uint8_t>(mem, f + 2, 0); wr<uint8_t>(mem, f + 3, 2);
        uint32_t i = (slack & 0xffff) >> 5; if (i > 7) i = 8;
        wr<uint32_t>(mem, f + 4, rd<uint32_t>(mem, heap + 0x38 + i * 4));
        wr<uint32_t>(mem, heap + 0x38 + i * 4, f);
    } else {
        wr<uint8_t>(mem, end + 3, rd<uint8_t>(mem, end + 3) & 0xef);
        pad += slack; total += slack;
    }
    wr<uint16_t>(mem, blk, (uint16_t)total);
    wr<uint8_t>(mem, blk + 2, (uint8_t)pad);
    wr<uint8_t>(mem, blk + 3, (uint8_t)((trailer ? 1 : 0) | (oldFlags & 0xfe)));
    wr<uint32_t>(mem, blk + 4, rd<uint32_t>(mem, heap + 0xc));
    if (trailer) wr<uint16_t>(mem, blk + total - pad - 2, 0x12b1);
    wr<uint32_t>(mem, heap + 0x18, rd<uint32_t>(mem, heap + 0x18) + (newSize - oldSize));
}
