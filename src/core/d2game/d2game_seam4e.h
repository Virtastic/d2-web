// (own seam: d2game_seam.h holds the older D2GameSeam) Callee seam shared by the one-file D2Game ports: retail callees are reached through `D2Seam::fn`, which the
// FN probe wires to HookEnv::h (src/core/fn/hookenv.h) so every call is logged like the capture's hook recorder. Arguments are the
// registers and stack words retail hands the callee (those the descriptor's hook REGS log; others are passed as 0).
#pragma once
#include <cstdint>
#include <cstring>
struct D2Seam {
    void* ctx;
    uint32_t (*fn)(void* ctx, uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s,
                   uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7);
    // hookblk: log n dwords at p (a stack block handed to the callee just called), 9 per record
    void (*bl)(void* ctx, uint32_t rva, uint32_t p, uint32_t n) = nullptr;
    void blk(uint32_t rva, const void* p, uint32_t n) const { if (bl) bl(ctx, rva, (uint32_t)(uintptr_t)p, n); }
    uint32_t call(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0,
                  uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0, uint32_t s7 = 0) const {
        return fn(ctx, rva, a, c, d, b, s, s0, s1, s2, s3, s4, s5, s6, s7);
    }
};
// Retail-offset field access: `a` is a 32-bit retail-style address (a blob in the replay).
static inline uint32_t d2rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t d2rd16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline uint32_t d2rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline void d2wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
static inline void d2wr16(uint32_t a, uint32_t v) { uint16_t x = (uint16_t)v; memcpy((void*)(uintptr_t)a, &x, 2); }
static inline void d2wr8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }
