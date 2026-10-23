// Fog implementation batch b78 (decompile-derived). Register-arg hidden inputs are leading params.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
void FUN_6ff61690(void*);          // critical-section pre-hook
void FUN_6ff6b0c0(void**);
void FUN_6ff5fc20(void);
void Ordinal_116(void);
void Ordinal_111(void);
int Ordinal_3(int);
int Ordinal_19(int);
void FUN_6ff60f30(int, const char*, int, const char*);
void FUN_6ff684a0(void);
void FUN_6ff6ad20(const char*);
void* FUN_6ff520d7(void*, size_t);
void* FUN_6ff6cd10(int, const char*, int);
int FUN_6ff5ceb0(uint32_t);
void FUN_6ff5d230(void);
uint32_t* FUN_6ff5e4e0(uint8_t*, uint32_t);
void FUN_6ff5e1b0(uint32_t);
void FUN_6ff5e3b0(void);
void FUN_6ff690a0(const char*, const char*);
int FUN_6ff70180(int*, uint32_t*, uint32_t);
extern const char DAT_6ff768ec[];
void EnterCriticalSection(void*);
void LeaveCriticalSection(void*);
void DeleteCriticalSection(void*);
long InterlockedDecrement(volatile long*);
long InterlockedIncrement(volatile long*);
}

[[noreturn]] static void b78_fatal(int line) {
  FUN_6ff5fc20();
  Ordinal_116();
  FUN_6ff60f30(6, "", line, "Unrecoverable internal error %08x");
  FUN_6ff684a0();
  _Exit(-1);
}

// 1.13c Fog+0x17B60  (ESI = object, hidden)
extern "C" EMSCRIPTEN_KEEPALIVE void Fog_17B60(uint8_t* esi) {
  void* cs = esi + 4;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  int iVar1 = *(int*)(esi + 0x1c);
  while (iVar1 != 0) {
    void* local = *(void**)(esi + 0x1c);
    iVar1 = *(int*)((uint8_t*)(intptr_t)iVar1 + 4);
    FUN_6ff6b0c0(&local);
    *(int*)(esi + 0x1c) = iVar1;
  }
  *(int*)(esi + 0x20) = 0;
  *(int*)(esi + 0x1c) = 0;
  DeleteCriticalSection(cs);
}

// 1.13c Fog+0x1BB00  (EAX = pool, hidden)
extern "C" EMSCRIPTEN_KEEPALIVE void Fog_1BB00(uint8_t* eax, uint32_t* p) {
  uint8_t* tbl = *(uint8_t**)(eax + 0x7a8);
  void* cs = eax + 4;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  int n = *(int*)(eax + 0x7a0);
  int lo = 0, hi = n, mid;
  if (0 < n) {
    do {
      mid = ((hi - lo) >> 1) + lo;
      uint32_t v = *(uint32_t*)(tbl + 4 + mid * 8);
      if (v < p[0]) {
        lo = mid + 1;
        mid = hi;
      } else if (v <= p[0]) {
        if (mid < n - 1) memmove(tbl + mid * 8, tbl + mid * 8 + 8, (n - mid) * 8 - 8);
        int old = *(int*)(eax + 0x7a0);
        *(int*)(eax + 0x7a0) = old - 1;
        uint8_t* blk = *(uint8_t**)(p + 5);
        *(int*)(eax + 0x17a8) += -(((*(int*)(blk + 0x1c) + 0x1f) >> 5) * 4 + 0x18) - *(int*)(blk + 0x20);
        if (old + 0x3f < *(int*)(eax + 0x7a4)) {
          int cap = *(int*)(eax + 0x7a4) - 0x20;
          *(int*)(eax + 0x7a4) = cap;
          void* np = FUN_6ff520d7(*(void**)(eax + 0x7a8), (size_t)cap * 8);
          *(void**)(eax + 0x7a8) = np;
          if (!np) {
            FUN_6ff6ad20("(1) Pool Blocks overflowed at %d");
            b78_fatal(0x2b4);
          }
        }
        FUN_6ff61690(cs);
        LeaveCriticalSection(cs);
        return;
      }
      hi = mid;
    } while (lo < mid);
  }
  FUN_6ff61690(cs);
  LeaveCriticalSection(cs);
  b78_fatal(0x2bf);
}

