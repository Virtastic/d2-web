// Fog+0x1B9F0 pool block realloc (decompile-derived). Hidden register inputs are leading params.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
extern "C" {
void FUN_6ff61690(void*);
void FUN_6ff5fc20(void);
void Ordinal_116(void);
void FUN_6ff60f30(int, const char*, int, const char*);
void FUN_6ff684a0(void);
void FUN_6ff6ad20(const char*);
void* FUN_6ff520d7(void*, size_t);
void EnterCriticalSection(void*);
void LeaveCriticalSection(void*);
}

// 1.13c Fog+0x1B9F0  (EAX = user ptr, EBX = pool, hidden; param_1 = new-size-ish arg, header is at ptr-4)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* Fog_1B9F0(uint8_t* eax, uint8_t* ebx, uint8_t* param_1) {
  uint32_t* blk = (uint32_t*)(eax - 4);
  void* cs = ebx + 4;
  auto slot = [&](void* p) { return (uint32_t**)(ebx + 0x7ac) + ((int32_t)(intptr_t)p % 0x3ff); };
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  uint32_t* prev = nullptr;
  uint32_t* cur = *slot(blk);
  for (; cur != blk; cur = *(uint32_t**)cur) {
    if (!cur) {
      FUN_6ff61690(cs);
      LeaveCriticalSection(cs);
      return nullptr;
    }
    prev = cur;
  }
  if (!prev) *slot(blk) = *(uint32_t**)cur;
  else *(uint32_t**)prev = *(uint32_t**)cur;
  uint32_t* n = (uint32_t*)FUN_6ff520d7(blk, *(uint32_t*)(param_1 + 4));
  if (!n) {
    FUN_6ff6ad20("Unable to allocate free block of %s");
    FUN_6ff5fc20();
    Ordinal_116();
    FUN_6ff60f30(6, "", 0x421, "Unrecoverable internal error %08x");
    FUN_6ff684a0();
    _Exit(-1);
  }
  *(uint32_t**)n = *slot(n);
  *slot(n) = n;
  FUN_6ff61690(cs);
  LeaveCriticalSection(cs);
  return n + 1;
}
