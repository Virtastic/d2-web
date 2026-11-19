// 1.13c Storm+0x3D00 (EAX = need, stack = the stream): the grow step for a stream without one. From 0x100 up the step 0x100 is stored
// at +0xC and returned; below, the highest power of two <= need (1 for 0).
#include <cstdint>
extern "C" uint32_t Storm13c_3D00(uint32_t n, uint32_t* obj) {
    if (n >= 256) { obj[3] = 256; return 256; }
    if (n == 0) return 1;
    while (n & (n - 1)) n &= n - 1;
    return n;
}
