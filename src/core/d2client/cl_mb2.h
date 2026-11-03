// Seam for the D2Client 0x2B4C0..0x2BE30 skill-start ports written from the 1.13c disassembly (one file per function).
// Every callee goes through h() with the registers and stack words the retail call site passes (up to seven stack words),
// so a function-probe vector records exactly that. Addresses are the retail 32-bit address space.
#pragma once
#include <cstdint>
namespace clm {
struct Env {
    void* ctx;
    uint32_t (*h)(void* ctx, uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s,
                  uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
    uint32_t (*rd)(void* ctx, uint32_t addr);      // 32-bit little-endian load
    void (*wr)(void* ctx, uint32_t addr, uint32_t v);
    uint32_t (*g)(void* ctx, uint32_t rva);        // a module-global dword by its retail RVA
    void (*blk)(void* ctx, uint32_t rva, uint32_t p, uint32_t n);   // the `C` block records: n dwords at p after the call record
    void (*ed)(void* ctx, uint32_t v);             // EDI at the next h() (an out-pointer the callee fills)
    void (*edi)(void* ctx, uint32_t rva, uint32_t v);   // REGS `J`: EDI logged as an argument
    uint32_t dt;                                   // *sgptDataTables
    uint32_t R(uint32_t a) const { return rd(ctx, a); }
    int32_t R16s(uint32_t a) const { return (int16_t)(rd(ctx, a) & 0xffff); }
    uint32_t R8(uint32_t a) const { return rd(ctx, a) & 0xff; }
    void W(uint32_t a, uint32_t v) const { wr(ctx, a, v); }
    uint32_t G(uint32_t rva) const { return g(ctx, rva); }
    static uint32_t Mod(uint32_t rva) { return 0xB2000000u | rva; }   // a module address as the capture logs it
    uint32_t H(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t s0 = 0, uint32_t s1 = 0,
               uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0) const {
        return h(ctx, rva, a, c, d, b, s, s0, s1, s2, s3, s4, s5, s6);
    }
    uint32_t HB(uint32_t rva, uint32_t p) const { uint32_t r = H(rva, 0, p); blk(ctx, rva, p, 23); return r; }
    // the skills table row for index i, 0 when i is out of range or the row is absent (the 0xBA0 count / 0xB98 base pair, stride 0x23C)
    uint32_t SkillRow(int32_t i) const {
        if (i < 0 || i >= (int32_t)R(dt + 0xba0)) return 0;
        return (uint32_t)i * 0x23c + R(dt + 0xb98);
    }
};
}  // namespace clm
