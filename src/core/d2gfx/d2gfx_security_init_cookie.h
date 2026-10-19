#pragma once
#include <cstdint>

struct D2GfxCookieWords { uint32_t low, high; };
// Native callbacks for Win32 queries; capture/replay supplies pinned results.
struct D2GfxCookieEnv {
    D2GfxCookieWords (*system_time_filetime)();
    uint32_t (*process_id)();
    uint32_t (*thread_id)();
    uint32_t (*tick_count)();
    D2GfxCookieWords (*performance_counter)();
};
// cookie points to the caller's module state at retail RVA 0x106E0,
// including D2GfxCrtTinyHost::stack_cookie used by the cookie checker.
extern "C" void d2gfx_security_init_cookie(uint32_t* cookie, const D2GfxCookieEnv* env);
