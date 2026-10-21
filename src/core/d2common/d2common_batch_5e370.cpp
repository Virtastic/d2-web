// D2Common 1.13c batch. Copy a 4-char code (src in ECX, dst in EAX), space-pad after the NUL.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x5E370
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5e370(char* dst, const char* src) {
 int i = 0;
 for (; i < 4 && src[i]; ++i) dst[i] = src[i];
 for (; i < 4; ++i) dst[i] = ' ';
}
}
