// D2Net.dll MSVC 2003 CRT block 0x2557-0x317B. Win32/CRT services are function-pointer seams in Host.
// 0x2D12 0x2D2A 0x2E92 0x2EDC 0x2F49 0x317B had no cached decompile; written to the VS2003 CRT shape
// (_realloc, unlock(4) stubs, _msize, ___freetlocinfo) and to be re-checked once the decompile exists.
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace d2net_n83 {

struct Host {
  uint16_t* (*get_env_w)();                  // GetEnvironmentStringsW, null = unsupported
  int (*last_error)();                       // GetLastError
  char* (*get_env_a)();                      // GetEnvironmentStrings
  int (*wide_to_mb)(const uint16_t*, int, char*, int);  // WideCharToMultiByte(0,0,...)
  void (*free_env)(void*);                   // FreeEnvironmentStrings[AW]
  void* (*heap_create)(uint32_t, uint32_t, uint32_t);   // HeapCreate
  void (*heap_destroy)(void*);               // HeapDestroy
  int (*sbh_init)(int);                      // ___sbh_heap_init
  void* (*nh_malloc)(size_t, int);           // __nh_malloc
  void (*unlock)(int);                       // _mtunlock_
  void (*lock)(int);                         // __lock
  void (*rtl_unwind)(void*, void*, void*, void*);  // RtlUnwind
  void* (*heap_size_sbh)(void*);             // block size via SBH, null = not SBH
  void* (*sbh_realloc)(void*, size_t);       // sbh realloc, null = fail
  size_t (*heap_size)(void*, void*);         // HeapSize
  void* (*heap_realloc)(void*, void*, size_t);  // HeapReAlloc
  int (*callnewh)(size_t);
};

struct State {
  int envmode = 0;                 // DAT_6fbfb068
  int winplatform = 0, winmajor = 0;  // DAT_6fbfada4 / DAT_6fbfadb0
  void* heap = nullptr;            // DAT_6fbfb680
  int heap_mode = 0;               // DAT_6fbfb684
  int newh_set = 0;                // DAT_6fbfb0d8
};

// 1.13c D2NET+0x02C28
void* crt_malloc(State& s, const Host& h, size_t n) { return h.nh_malloc(n, s.newh_set); }

// 1.13c D2NET+0x02C3C
void global_unwind2(const Host& h, void* frame, void* target) { h.rtl_unwind(frame, target, nullptr, nullptr); }

// scope table entry, 12 bytes; registration record: +8 table, +0xc trylevel
struct ScopeEntry { int32_t prev; void (*filter)(); void (*handler)(); };
struct Reg { void* next; void* handler; ScopeEntry* table; int32_t trylevel; };

// 1.13c D2Net+0x02D12 (unwind helper: reload frame registers before a termination handler runs; seam is a no-op in wasm)
void local_unwind_restore_2d12() {}

// 1.13c D2NET+0x02C7E
void local_unwind2(Reg* r, int32_t stop) {
  for (;;) {
    ScopeEntry* t = r->table;
    int32_t lvl = r->trylevel;
    if (lvl == -1 || lvl == stop) break;
    r->trylevel = t[lvl].prev;
    if (t[lvl].filter == nullptr) {
      local_unwind_restore_2d12();
      t[lvl].handler();
    }
  }
}

// 1.13c D2NET+0x02E92
void unlock_4_2e92(const Host& h) { h.unlock(4); }

// 1.13c D2NET+0x02F49
void unlock_4_2f49(const Host& h) { h.unlock(4); }

// 1.13c D2NET+0x02EDC  _msize: SBH-owned block size else HeapSize
size_t crt_msize(State& s, const Host& h, void* p) {
  if (s.heap_mode == 3) {
    h.lock(4);
    void* sz = h.heap_size_sbh(p);
    h.unlock(4);
    if (sz) return reinterpret_cast<size_t>(sz) - 1;  // seam returns size+1 to keep null = not SBH
  }
  return h.heap_size(s.heap, p);
}

// 1.13c D2NET+0x02D2A  _realloc
void* crt_realloc(State& s, const Host& h, void* p, size_t n) {
  if (!p) return crt_malloc(s, h, n);
  if (n == 0) { std::free(p); return nullptr; }
  for (;;) {
    void* r = nullptr;
    if (n <= 0xffffffe0u) {
      if (s.heap_mode == 3) {
        h.lock(4);
        r = h.sbh_realloc(p, n);
        h.unlock(4);
      }
      if (!r) r = h.heap_realloc(s.heap, p, n ? n : 1);
    }
    if (r || s.newh_set == 0) return r;
    if (!h.callnewh(n)) return nullptr;
  }
}

// ___freetlocinfo input: reference counts and owned tables (null refcount = static table)
struct LocInfo {
  void* lconv; const void* lconv_c; long* intl_ref; long* mon_ref; long* num_ref;
  long* ctype1_ref; void* ctype1; void* pclmap; void* pcumap; void* lc_time; const void* lc_time_c;
  long* time_ref; void* lconv_mon; void* lconv_num;
};

// 1.13c D2NET+0x0317B
void freetlocinfo(LocInfo* p) {
  if (p->lconv != p->lconv_c && p->lconv && p->intl_ref && *p->intl_ref == 0) {
    std::free(p->intl_ref);
    if (p->mon_ref && *p->mon_ref == 0) { std::free(p->lconv_mon); std::free(p->mon_ref); }
    if (p->num_ref && *p->num_ref == 0) { std::free(p->lconv_num); std::free(p->num_ref); }
    std::free(p->lconv);
  }
  if (p->ctype1_ref && *p->ctype1_ref == 0) {
    std::free(p->ctype1_ref); std::free(p->pclmap); std::free(p->pcumap); std::free(p->ctype1);
  }
  if (p->lc_time != p->lc_time_c && p->time_ref && *p->time_ref == 0) {
    std::free(p->time_ref); std::free(p->lc_time);
  }
  std::free(p);
}

}  // namespace d2net_n83
