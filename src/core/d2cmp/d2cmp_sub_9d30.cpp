// 1.13c D2CMP+0x09D30 -- nearest palette entry (squared RGB distance) over `count` 4-byte entries.
#include <cstdint>
// 1.13c D2CMP+0x09D30
// Entry bytes [0],[1],[2] are compared to (p3,p4,p5); first minimum wins. count==0 returns 0;
// otherwise the result keeps count's bits 8+ and replaces the low byte with the index (retail quirk).
uint32_t d2cmp_sub_9d30(const uint8_t* pal, uint32_t count, int32_t p3, int32_t p4, int32_t p5) {
    if (count == 0) return 0;
    uint32_t best = 0xffffffffu, idx = 0;
    for (uint32_t i = 0; i < count; ++i, pal += 4) {
        uint32_t a = (uint32_t)pal[0] - (uint32_t)p3, b = (uint32_t)pal[1] - (uint32_t)p4, c = (uint32_t)pal[2] - (uint32_t)p5;
        uint32_t d = c * c + b * b + a * a;
        if (d < best) { best = d; idx = i; }
    }
    return (count & ~0xffu) | (idx & 0xffu);
}
