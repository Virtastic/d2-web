// 1.13c Storm+0x22AB0: SCOMP PKWARE explode wrapper (dst, &dstSize, src, srcSize). Returns SMemFree's result.
#include <cstdint>
#include "storm_scomp_cb.h"
#include "../../../third_party/StormLib/src/pklib/pklib.h"

extern "C" int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);
extern "C" uint32_t* Storm_Sub1C580(uint32_t size, const char* file, int32_t line, uint32_t flags);

struct StormExplodeEnv {
  uint32_t (*read)(uint8_t* buf, uint32_t* size, void* user);
  void* user;
  void (*copy)(void* dst, const void* src, uint32_t dwords);
  void (*build)(uint32_t n, uint8_t* a, const uint8_t* b, uint8_t* c);
  void (*decodeTbl)(uint8_t* st);
  uint32_t (*fill)(uint8_t* st);
  const uint8_t *t40d60, *t40d40, *t40d50, *t40d10, *t40d20, *t40c90, *t40cd0;
};
extern "C" uint8_t Storm_SubEC20(StormExplodeEnv& e, uint32_t cbLo, uint32_t cbHi, uint8_t* st, void* user);
extern "C" uint32_t Storm_ExplodeLitEEA0(uint8_t* st);
extern "C" int32_t Storm_ExplodeDistF050(uint8_t* st, int32_t len);
extern "C" void Storm_SubF2A0(void* dst, const void* src, uint32_t bytes);
extern "C" void Storm_SubF160(int32_t count, const uint8_t* widths, const uint8_t* codes, uint8_t* table);
extern "C" uint32_t pk_explode_loop(uint8_t* c, uint32_t (*decode_sym)(uint8_t*), int (*copy_dist)(uint8_t*, uint32_t),
                                    void (*mv)(void*, void*, int));
extern "C" void pk_gen_asc_tabs(uint8_t* c, const uint16_t* tab);

namespace {
void copyBytes(void* d, const void* s, uint32_t n) { Storm_SubF2A0(d, s, n); }
void buildF160(uint32_t n, uint8_t* a, const uint8_t* b, uint8_t* c) { Storm_SubF160((int32_t)n, a, b, c); }
void decodeF1A0(uint8_t* st) { pk_gen_asc_tabs(st, ChCodeAsc); }
int distW(uint8_t* st, uint32_t len) { return Storm_ExplodeDistF050(st, (int32_t)len); }
void mvW(void* d, void* s, int n) { Storm_SubF2A0(d, s, (uint32_t)n); }
uint32_t fillEDA0(uint8_t* st) { return pk_explode_loop(st, Storm_ExplodeLitEEA0, distW, mvW); }
}

// 1.13c Storm+0x22AB0
extern "C" int32_t Storm_Sub22AB0(uint8_t* dst, uint32_t* dstSize, const uint8_t* src, uint32_t srcSize) {
  static const char kF[] = "..\\3rdParty\\STORM\\SOURCE\\SCOMP.CPP";
  uint8_t* st = (uint8_t*)Storm_Sub1C580(0x3134, kF, 0x41C, 0);
  uint32_t ctx[6] = { (uint32_t)(uintptr_t)dst, 0, *dstSize, (uint32_t)(uintptr_t)src, 0, srcSize };
  StormExplodeEnv e{};
  e.read = scomp::readCb; e.copy = copyBytes; e.build = buildF160; e.decodeTbl = decodeF1A0; e.fill = fillEDA0;
  e.t40d60 = ChBitsAsc; e.t40d40 = LenBits; e.t40d50 = LenCode; e.t40d10 = ExLenBits;
  e.t40d20 = (const uint8_t*)LenBase; e.t40c90 = DistBits; e.t40cd0 = DistCode;
  Storm_SubEC20(e, scomp::cb32(scomp::readCb), scomp::cb32(scomp::writeCb), st, ctx);
  const int32_t r = Storm_Sub1B960(st, kF, 0x42E);
  *dstSize = ctx[1];
  return r;
}
