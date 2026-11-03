// 1.13c D2Net+0x1f43 unlock_4_1f43: h.unlock(4) through the n38 Host seam. r[0] = hooklog blob (@lg).
// Retail's _mtunlock_ (0x182B) is hooked; the probe records the call with the same layout.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
namespace d2net_n38 {
struct Host {
  void* (*sbh_alloc)(size_t);          // ___sbh_alloc_block, nullptr result = fall through to HeapAlloc
  bool (*sbh_free)(void*);             // find_block + free_block; true if the block was SBH-owned
  void (*lock)(int);                   // __lock(n)
  void (*unlock)(int);                 // _mtunlock_(n)
  int (*callnewh)(size_t);             // __callnewh
  int (*init_cs)(void*, int);          // ___crtInitCritSecAndSpinCount
  void (*del_cs)(void*);               // DeleteCriticalSection
  int (*file_type)(intptr_t);          // GetFileType, 0 = unknown
  intptr_t (*std_handle)(uint32_t);    // GetStdHandle, -1 = invalid
};
void unlock_4_1f43(const Host& h);
}
static uint32_t* g_lg;
static void rec(uint32_t rva, uint32_t s0) { uint32_t* w = g_lg + 1 + 10 * g_lg[0]++; for (int i = 0; i < 10; i++) w[i] = 0; w[0] = rva; w[6] = s0; }
static void ul(int n) { rec(0x182B, (uint32_t)n); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2net_1f43(uint32_t* r) {
  g_lg = (uint32_t*)(uintptr_t)r[0];
  for (int i = 0; i < 1 + 10 * 8; i++) g_lg[i] = 0;
  d2net_n38::Host h{};
  h.unlock = ul;
  d2net_n38::unlock_4_1f43(h);
  r[4] = 0;
}
