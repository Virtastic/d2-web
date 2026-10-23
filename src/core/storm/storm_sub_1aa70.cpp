// src/core/storm/storm_sub_1aa70.cpp -- 1.13c Storm+0x1AA70
#include <cstdint>
// Heap block in-place resize. Page layout (retail 32-bit): +0x0C stamp, +0x18 bytes-in-use, +0x20 end,
// +0x24 free-list count, +0x2C committed, +0x30 reserved. Block: u16 size +0, u8 pad +2, flags +3 (0x02 free, 0x10 busy-mark), u32 +4 stamp.
struct StormResizeEnv {
  uint8_t* page;                                    // unaff_EBX
  bool dat43554, dat44d5c;                          // DAT_6fc43554 / DAT_6fc44d5c
  bool (*commit)(uint8_t* page, uint32_t newEnd);   // 1A550 (EAX=newEnd offset, EDI=page)
  void (*rebuild)(uint8_t* page);                   // 1A770 (EAX=page)
  void (*split)(uint8_t* page, uint16_t* blk, int32_t* total, int32_t* pad); // 1A970
};
// 1.13c Storm+0x1AA70: blk = block header, oldReq/newReq = requested sizes. Returns 1 on success.
extern "C" uint32_t Storm_Sub1AA70(StormResizeEnv& e, uint16_t* blk, int32_t oldReq, uint32_t newReq) {
  uint8_t* pg = e.page;
  // 1A6F0
  int32_t big = (!e.dat43554 && newReq < 0xfe88) ? 0 : 1;
  int32_t trk = (e.dat44d5c && !big) ? 1 : 0;
  uint32_t n = big ? 4 : newReq;
  int32_t t = (trk ? 2 : 0) + 8 + (int32_t)n;
  int32_t total = t + ((-t) & 7), pad = total - t;
  if ((uint32_t)total > 0xffff || big) return 0;
  uint32_t cur = *blk;
  if ((uint32_t)total <= cur) {
    pad += (int32_t)(cur - total);
    total = (int32_t)cur;
  } else {
    uint32_t need = total - cur, acc = 0;
    uint8_t* endNew = (uint8_t*)blk + total;
    uint16_t* p = (uint16_t*)((uint8_t*)blk + cur);
    {
      do {
        if ((uint8_t*)p == *(uint8_t**)(pg + 0x20)) break;
        if ((((uint8_t*)p)[3] & 2) == 0) return 0;
        acc += *p; p = (uint16_t*)((uint8_t*)p + *p);
      } while (acc < need);
    }
    uint32_t use = (acc <= need) ? need : acc;
    if (cur + use > 0xffff) return 0;
    if (acc < need) {
      uint32_t off = (uint32_t)(endNew - pg);
      if (*(uint32_t*)(pg + 0x2c) < off) {
        if (*(uint32_t*)(pg + 0x30) < off) return 0;
        if (!e.commit(pg, off)) return 0;
      }
      *(uint8_t**)(pg + 0x20) = endNew;
    }
    *blk = (uint16_t)(cur + use);
    e.split(pg, blk, &total, &pad);
    *blk = (uint16_t)total;
    if (endNew != *(uint8_t**)(pg + 0x20)) {
      uint8_t b = endNew[3]; endNew[3] = b & 0xef;
      if ((b & 2) && *(uint32_t*)(pg + 0x24)) *(uint32_t*)(pg + 0x24) -= 1;
    }
    e.rebuild(pg);
  }
  *blk = (uint16_t)total;
  ((uint8_t*)blk)[2] = (uint8_t)pad;
  ((uint8_t*)blk)[3] = (uint8_t)((trk != 0) | (((uint8_t*)blk)[3] & 0xfe));
  *(uint32_t*)((uint8_t*)blk + 4) = *(uint32_t*)(pg + 0x0c);
  if (trk) *(uint16_t*)((uint8_t*)blk + total - 2 - pad) = 0x12b1;
  *(uint32_t*)(pg + 0x18) += newReq - (uint32_t)oldReq;
  return 1;
}
