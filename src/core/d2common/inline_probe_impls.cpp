// Engine implementations behind FN probes d2common_{34690,38020,3c000,34ce0,34d10,34cb0,36c20,23be0,22510,306d0}.
// Operate on retail's 32-bit image through the low 32 bits of a wasm address (same as sub_34d40.cpp).
#include <cstdint>
#define P(x) ((uint8_t*)(uintptr_t)(x))
// 1.13c D2Common+0x34690
uint32_t D2Common_34690(uint32_t p) { return *P(p + 0x93) + 1; }
// 1.13c D2Common+0x38020
void D2Common_38020(uint32_t pp, int32_t v) {
    uint8_t* p = P(pp); if (!p) return;
    if (v > 0) *(uint32_t*)(p + 0x10) |= 2;
    *(int32_t*)(p + 0x18) = v;
}
// 1.13c D2Common+0x3c000
uint32_t D2Common_3C000(uint32_t u) {
    if (!u) return 0;
    uint32_t b = *(uint32_t*)(P(u) + 0x10), c = *(uint32_t*)(P(b) + 0x58);
    return *(uint32_t*)(P(c) + 0x1d0);
}
// 1.13c D2Common+0x34ce0
void D2Common_34CE0(uint32_t p, uint32_t v) { *(uint32_t*)(P(p) + 0x88) = v; }
// 1.13c D2Common+0x34d10
void D2Common_34D10(uint32_t p, uint32_t v) { *(uint32_t*)(P(p) + 0x84) = v; }
// 1.13c D2Common+0x34cb0
void D2Common_34CB0(uint32_t pp) { uint8_t* p = P(pp); *(uint32_t*)(p + 0x34) &= 0xffffffdfu; *(uint32_t*)(p + 0x28) = 0; }
// 1.13c D2Common+0x36c20
void D2Common_36C20(uint32_t pDst, uint32_t v) { *(uint32_t*)P(pDst) = v; }
// 1.13c D2Common+0x23be0
void D2Common_23BE0(uint32_t pu) {
    uint32_t* u = (uint32_t*)P(pu);
    uint32_t* p = (u && u[0] == 4) ? (uint32_t*)P(u[5]) : 0;
    if (p) { p[1] = 1; p[2] = 0x29a; }
}
// 1.13c D2Common+0x22510
void D2Common_22510(uint32_t pu, uint32_t v) {
    const uint32_t* u = (const uint32_t*)P(pu);
    if (u[0] == 4 && u[5]) *(uint32_t*)(P(u[5]) + 0xC) = v;
}
// 1.13c D2Common+0x306d0
void D2Common_306D0(uint32_t pu) {
    const uint8_t* u = P(pu);
    if (!u || *(const uint32_t*)u != 0) return;
    uint8_t* pd = P(*(const uint32_t*)(u + 0x14));
    const uint8_t* rec = P(*(const uint32_t*)(P(*(const uint32_t*)(u + 0xA8)) + 0xC));
    *(int32_t*)(pd + 0x70) = rec ? (int32_t)*(const int16_t*)P(*(const uint32_t*)rec) : 0;
    if (rec) *(uint32_t*)(pd + 0x78) = *(const uint32_t*)(rec + 0x34);
}
