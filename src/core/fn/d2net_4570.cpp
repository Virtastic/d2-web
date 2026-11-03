// 1.13c D2Net+0x4570 _memcpy: d2net_n97::d2_memcpy (memmove semantics; non-overlapping vectors only).
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
namespace d2net_n97 { void* d2_memcpy(void* dst, const void* src, size_t n); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2net_4570(uint32_t* r) {
  r[4] = (uint32_t)(uintptr_t)d2net_n97::d2_memcpy((void*)(uintptr_t)r[0], (const void*)(uintptr_t)r[1], r[2]);
}