// 1.13c Fog+0x0DF00  (ESI = server, hidden)
extern "C" EMSCRIPTEN_KEEPALIVE void Fog_DF00(uint8_t* esi, uint32_t id, uint32_t p2, uint32_t p3) {
  (void)p2;
  if (*(int*)(esi + 0xbd0) == 0 || FUN_6ff5ceb0(id) != 0) return;
  void (*cb)(uint32_t) = *(void (**)(uint32_t))(esi + 3000);
  if (cb) cb(p3);
  void* cs = esi + 0x894;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  uint32_t b = id % *(uint32_t*)(esi + 0x8b0);
  uint32_t** buckets = *(uint32_t***)(esi + 0x8ac);
  for (uint32_t* e = buckets[b]; e; e = *(uint32_t**)(e + 3)) {
    if (e[0] == id) {
      e[2] = 0x78;
      LeaveCriticalSection(cs);
      return;
    }
  }
  uint32_t* e = *(uint32_t**)(esi + 0x88c);
  if (!e) e = (uint32_t*)FUN_6ff6cd10(0x10, "..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x4a6);
  else *(uint32_t**)(esi + 0x88c) = *(uint32_t**)(e + 3);
  e[0] = id;
  e[1] = 0x78;
  e[2] = 0;
  *(uint32_t**)(e + 3) = buckets[b];
  buckets[b] = e;
  *(int*)(esi + 0x890) += 1;
  LeaveCriticalSection(cs);
  FUN_6ff5d230();
}

// 1.13c Fog+0x0E550  (EAX = conn id, ESI = server, hidden)
extern "C" EMSCRIPTEN_KEEPALIVE void Fog_E550(uint32_t eax, uint8_t* esi) {
  uint32_t* c = FUN_6ff5e4e0(esi, eax);
  if (!c || c[3] != 0) return;
  c[3] = 1;
  int r = Ordinal_3((int)c[4]);
  c[4] = 0xffffffff;
  if (r != 0) {
    Ordinal_111();
    FUN_6ff690a0(DAT_6ff768ec, "[QSrvCloseRemoteConnection] *** closesocket() err = %d ***");
  }
  if (InterlockedDecrement((volatile long*)(c + 2)) == 0) {
    void (*cb)(const char*, int) = *(void (**)(const char*, int))(esi + 0xbc8);
    if (cb) cb("..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x374);
    FUN_6ff5e1b0(c[0]);
    FUN_6ff5e3b0();
  }
  if (r == -1) Ordinal_111();
}

// 1.13c Fog+0x0E8C0  (ESI = server, EBX, EDI hidden)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t Fog_E8C0(int* esi, uint32_t* ebx, uint32_t edi, uint32_t id) {
  if (!esi) return 0;
  uint32_t* c = FUN_6ff5e4e0((uint8_t*)esi, id);
  if (!c || c[3] != 0) return 0;
  if (*esi != 0) return (uint32_t)FUN_6ff70180(esi, ebx, edi);
  if (Ordinal_19((int)c[4]) != -1) return 1;
  Ordinal_111();
  return 0;
}

// 1.13c Fog+0x1FF60  (EDI = async log/queue object, hidden; Win32 calls injected as seams)
extern "C" void FUN_6ff5e420(void);
typedef int (*b78_writefile_fn)(void*, const void*, uint32_t, uint32_t*, void*);
typedef uint32_t (*b78_u32_fn)(void);
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t Fog_1FF60(uint8_t* edi, b78_writefile_fn WriteFile, b78_u32_fn GetLastError) {
  uint32_t* buf = *(uint32_t**)(edi + 0x74);
  if (!buf) return 1;
  InterlockedIncrement((volatile long*)(edi + 8));
  void* h = *(void**)(edi + 0x10);
  if (h == (void*)0xffffffff) b78_fatal(0x511);
  uint32_t n;
  int ok = WriteFile(h, buf + 2, buf[0], &n, edi + 0x5c);
  uint32_t err = GetLastError();
  if (!ok) {
    if (err != 0x3e5) { FUN_6ff5e420(); Fog_E550(0, edi); return 0; }
  } else if (err != 0x3e5) return 1;
  *(uint32_t*)(edi + 0x70) |= 4;
  return 1;
}
