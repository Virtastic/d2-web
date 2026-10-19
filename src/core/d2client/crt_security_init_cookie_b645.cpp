#include "crt_security_init_cookie_b645.h"

// 1.13c D2Client+0x0B645: initialize the module cookie at +0xDB780.
// Host query results are supplied through native callbacks so replay can pin
// every OS result without replacing retail's branches or 32-bit XOR arithmetic.
extern "C" void d2_client_security_init_cookie_b645(const D2ClientCookieEnv* env) {
    constexpr uint32_t initial = 0xBB40E64Eu;
    if (d2_client_stack_cookie_db780 != 0 && d2_client_stack_cookie_db780 != initial)
        return;
    const auto time = env->system_time_filetime();
    const uint32_t process = env->process_id();
    const uint32_t thread = env->thread_id();
    const uint32_t tick = env->tick_count();
    const auto counter = env->performance_counter();
    const uint32_t cookie = time.high ^ time.low ^ process ^ thread ^ tick ^
                            counter.high ^ counter.low;
    d2_client_stack_cookie_db780 = cookie == 0 ? initial : cookie;
}
