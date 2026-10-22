#include <cstdint>

// Prior art: none in D2MOO (CRT); sibling d2gfx_security_init_cookie.cpp, same VS2003 body.
// OS queries are explicit args (capture/replay pins them).
// 1.13c BNClient+0x7264
extern "C" void bnclient_security_init_cookie(uint32_t* cookie, uint32_t ft_low, uint32_t ft_high,
                                              uint32_t pid, uint32_t tid, uint32_t tick,
                                              uint32_t qpc_low, uint32_t qpc_high) {
    constexpr uint32_t initial = 0xBB40E64Eu;
    if (*cookie != 0 && *cookie != initial) return;
    *cookie = ft_high ^ ft_low ^ pid ^ tid ^ tick ^ qpc_high ^ qpc_low;
    if (*cookie == 0) *cookie = initial;
}
