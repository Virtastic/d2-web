#pragma once
#include <cstdint>

// Native host callbacks, never serialized retail x86 addresses. Each two-word
// result is low word followed by high word, independent of native pointer width.
struct D2ClientCookieWords { uint32_t low, high; };
struct D2ClientCookieEnv {
    D2ClientCookieWords (*system_time_filetime)();
    uint32_t (*process_id)();
    uint32_t (*thread_id)();
    uint32_t (*tick_count)();
    D2ClientCookieWords (*performance_counter)();
};
extern uint32_t d2_client_stack_cookie_db780;
extern "C" void d2_client_security_init_cookie_b645(const D2ClientCookieEnv* env);
