// 1.13c D2Lang+0x1580
#include <emscripten.h>
#include <cstdint>
extern "C" int d2lang_strncmp(const char*, const char*, size_t);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2lang_1580(uint32_t* r) { r[4] = (uint32_t)d2lang_strncmp((const char*)(uintptr_t)r[0], (const char*)(uintptr_t)r[1], r[2]); }
