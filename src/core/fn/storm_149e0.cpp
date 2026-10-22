// src/core/fn/storm_149e0.cpp -- 1.13c Storm+0x149e0 (wait until ids[] flags set in 6FC45178[16])
#include <emscripten.h>
#include <cstdint>
namespace { uint32_t g_flags[16]; void (*g_pump[3])(); }  // pump = callees 124B0, 14250, 13170 (unported)
// 1.13c Storm+0x149E0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t Storm_Sub149E0(int32_t any, uint32_t timeout, const uint32_t* ids, uint32_t n) {
  for (auto& f : g_flags) f = 0;
  uint32_t t0 = (uint32_t)emscripten_get_now();
  for (bool first = true;; first = false) {
    if (!first) emscripten_sleep(10);
    for (auto p : g_pump) if (p) p();
    if (!n) return 1;
    uint32_t hit = 0;
    for (uint32_t i = 0; i < n; i++) if (ids[i] < 16 && g_flags[ids[i]]) hit++;
    if (n <= hit) return 1;
    if (hit && !any) return 1;
    if (timeout != 0xffffffffu && timeout <= (uint32_t)emscripten_get_now() - t0) return 0;
  }
}
