#pragma once
#include <cstdint>
struct D2GameCookieWords { uint32_t low, high; };
// Native OS queries; this is neither serialized state nor an address-token registry.
struct D2GameCookieEnv {
    D2GameCookieWords (*system_time_filetime)();
    uint32_t (*process_id)();
    uint32_t (*thread_id)();
    uint32_t (*tick_count)();
    D2GameCookieWords (*performance_counter)();
};
extern "C" void d2_game_security_init_cookie_9bb0(const D2GameCookieEnv* env);
