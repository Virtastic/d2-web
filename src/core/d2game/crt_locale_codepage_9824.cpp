// 1.13c D2Game+0x09824 (71 B, callers 0x6B8D, 0x75E8): locale ANSI codepage.
// GetLocaleInfoA(locale, LOCALE_IDEFAULTANSICODEPAGE=0x1004, buf[6], 6); 0 chars -> -1, else _atol(buf).
// The Win32 call is a host seam (returns the char count, fills buf).
#include <cstdint>
#include <emscripten.h>

extern "C" int32_t d2_fn_d2game_9B28(const char* str, const void* env);

struct D2GameLocaleCpEnv {
    int32_t (*getLocaleInfoA)(uint32_t lcid, uint32_t type, char* buf, int32_t cch);
};

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_9824(uint32_t locale, const D2GameLocaleCpEnv* env) {
    char buf[7] = {0};  // 6 chars + forced NUL (local_a = 0)
    if (env->getLocaleInfoA(locale, 0x1004, buf, 6) == 0) return -1;
    return d2_fn_d2game_9B28(buf, nullptr);
}
