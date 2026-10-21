// D2Common 1.13c batch. Copies a NUL-terminated string of up to 4 chars into dst, padding with ' '.
// Oddity: Ghidra shows dst as the hidden EAX register argument; src is ECX (param_1).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x7C5F0
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7c5f0(char* dst, const char* src) {
 int i = 0;
 for (; i < 4 && src[i]; ++i) dst[i] = src[i];
 for (; i < 4; ++i) dst[i] = ' ';
}
}
