// Shared seam for the D2Client 0xB0000.. ports written from the 1.13c disassembly (one file per function).
// Addresses are the retail 32-bit address space; every imported ordinal / callee goes through h() with the
// registers and stack words the retail call site passes, so a vector can record exactly that.
#pragma once
#include <cstdint>
namespace cl {
struct Env {
    void* ctx;
    uint32_t (*h)(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi,
                  uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    uint32_t (*rd)(void* ctx, uint32_t addr);            // 32-bit little-endian load
    void (*wr)(void* ctx, uint32_t addr, uint32_t v);
    uint32_t dt;                                         // *sgptDataTables  (D2Client slot 0xCE474, twice-indirect)
    uint32_t bm;                                         // gdwBitMasks      (D2Client slot 0xCEE50)
    uint32_t self = 0;                                   // playerSelf       (D2Client slot 0x11BBFC)
    uint32_t R(uint32_t a) const { return rd(ctx, a); }
    int32_t  R16s(uint32_t a) const { return (int16_t)(rd(ctx, a) & 0xffff); }
    uint32_t R8(uint32_t a) const { return rd(ctx, a) & 0xff; }
    void     W(uint32_t a, uint32_t v) const { wr(ctx, a, v); }
    uint32_t H(uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0,
               uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0) const {
        return h(ctx, rva, a, c, d, b, s, s0, s1, s2, s3, s4);
    }
    // missiles/monsters table row (stride 0x1A4) by index, 0 when out of range (the 0xB64 / 0xB6C pair)
    uint32_t Row(int32_t i) const {
        if (i < 0 || (int32_t)R(dt + 0xb6c) <= i) return 0;
        return R(dt + 0xb64) + (uint32_t)i * 0x1a4;
    }
    uint32_t UnitRow(uint32_t u) const { return u ? Row((int32_t)R(u + 4)) : 0; }
};
}  // namespace cl
