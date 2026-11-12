// 1.13c D2Win+0x135e0 -- stdcall(msg): msg[0] = type-1 control (null: fatal 0x739; other type: fatal 0x73a). Clears its flag word +0x280 and returns 1 when it was set.
#include <cstdint>
#include <cstdlib>
extern "C" uint32_t d2win_135e0(const uint32_t* msg)
{
    uint32_t* c = (uint32_t*)(uintptr_t)msg[0];
    if (!c || c[0] != 1) abort();
    if (!c[0xa0]) return 0;
    c[0xa0] = 0;
    return 1;
}
