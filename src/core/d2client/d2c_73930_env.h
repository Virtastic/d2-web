// Callee seam shared by the D2Client skill-cast helpers 0x73930..0x73DF0 . Every callee the retail
// function reaches goes through call(): rva (a D2Client thunk / function), the registers it reads and its pushed stack words.
// blk() is the capture's `hookblk` (the dwords of the block a callee was handed). mem() follows a 32-bit retail address.
#pragma once
#include <cstdint>
#include <cstring>

struct D2c73Env {
    void* ctx;
    uint32_t (*call)(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi,
                     uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    void (*blk)(void* ctx, uint32_t rva, const uint32_t* p, uint32_t n);
    uint8_t* (*mem)(void* ctx, uint32_t addr);
    uint32_t tab;   // sgptDataTables: +0xBC/+0xC4 skills, +0xB98/+0xBA0 skill descs, +0xB64/+0xB6C missiles
    uint32_t bm;    // the bytes [0x6FB7EE50] points at (gdwBitMasks; byte 0 is tested)
};

namespace d2c73 {
inline uint32_t rd32(const D2c73Env* e, uint32_t a) { uint32_t v; memcpy(&v, e->mem(e->ctx, a), 4); return v; }
inline int32_t rd16s(const D2c73Env* e, uint32_t a) { int16_t v; memcpy(&v, e->mem(e->ctx, a), 2); return v; }
inline uint32_t rd8(const D2c73Env* e, uint32_t a) { return *e->mem(e->ctx, a); }
inline void wr32(const D2c73Env* e, uint32_t a, uint32_t v) { memcpy(e->mem(e->ctx, a), &v, 4); }
inline uint32_t ptr(const void* p) { return (uint32_t)(uintptr_t)p; }
inline uint32_t C(const D2c73Env* e, uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t di = 0,
                  uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0) {
    return e->call(e->ctx, rva, a, c, d, b, s, di, s0, s1, s2, s3, s4);
}
// skill record (0x3C stride, count +0xC4, base +0xBC) for index i, or 0
inline uint32_t skillRec(const D2c73Env* e, int32_t i) {
    if (i < 0 || i >= (int32_t)rd32(e, e->tab + 0xc4)) return 0;
    return (uint32_t)i * 0x3cu + rd32(e, e->tab + 0xbc);
}
// skill-desc record (0x23C stride, count +0xBA0, base +0xB98) for index i, or 0
inline uint32_t descRec(const D2c73Env* e, int32_t i) {
    if (i < 0 || i >= (int32_t)rd32(e, e->tab + 0xba0)) return 0;
    return (uint32_t)i * 0x23cu + rd32(e, e->tab + 0xb98);
}
}
