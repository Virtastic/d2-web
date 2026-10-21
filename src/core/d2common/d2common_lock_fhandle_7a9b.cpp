#include <cstdint>

// 1.13c D2Common+0x07A9B (148 B): __lock_fhandle. Same shape as BNClient+0x0790D. ioinfo record =
// table[h >> 5] + (h & 0x1F) * 0x24; lockinitflag at +8, critical section at +0xC. If the flag is 0,
// take CRT lock 10, init the section (4000 spins) if still unset, bump the flag, release lock 10; an
// init failure returns 0 without entering. Otherwise enter the section and return 1.
struct D2CommonIoinfoEnv {
    uint8_t* (*ioinfo_entry)(int32_t handle);
    void     (*lock)(int32_t n);                            // 0x2EF9 __lock
    void     (*unlock)(int32_t n);                          // 0x7B32 __unlock(10) thunk
    int32_t  (*init_crit_sec)(void* cs, uint32_t spin);     // ___crtInitCritSecAndSpinCount
    void     (*enter)(void* cs);                            // EnterCriticalSection
};

int32_t d2common_lock_fhandle_7a9b(const D2CommonIoinfoEnv& e, int32_t h)
{
    uint8_t* const pio = e.ioinfo_entry(h);
    int32_t* const flag = (int32_t*)(pio + 8);
    void* const cs = pio + 0xC;
    if (*flag == 0) {
        e.lock(10);
        bool ok = true;
        if (*flag == 0) {
            if (e.init_crit_sec(cs, 4000) == 0) ok = false; else ++*flag;
        }
        e.unlock(10);
        if (!ok) return 0;
    }
    e.enter(cs);
    return 1;
}
