// Pure D2Client leaves, batch 6. 1.13c byte offsets, 32-bit pointer fields (wasm64 blobs).
#include <cstdint>
#include <cstring>
static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static uint8_t* ptr(uint32_t v) { return (uint8_t*)(uintptr_t)v; }

// 1.13c D2Client+0x4C2C0: eax=unit. dwType==1 and pData(+0x14) -> pData+0x34, else 0.
uint32_t D2Client_PlayerDataPlus34(const uint8_t* u) {
    return (u && rd(u, 0) == 1 && rd(u, 0x14)) ? rd(u, 0x14) + 0x34 : 0;
}
// 1.13c D2Client+0xC7C30: eax=unit. unit->[+0x54]->[+0x30] |= 1, null-guarded at each level.
uint32_t D2Client_SetFlag1Via54_30(const uint8_t* u) {   // returns leftover eax
    if (!u) return 0;
    if (!rd(u, 0x54)) return 0;
    uint32_t qa = rd(ptr(rd(u, 0x54)), 0x30);
    if (qa) { uint32_t f = rd(ptr(qa), 0) | 1; std::memcpy(ptr(qa), &f, 4); }
    return qa;
}
// 1.13c D2Client+0xA5340: eax=node. Follow dword[0x3A] while node && type(dword 0) != 5; returns the stop node.
uint32_t D2Client_FindType5(uint32_t n) {
    while (n && rd(ptr(n), 0) != 5) n = rd(ptr(n), 0x3A * 4);
    return n;
}
// 1.13c D2Client+0x69770/69830/69850: fastcall(ecx, edx=obj). obj->[+0x54] byte block: bytes 0,2,4.. < n cleared.
uint32_t D2Client_ClearEvenBytes54(const uint8_t* obj, uint32_t n) {   // returns leftover eax
    if (!obj || !rd(obj, 0x54)) return 0;
    for (uint32_t i = 0; i < n; i += 2) ptr(rd(obj, 0x54))[i] = 0;
    return rd(obj, 0x54);
}
