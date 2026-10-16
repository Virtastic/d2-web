// Pure D2Client leaves, batch 7. 1.13c byte offsets, 32-bit pointer fields (wasm64 blobs).
#include <cstdint>
#include <cstring>
static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static uint8_t* ptr(uint32_t v) { return (uint8_t*)(uintptr_t)v; }

// 1.13c D2Client+0x21E80: eax=obj. flags=word[obj->[+0x18]+6]; first set bit in order 0x20,0x08,0x10,0x40,0x80,0x400 -> 1,2,3,4,6,5.
uint32_t D2Client_FlagClass18_6(const uint8_t* o) {
    if (!rd(o, 0x18)) return 0;   // retail never tests o itself
    uint16_t f; std::memcpy(&f, ptr(rd(o, 0x18)) + 6, 2);
    if (f & 0x20) return 1;
    if (f & 0x08) return 2;
    if (f & 0x10) return 3;
    if (f & 0x40) return 4;
    if (f & 0x80) return 6;
    if (f & 0x400) return 5;
    return 0;
}
// 1.13c D2Client+0x2C830: eax=n. 1->1, 2->9, 3,4->3, 5->2, else 0.
uint32_t D2Client_Map2C830(uint32_t n) {
    switch (n) { case 1: return 1; case 2: return 9; case 3: case 4: return 3; case 5: return 2; default: return 0; }
}
// 1.13c D2Client+0x2C870: eax=n. Sets ax only: high 16 bits of the result are those of (n-1).
uint32_t D2Client_Map2C870(uint32_t n) {
    uint32_t k = n - 1, hi = k & 0xFFFF0000u;
    switch (k) {
        case 0: return hi | 0x10a1; case 1: return hi | 0x10a3; case 2: return hi | 0x10c3;
        case 3: return hi | 0x10a2; case 4: return hi | 0x10a4; case 5: return hi | 0xdc4;
        default: return hi;
    }
}
// 1.13c D2Client+0x42400: ecx=a, edx=b (signed). (b<7) + (a>1) + (a>9) + (a>0x13) + (a>0x1d).
uint32_t D2Client_Tier42400(int32_t a, int32_t b) {
    return (uint32_t)(b < 7) + (a > 1) + (a > 9) + (a > 0x13) + (a > 0x1d);
}
// 1.13c D2Client+0x4C0B0: ecx=unit. dwType==1 and pData(+0x14) -> (byte[pData+0x16] >> 1) & 1, else 0.
uint32_t D2Client_PlayerFlag16Bit1(const uint8_t* u) {
    return (u && rd(u, 0) == 1 && rd(u, 0x14)) ? (ptr(rd(u, 0x14))[0x16] >> 1) & 1 : 0;
}
