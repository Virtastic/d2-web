// 1.13c Fog+0x1C9E0 (ordinal 10055): coarse seconds clock. The stamp {seconds, tick} at Fog+0x316D0 is read whole
// (Fog+0x1C9A0); when GetTickCount has moved more than 30000 ms past the stamp tick the wall clock (_time32) is re-read,
// the thread priority is read and set back to itself, and the stamp is restamped {time32(0), GetTickCount()} (Fog+0x1C9C0).
// Returns (now - stamp tick) / 1000 + stamp seconds, all unsigned, using the pre-refresh stamp.
#include <cstdint>

struct FogClockSeams {
    uint32_t (*tick)();                  // KERNEL32 GetTickCount
    int (*threadPriority)();             // GetCurrentThread + GetThreadPriority
    void (*setThreadPriority)(int p);    // GetCurrentThread + SetThreadPriority
    uint32_t (*time32)();                // Fog+0x1BDA _time32(NULL)
};

int fog_clock_seconds_1c9e0(uint32_t* stamp, const FogClockSeams* s) {
    const uint32_t now = s->tick();
    const uint32_t secs = stamp[0], base = stamp[1];
    if (now - base > 30000u) {
        const int p = s->threadPriority();
        if (p != 0x7fffffff) s->setThreadPriority(p);
        const uint32_t t = s->time32();
        stamp[0] = t;
        stamp[1] = s->tick();
    }
    return (int)((now - base) / 1000u + secs);
}
