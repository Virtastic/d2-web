// src/core/storm/storm_batch_b143.cpp -- Storm batch 143 (decompile-derived; no spec/vector yet).
// Retail 32-bit addresses stay uint32_t; register-passed args (EAX/EDI) become explicit params.
#include <cstdint>
#include <cstdlib>
#include <cstring>

// Retail globals (DAT_*), modelled as plain statics.
static uint32_t g_pageSize;          // DAT_6fc43550
static bool g_dat43554, g_dat44d5c;  // DAT_6fc43554 / DAT_6fc44d5c
static void* g_crtLocks[32];         // DAT_6fc402c0 (stride 8 in retail: [n*2])
void (*Storm_LeaveCS)(void*) = nullptr;  // LeaveCriticalSection; null on wasm (single thread)
static uint32_t g_ctxSaved[3];       // DAT_6fc407d4/d8/dc

// 1.13c Storm+0x902a: CRT _unlock(n)
void Storm_SetCrtLock(int n, void* p) { g_crtLocks[n] = p; }   // FN probes pin a lock slot
extern "C" void Storm_Sub902A(int n) { if (Storm_LeaveCS) Storm_LeaveCS(g_crtLocks[n]); }
// 1.13c Storm+0x6d64
extern "C" void Storm_Sub6D64() { Storm_Sub902A(0xc); }
// 1.13c Storm+0x8386
extern "C" void Storm_Sub8386() { Storm_Sub902A(4); }

// 1.13c Storm+0xc03a: EAX=in_EAX, EBP-frame arg0 passed explicitly
extern "C" void Storm_SubC03A(uint32_t eax, uint32_t arg0, uint32_t ebp) {
  g_ctxSaved[1] = arg0; g_ctxSaved[0] = eax; g_ctxSaved[2] = ebp;
}

// 1.13c Storm+0x9198: CRT calloc. Small-block heap path (__sbh) not modelled: libc calloc covers it.
extern "C" void* Storm_Sub9198(uint32_t n, uint32_t sz) {
  if (n && sz > 0xffffffe0u / n) return nullptr;
  return calloc(n * sz ? n * sz : 1, 1);
}

// 1.13c Storm+0x1a6c0: page size rounded up to a power of two (SYSTEM_INFO.dwPageSize passed in)
extern "C" void Storm_Sub1A6C0(uint32_t dwPageSize) {
  uint32_t v = 1;
  g_pageSize = 1;
  while (v < dwPageSize) { v <<= 1; g_pageSize = v; }
}
// The 0x6FC43550 slot, for the function-mode probe.
extern "C" uint32_t Storm_Sub1A6C0_PageSize(void) { return g_pageSize; }

// 1.13c Storm+0x1a550: grow committed region of page (EDI) to newEnd (EAX). Retail calls
// VirtualAlloc(page+cm, size, MEM_COMMIT, PAGE_READWRITE) and returns 0 with the commit point unchanged
// when it returns NULL. The commit seam takes (address, size) and answers nonzero on success.
extern "C" uint32_t Storm_Sub1A550_commit(uint8_t* pg, int32_t newEnd, bool (*commit)(uintptr_t addr, uint32_t size)) {
  int32_t cm = *(int32_t*)(pg + 0x2c);
  uint32_t sz = (uint32_t)newEnd - (uint32_t)cm, al = *(uint32_t*)(pg + 0x28);
  uint32_t r = (al - 1) & sz;
  if (r) sz += al - r;
  if (*(uint32_t*)(pg + 0x30) < (uint32_t)cm + sz) sz = *(uint32_t*)(pg + 0x30) - (uint32_t)cm;
  if (!commit((uintptr_t)pg + (uint32_t)cm, sz)) return 0;
  *(int32_t*)(pg + 0x2c) = cm + (int32_t)sz;
  return 1;
}
// Engine callers keep the reserved-up-front model: the commit always succeeds.
static bool commit_reserved(uintptr_t, uint32_t) { return true; }
extern "C" uint32_t Storm_Sub1A550(uint8_t* pg, int32_t newEnd) {
  return Storm_Sub1A550_commit(pg, newEnd, commit_reserved);
}

// 1.13c Storm+0x1a6f0
extern "C" void Storm_Sub1A6F0(uint32_t n, int32_t* big, int32_t* total, int32_t* pad, int32_t* trk) {
  int32_t b = (!g_dat43554 && n < 0xfe88) ? 0 : 1;
  *big = b;
  int32_t t = (g_dat44d5c && !b) ? 1 : 0;
  *trk = t;
  if (b) n = 4;
  int32_t v = (t ? 2 : 0) + 8 + (int32_t)n;
  int32_t tot = ((-v) & 7) + v;
  *total = tot; *pad = tot - v;
}

// 1.13c Storm+0x1a770: rebuild free lists of page (EAX). Free-list heads at +0x38..+0x58 (9 u32 slots).
extern "C" void Storm_Sub1A770(uint8_t* pg) {
  uint32_t* tail[9];
  for (int i = 0; i < 9; i++) tail[i] = (uint32_t*)(pg + 0x38 + 4 * i);
  uint16_t* end = *(uint16_t**)(pg + 0x20);
  uint16_t* p = *(uint16_t**)(pg + 0x1c);
  uint16_t* run = nullptr;
  auto push = [&](uint16_t* b) {
    uint32_t c = *b >> 5; if (c > 7) c = 8;
    *tail[c] = (uint32_t)(uintptr_t)b;  // ponytail: retail 32-bit link; truncates on wasm64 heap addrs
    tail[c] = (uint32_t*)(b + 2);
  };
  if (p != end) {
    do {
      uint16_t* keep = run;
      if (((uint8_t*)p)[3] & 2) {
        p[2] = 0; p[3] = 0; keep = p;
        if (run) {
          if (p == (uint16_t*)((uint8_t*)run + *run) && (uint32_t)*p + *run < 0x10000) { *run += *p; keep = run; }
          else push(run);
        }
      }
      p = (uint16_t*)((uint8_t*)p + *p);
      run = keep;
    } while (p != end);
    if (run) push(run);
  }
  for (int i = 0; i < 9; i++) *tail[i] = 0;
  *(uint32_t*)(pg + 0x24) = 0;
}

// 1.13c Storm+0x28110: SFile callback registration under lock. Unverified: callees (0xEC20 list-insert, 0x1C580 alloc,
// critical section, callbacks LAB_15fd0/15f90) are hooks to be wired by the integrator; link-safe by default.
struct StormSfileHooks {
  void* (*alloc)(uint32_t, const char*, int, uint32_t);
  void (*insert)(void* cbA, void* cbB, void* list, uint32_t* rec);
  void (*lock)(); void (*unlock)();
  void *cbA, *cbB;
};
extern "C" StormSfileHooks Storm_SfileHooks = {};
extern "C" void* Storm_SfileList = nullptr;  // list at module +0x53118 (the FN probe pins it)
extern "C" uint32_t Storm_Sub28110(uint32_t p1, uint32_t p2, uint32_t p3) {
  StormSfileHooks& h = Storm_SfileHooks;
  if (h.lock) h.lock();
  if (!Storm_SfileList && h.alloc) Storm_SfileList = h.alloc(0x3134, "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP", 0x3b5, 0);
  uint32_t rec[5] = {p2, 0, p1, 0, p3};
  if (h.insert) h.insert(h.cbA, h.cbB, Storm_SfileList, rec);
  if (h.unlock) h.unlock();
  return 1;
}

// 1.13c Storm+0x11160 moved to storm_11160.cpp (retail layout).
