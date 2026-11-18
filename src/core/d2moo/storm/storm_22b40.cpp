// 1.13c Storm+0x22B40: SCOMP PKWARE implode wrapper (dst, &dstSize, src, srcSize, &type). Returns SMemFree's result.
#include <cstdint>
#include "storm_scomp_cb.h"

extern "C" int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);
extern "C" uint32_t* Storm_Sub1C580(uint32_t size, const char* file, int32_t line, uint32_t flags);
extern "C" uint32_t storm_n13_F2E0(uint32_t readCb, uint32_t writeCb, uint8_t* st, uint32_t param, uint32_t* type, uint32_t* dsize);

// 1.13c Storm+0x22B40
extern "C" int32_t Storm_Sub22B40(uint8_t* dst, uint32_t* dstSize, const uint8_t* src, uint32_t srcSize, int32_t* type, uint32_t) {
  static const char kF[] = "..\\3rdParty\\STORM\\SOURCE\\SCOMP.CPP";
  uint8_t* st = (uint8_t*)Storm_Sub1C580(0x8dd8, kF, 0x3f1, 0);
  uint32_t ctx[6] = { (uint32_t)(uintptr_t)dst, 0, *dstSize, (uint32_t)(uintptr_t)src, 0, srcSize };
  uint32_t flag = (*type == 2);
  const uint32_t lim = srcSize < 0xc00 ? ((srcSize < 0x600 ? 0xfffffc00u : 0u) + 0x800) : 0x1000;
  uint32_t dsz = lim;
  storm_n13_F2E0(scomp::cb32(scomp::readCb), scomp::cb32(scomp::writeCb), st, (uint32_t)(uintptr_t)ctx, &flag, &dsz);
  const int32_t r = Storm_Sub1B960(st, kF, 0x40f);
  *dstSize = ctx[1];
  *type = 0;
  return r;
}
