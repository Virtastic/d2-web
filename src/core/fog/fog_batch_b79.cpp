// Fog implementation batch b79 (decompile-derived).
#include <cstdint>
#include <cstddef>
#include <initializer_list>
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
uint32_t* FUN_6ff5d9e0(int, uint32_t, int*);
void LeaveCriticalSection(void*);
}

// 1.13c Fog+0xEDC0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t Fog_EDC0(int a, uint32_t b) {
  void* cs = nullptr;
  uint32_t* p = FUN_6ff5d9e0(a, b, (int*)&cs);
  uint32_t r = 0;
  if (p) r = p[0x1f];
  if (cs) LeaveCriticalSection(cs);
  return r;
}
