// 1.13c Fog+0x90c0 _strncmp: unsigned byte compare of at most n bytes, -1 / 0 / 1.
#include <cstdint>
extern "C" int32_t fog_strncmp_90c0(const uint8_t* a, const uint8_t* b, uint32_t n) {
    for (; n; --n, ++a, ++b) { if (*a != *b) return *b > *a ? -1 : 1; if (!*a) break; }
    return 0;
}
