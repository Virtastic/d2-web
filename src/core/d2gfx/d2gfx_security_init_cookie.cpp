#include "d2gfx_security_init_cookie.h"

// Prior art: D2MOO/source/D2Gfx/src/DllMain.cpp does not implement CRT cookie
// initialization. Cross-checked the sibling D2Client initializer; the cached
// 1.13c D2Gfx body is authoritative. OS queries remain explicit native seams.
// 1.13c D2Gfx+0x06026
extern "C" void d2gfx_security_init_cookie(uint32_t* cookie, const D2GfxCookieEnv* env) {
    constexpr uint32_t initial = 0xBB40E64Eu;
    if (*cookie != 0 && *cookie != initial) return;
    const auto time = env->system_time_filetime();
    const uint32_t process = env->process_id();
    const uint32_t thread = env->thread_id();
    const uint32_t tick = env->tick_count();
    const auto counter = env->performance_counter();
    *cookie = time.high ^ time.low ^ process ^ thread ^ tick ^ counter.high ^ counter.low;
    if (*cookie == 0) *cookie = initial;
}
