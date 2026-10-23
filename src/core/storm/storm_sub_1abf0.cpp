// src/core/storm/storm_sub_1abf0.cpp -- 1.13c Storm+0x1ABF0
#include <cstdint>
// Heap block free (register args: ESI = page, EDI = block). Retail 32-bit layout; addresses are u32
// resolved through env.at(). Block: u16 size +0, u8 +2 pad, flags +3, u32 +4 free-list next, +8 big-alloc ptr.
struct StormFreeEnv {
  uint8_t* (*at)(uint32_t addr);
  void (*virtualFree)(uint32_t addr);       // VirtualFree(addr,0,MEM_RELEASE)
  uint32_t allocGranularity;                // DAT_6fc43550
  uint8_t* pageActiveFlags;                 // DAT_6fc44d60 (indexed by page+8)
  uint32_t* curPage;                        // DAT_6fc45160
};
template <class T> static T& F(StormFreeEnv& e, uint32_t a, uint32_t off) { return *(T*)e.at(a + off); }
// 1.13c Storm+0x1ABF0
extern "C" void Storm_Sub1ABF0(StormFreeEnv& e, uint32_t pg, uint32_t blk) {
  uint8_t& fl = F<uint8_t>(e, blk, 3);
  int32_t sz;
  if (!(fl & 4)) {
    sz = (int32_t)F<uint16_t>(e, blk, 0) - F<uint8_t>(e, blk, 2) - ((fl & 1) ? 10 : 8);
  } else {
    uint32_t p = F<uint32_t>(e, blk, 8);
    if (!p) sz = 0;
    else {
      sz = F<int32_t>(e, p, (uint32_t)-0x10);
      F<int32_t>(e, pg, 0x34) -= sz;
      e.virtualFree(~(e.allocGranularity - 1) & (p - 0x10));
    }
  }
  F<int32_t>(e, pg, 0x18) -= sz;
  F<int32_t>(e, pg, 0x14) -= 1;
  fl = (fl & 0x10) | 2;
  F<uint8_t>(e, blk, 2) = 0;
  uint32_t nx = F<uint16_t>(e, blk, 0) + blk;
  if (F<uint32_t>(e, pg, 0x20) == nx) {
    F<uint16_t>(e, blk, 4) = 0; F<uint16_t>(e, blk, 6) = 0;
    F<uint32_t>(e, pg, 0x20) = blk;
  } else {
    F<uint8_t>(e, nx, 3) |= 0x10;
    uint32_t b = F<uint16_t>(e, blk, 0) >> 5; if (b > 7) b = 8;
    F<uint32_t>(e, blk, 4) = F<uint32_t>(e, pg, 0x38 + b * 4);
    F<uint32_t>(e, pg, 0x38 + b * 4) = blk;
    if ((fl & 0x10) || (F<uint8_t>(e, nx, 3) & 2)) F<int32_t>(e, pg, 0x24) += 1;
  }
  if (F<int32_t>(e, pg, 0x14) == 0) {
    F<uint32_t>(e, pg, 0x20) = F<uint32_t>(e, pg, 0x1C);
    for (uint32_t o = 0x38; o <= 0x58; o += 4) F<uint32_t>(e, pg, o) = 0;
    F<uint32_t>(e, pg, 0x24) = 0;
    if (F<uint32_t>(e, pg, 4) < 0x80000000u) {
      e.pageActiveFlags[F<int32_t>(e, pg, 8)] = 1;
      *e.curPage = pg;
    }
  }
}
