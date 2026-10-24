#include <cstdint>

// __onexit(func) env seam: lock/unlock/lk are host hooks.
struct D2CommonOnexitEnv {
    void (*lock)();
    uintptr_t (*onexitLk)(uintptr_t);
    void (*unlock)();
};

// 1.13c D2Common+0x02FD2 __onexit(func): take the CRT lock (+0x19B6), run __onexit_lk (+0x2F2A)
// on func, release the lock (+0x3004), return the lk result. Env seam: lock/unlock/lk are host hooks.
uintptr_t d2common_onexit_2fd2(const D2CommonOnexitEnv& e, uintptr_t func)
{
    e.lock();
    uintptr_t r = e.onexitLk(func);
    e.unlock();
    return r;
}
