// D2Common 1.13c batch. Copies up to 4 chars of src into dst, space-padding after the first NUL.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x1B9D0: dst in EAX, src in ECX (retail register args).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1b9d0(char* dst, const char* src) {
 int32_t i = 0;
 for (; i < 4 && src[i]; ++i) dst[i] = src[i];
 for (; i < 4; ++i) dst[i] = ' ';
}
}
