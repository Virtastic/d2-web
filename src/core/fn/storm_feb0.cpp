// src/core/fn/storm_feb0.cpp -- 1.13c Storm+0xfeb0
#include <emscripten.h>
#include <cstdint>
namespace {
// 0x6FC45A64 (iVar3==0) and 0x6FC45A3C (iVar3==1): list head words; first node pointer at +8, next at node+4.
uint8_t* g_head[2];   // zero until the archive code (unmapped) populates them
}
// 1.13c Storm+0xfeb0: scan list 45A3C then 45A64 for a node whose byte +0x212 == id; 0xFF -> 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* Storm_SubFEB0(int32_t id) {
  if (id == 0xff) return nullptr;
  for (int i = 1; i >= 0; i--)
    for (uint8_t* n = g_head[i]; n; n = *(uint8_t**)(n + 4))
      if (n[0x212] == (uint8_t)id) return n;
  return nullptr;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_feb0(uint32_t* r) { r[4] = (uint32_t)(uintptr_t)Storm_SubFEB0((int32_t)r[0]); }
