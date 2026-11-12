// 1.13c BNClient+0x131F0 -- the cache table decipher. EAX = byte count, ECX = key, EDX = buffer. It walks count/4 dwords:
// the running sum (seed 0xEEEEEEEE) takes the crypt table word 0x400 + (key & 0xFF), the dword is xored with sum + key, the
// sum becomes sum * 33 + 3 + plain and the key rotates as in the cipher at 0x13190. Returns the sum.
#include <cstdint>
#include <cstring>
namespace bnclient {
uint32_t f131f0(const uint32_t* crypt, uint32_t bytes, uint32_t key, uint8_t* p)
{
    uint32_t sum = 0xEEEEEEEEu;
    for (uint32_t n = bytes >> 2; n; --n, p += 4) {
        sum += crypt[0x400 + (key & 0xFF)];
        uint32_t v;
        memcpy(&v, p, 4);
        v ^= sum + key;
        sum = sum * 0x21 + v + 3;
        memcpy(p, &v, 4);
        key = (key >> 11) | ((~key << 21) + 0x11111111u);
    }
    return sum;
}
}
