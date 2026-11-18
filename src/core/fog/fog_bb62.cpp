// Prior art used: MSVC VC7.1 CRT __ShrMan (decompile cache, Fog+0xBB62); man[0] is the most significant dword.
#include <cstdint>

// 1.13c Fog+0x0BB62: __ShrMan(man, n). Shifts the 96-bit mantissa right by n bits, n%32 inside the dwords then n/32 whole dwords.
// Returns the EAX retail leaves: the dword-move loop's counter after it runs out, -1.
int32_t fog_shrman_bb62(uint32_t* man, int32_t n)
{
    const unsigned s = (unsigned)n % 32;
    uint32_t carry = 0;
    for (int i = 0; i < 3; ++i) {
        const uint32_t v = man[i];
        man[i] = (v >> s) | carry;
        carry = (v & ~(0xFFFFFFFFu << s)) << ((0x20 - s) & 0x1f);
    }
    const uint32_t* src = man + (2 - n / 32);
    int i = 2;
    for (; i >= 0; --i, --src)
        man[i] = (i < n / 32) ? 0 : *src;
    return i;
}
