// 1.13c Fog+0x0D520 -- rate limiter: add `n` to a counter; over 1000 calls Fog+0xCEB0, else decay 200/s.
#include <cstdint>

namespace {
// Host seams for Win32 (GetTickCount, Enter/LeaveCriticalSection) and Fog+0xCEB0.
struct D520Host {
    uint32_t (*tick)();
    void (*lock)(void* cs);
    void (*unlock)(void* cs);
    int (*onLimit)(uint32_t arg);
};
D520Host g_d520Host = {};
}

// `self` is the object retail receives in EDI: +0x18 arg, +0x24 last tick, +0x28 counter, +0x2c critical section.
bool fog_sub_d520(uint8_t* self, int n) {
    uint32_t now = g_d520Host.tick();
    void* cs = self + 0x2c;
    uint32_t& lastTick = *(uint32_t*)(self + 0x24);
    uint32_t& count = *(uint32_t*)(self + 0x28);
    g_d520Host.lock(cs);
    count += n;
    g_d520Host.unlock(cs);
    if (count > 1000) return g_d520Host.onLimit(*(uint32_t*)(self + 0x18)) != 0;
    if (now - lastTick > 1000) {
        g_d520Host.lock(cs);
        lastTick = now;
        count = count < 200 ? 0 : count - 200;
        g_d520Host.unlock(cs);
    }
    return true;
}
