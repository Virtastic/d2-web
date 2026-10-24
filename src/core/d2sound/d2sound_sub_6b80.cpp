// (alt copy; batch_67c0 carries the marker) D2sound 0x06B80 -- start a timed fade on a sound object (fastcall: ecx=obj, edx=target, stack=duration ms).
#include <cstdint>
struct D2soundEnv6B80 {
    int enabled;                       // DAT_6f9c658c
    uint32_t (*tick)();                // GetTickCount
    void (*lock)(); void (*unlock)();  // critical section 6f9c6530
    void (*fatal)(int strId);          // Ordinal_10265 + 10024 + _exit(-1)
};
void D2sound_sub_6B80(uint32_t* obj, uint32_t target, int32_t ms, const D2soundEnv6B80* e) {
    if (!obj) { e->fatal(0x39e); return; }
    if (!e->enabled) return;
    e->lock();
    obj[0x13] = 1; obj[0x14] = obj[0x0e]; obj[0x15] = target;
    uint32_t t = e->tick(); obj[0x16] = t; obj[0x17] = t + ms;
    e->unlock();
}
