// 1.13c D2Net+0x5881 _atol: d2net_n93::crt_atol on the C locale.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
namespace d2net_n93 { long crt_atol(const char* s); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2net_5881(uint32_t* r) {
  r[4] = (uint32_t)d2net_n93::crt_atol((const char*)(uintptr_t)r[0]);
}
