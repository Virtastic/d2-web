// 1.13c D2Net+0x2C28 _malloc (src/core/d2net/d2net_crt_batch_2770.cpp): newmode is the second argument of our port;
// retail reads its global 0xB0D8 (0 in init). r[0] size, r[1] newmode, r[2] = @lg hooklog blob.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
extern "C" void* d2net_malloc_2c28(size_t n, int newmode, void* (*nh_malloc)(size_t, int));
static uint32_t* g_lg; static uint32_t g_n;
static void* rec2bfc(size_t n, int newmode) {
  uint32_t* w = g_lg + 1 + 10 * g_n++;
  for (int i = 0; i < 10; i++) w[i] = 0;
  w[0] = 0x2BFC; w[6] = (uint32_t)n; w[7] = (uint32_t)newmode;
  return (void*)(uintptr_t)0x1000;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2net_2c28(uint32_t* r) {
  g_lg = (uint32_t*)(uintptr_t)r[2]; g_n = 0;
  for (int i = 0; i < 1 + 10 * 8; i++) g_lg[i] = 0;
  r[4] = (uint32_t)(uintptr_t)d2net_malloc_2c28((size_t)r[0], (int)r[1], rec2bfc);
  g_lg[0] = g_n;
}
