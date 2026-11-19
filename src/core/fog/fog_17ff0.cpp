// 1.13c Fog+0x17FF0, 0x18050, 0x180B0: the base-255 four-byte codec.
#include <cstdint>
// 1.13c Fog+0x17FF0: o[0..3] = the four base-255 digits of v, each + 1; returns the residue EAX (the last quotient / 255).
uint32_t fog_sub_17ff0(uint8_t* o, uint32_t v) {
    o[0] = (uint8_t)(v % 255 + 1);
    o[1] = (uint8_t)(v / 255 % 255 + 1);
    o[2] = (uint8_t)((uint32_t)(((uint64_t)0x81018203u * v) >> 32 >> 15) % 255 + 1);
    uint32_t t = (uint32_t)(((uint64_t)0x103060a1u * v) >> 32 >> 20);
    o[3] = (uint8_t)(t % 255 + 1);
    return t / 255;
}
// 1.13c Fog+0x18050: decodes the four digits back (the inverse of 0x17FF0).
uint32_t fog_sub_18050(const uint8_t* p) {
    uint32_t v = ((uint32_t)p[3] * 255 + p[2]) * 255 + p[1];
    return v * 255 + p[0] - 0xfe0200u;
}
// 1.13c Fog+0x180B0: two 7-bit digits.
uint32_t fog_sub_180b0(const uint8_t* p) {
    return ((uint32_t)(p[1] & 0x7f) << 7) + (p[0] & 0x7f);
}
