// 1.13c D2Net+0x2e92 unlock_4_2e92: h.unlock(4) through the n83 Host seam. r[0] = hooklog blob (@lg).
// Retail's _mtunlock_ (0x182B) is hooked; the probe records the call with the same layout.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
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
void unlock_4_2e92(const Host& h);
}
static uint32_t* g_lg;
static void rec(uint32_t rva, uint32_t s0) { uint32_t* w = g_lg + 1 + 10 * g_lg[0]++; for (int i = 0; i < 10; i++) w[i] = 0; w[0] = rva; w[6] = s0; }
static void ul(int n) { rec(0x182B, (uint32_t)n); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2net_2e92(uint32_t* r) {
  g_lg = (uint32_t*)(uintptr_t)r[0];
  for (int i = 0; i < 1 + 10 * 8; i++) g_lg[i] = 0;
  d2net_n83::Host h{};
  h.unlock = ul;
  d2net_n83::unlock_4_2e92(h);
  r[4] = 0;
}
