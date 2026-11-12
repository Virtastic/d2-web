// 1.13c D2Game+0xE7050 -- EAX = a stale register, ECX = quest record q. With the q +0xC byte 0 and +9 nonzero, +0xC becomes 1 and the +0xA8 handler word becomes the function 0xE6910. Returns EAX with its low byte 1.
#include <cstdint>
#include <cstring>
namespace d2g {
uint32_t fe7050(uint8_t* q, uint32_t eax)
{
    if (q[0xc] == 0 && q[9] != 0) {
        q[0xc] = 1;
        const uint32_t h = 0xB2000000u | 0xe6910;
        memcpy(q + 0xa8, &h, 4);
    }
    return (eax & ~0xffu) | 1;
}
}
