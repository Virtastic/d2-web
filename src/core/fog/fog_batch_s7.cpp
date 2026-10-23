// Fog QServer batch: F730 create, E790, DA60, EE00, D8C0. Decompile-derived.
#include <cstdint>
#include <cstring>
extern "C" {
void FUN_6ff61690(void*);
void* FUN_6ff6b560(int*);
void FUN_6ff60f30(int, const char*, int, const char*);
uint32_t FUN_6ff5e8c0(uint32_t);
extern const char DAT_6ff768ec[];
extern int DAT_6ff7fee4, DAT_6ff7fee8, DAT_6ff7feec, DAT_6ff7fef0, DAT_6ffa6e80;
void EnterCriticalSection(void*);
void LeaveCriticalSection(void*);
void InitializeCriticalSection(void*);
}
static const char kQS7[] = "..\\Source\\Fog\\Src\\QServer\\QServer.cpp";

// 1.13c Fog+0x0E790 (ord 10151): set server field +0x884 (0 -> 1) under the lock.
extern "C" void Fog_E790(uint8_t* srv, int v) {
  if (!srv) return;
  if (!v) v = 1;
  FUN_6ff61690(srv + 0x854);
  EnterCriticalSection(srv + 0x854);
  *(int*)(srv + 0x884) = v;
  LeaveCriticalSection(srv + 0x854);
}

// 1.13c Fog+0x0EE00 (ord 10157)
extern "C" uint32_t Fog_EE00(uint8_t* srv, uint32_t p2, uint32_t, uint32_t p4) {
  if (!srv) return 0;
  int m = *(int*)(srv + 0x86c);
  typedef uint32_t (*Cb)(uint32_t);
  if ((m == 1 || m == 2) && *(Cb*)(srv + 0xbcc)) return (*(Cb*)(srv + 0xbcc))(p4);
  return FUN_6ff5e8c0(p2);
}

// 1.13c Fog+0x0D8C0 (ord 10158): hash lookup, bucket = key & mask; returns node[6] or 0.
extern "C" uint32_t Fog_D8C0(uint8_t* t, uint32_t key) {
  uint32_t b = *(uint32_t*)(t + 0xc) & key;
  uint8_t* cs = (uint8_t*)(intptr_t)*(uint32_t*)(t + 0x1c) + b * 0x18;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  uint32_t* n = (uint32_t*)(intptr_t)((uint32_t*)(intptr_t)*(uint32_t*)(t + 0x14))[b];
  while (n) {
    if (*n == key) { uint32_t r = n[6]; LeaveCriticalSection(cs); return r; }
    n = (uint32_t*)(intptr_t)n[1];
  }
  LeaveCriticalSection(cs);
  return 0;
}

// 1.13c Fog+0x0DA60 (ord 10156): pop head packet of queue idx, copy out, recycle to free list.
extern "C" uint32_t Fog_DA60(uint8_t* srv, int idx, uint8_t* out, uint32_t cap) {
  if (!srv || idx >= *(int*)(srv + 0x87c)) return 0xffffffff;
  if (cap > 0x5b4) cap = 0x5b4;
  void* cs = srv + 0x854;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  uint32_t* heads = (uint32_t*)(intptr_t)*(uint32_t*)(srv + 0x870);
  uint32_t* tails = (uint32_t*)(intptr_t)*(uint32_t*)(srv + 0x874);
  uint32_t* p = (uint32_t*)(intptr_t)heads[idx];
  if (!p) { LeaveCriticalSection(cs); return 0xffffffff; }
  heads[idx] = p[0x16f];
  if ((uint32_t*)(intptr_t)tails[idx] == p) tails[idx] = 0;
  LeaveCriticalSection(cs);
  uint32_t len = p[0x16d];
  uint32_t n = len + 4 < cap ? len + 4 : cap;
  memcpy(out, p, n);
  memset(p, 0, 0x5c0);
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  p[0x16f] = *(uint32_t*)(srv + 0x878);
  *(uint32_t*)(srv + 0x878) = (uint32_t)(intptr_t)p;
  LeaveCriticalSection(cs);
  return len;
}

static void* qalloc(int line) {
  void* p = FUN_6ff6b560(nullptr);
  if (!p) FUN_6ff60f30(2, kQS7, line, DAT_6ff768ec);
  return p;
}

// 1.13c Fog+0x0F730 (ord 10149): QServer create. GetVersionEx/Ordinal_115 tail is host-only
// (NT platform check sets mode flag; network init not applicable in wasm): flag = 0.
extern "C" int* Fog_F730(int mode, int nq, uint32_t, int, int p5, int p6, int p7, int p8) {
  int* s = (int*)qalloc(0x94);
  memset(s, 0, 0x313 * 4);
  s[3] = DAT_6ff7fee8; s[2] = DAT_6ff7fee4; s[0x306] = DAT_6ffa6e80;
  s[6] = DAT_6ff7fee4; s[4] = DAT_6ff7feec;
  uint32_t* z = (uint32_t*)qalloc(0x9f); s[5] = (int)(intptr_t)z;
  memset(z, 0, DAT_6ff7fee4 * 4);
  z = (uint32_t*)qalloc(0xa2); s[7] = (int)(intptr_t)z;
  memset(z, 0, DAT_6ff7fee4 * 24);
  for (int i = 0; i < DAT_6ff7fee4; i++) InitializeCriticalSection((uint8_t*)z + i * 0x18);
  s[9] = DAT_6ff7fee4;
  z = (uint32_t*)qalloc(0xa9); s[8] = (int)(intptr_t)z;
  memset(z, 0, DAT_6ff7fee4 * 4);
  InitializeCriticalSection(s + 10);
  s[0x11] = DAT_6ff7feec;
  z = (uint32_t*)qalloc(0xaf); s[0x10] = (int)(intptr_t)z;
  memset(z, 0, DAT_6ff7feec * 4);
  s[0x22c] = DAT_6ff7fef0;
  z = (uint32_t*)qalloc(0xb4); s[0x22b] = (int)(intptr_t)z;
  memset(z, 0, DAT_6ff7fef0 * 4);
  s[0x2f1] = p6; s[0x2f0] = p5; s[0x220] = 0; s[0x213] = 0; s[0x214] = 0;
  s[0x2f2] = p7; s[0x2f3] = p8; s[0x222] = -1; s[0x224] = 0; s[0x2f4] = 1;
  InitializeCriticalSection(s + 0x225);
  InitializeCriticalSection(s + 0x22d);
  s[0x2e4] = s[0x2e5] = s[0x2e6] = s[0x2e7] = -1;
  z = (uint32_t*)qalloc(0xda); s[0x21c] = (int)(intptr_t)z;
  for (int i = 0; i < nq; i++) z[i] = 0;
  z = (uint32_t*)qalloc(0xde); s[0x21d] = (int)(intptr_t)z;
  for (int i = 0; i < nq; i++) z[i] = 0;
  s[0x21f] = nq; s[0x21e] = 0; s[0x223] = 0;
  InitializeCriticalSection(s + 0x215);
  s[0] = 0;
  s[0x21b] = mode;
  return s;
}
