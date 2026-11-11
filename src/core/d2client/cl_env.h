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
    uint32_t gl = 0;                                     // a pinned module-global array the row reads (per-row meaning; 0 in the retail wiring until pointed at the slot)
    uint32_t gl2 = 0;                                    // a second pinned module-global block (0xA2C90: the skill-effect pair at 0x1197F8)
    void (*blk)(void* ctx, uint32_t rva, uint32_t p, uint32_t n) = nullptr;   // the capture's `C` block records: n dwords at p, after the call record
    void (*ed)(void* ctx, uint32_t edi) = nullptr;       // sets EDI for the next h() (a callee that fills an EDI out-pointer: `hookw RVA e V`)
    void Ed(uint32_t v) const { if (ed) ed(ctx, v); }
    void (*edj)(void* ctx, uint32_t rva, uint32_t edi) = nullptr;   // REGS `J`: logs EDI as an argument of the callee just called
    void Edj(uint32_t rva, uint32_t v) const { if (edj) edj(ctx, rva, v); }
    void (*xs)(void* ctx, uint32_t s5, uint32_t s6) = nullptr;      // stack words 5 and 6 of the NEXT h() (a callee with more than five stack words; the log keeps s0..s3)
    void Xs(uint32_t s5, uint32_t s6 = 0) const { if (xs) xs(ctx, s5, s6); }
    void (*xw)(void* ctx, uint32_t rva, const uint32_t* w, uint32_t n) = nullptr;   // logs n extra dwords after the call record of rva
    void Xw(uint32_t rva, const uint32_t* w, uint32_t n) const { if (xw) xw(ctx, rva, w, n); }
    void Ej(uint32_t rva, uint32_t v) const { if (edj) edj(ctx, rva, v); }
    uint32_t R(uint32_t a) const { return rd(ctx, a); }
    int32_t  R16s(uint32_t a) const { return (int16_t)(rd(ctx, a) & 0xffff); }
    uint32_t R8(uint32_t a) const { return rd(ctx, a) & 0xff; }
    void     W(uint32_t a, uint32_t v) const { wr(ctx, a, v); }
    uint32_t H(uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0,
               uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0) const {
        return h(ctx, rva, a, c, d, b, s, s0, s1, s2, s3, s4);
    }
    // one inline LCG step on the unit seed at p (lo,hi): s = lo * 0x6AC690C5 + hi; returns the new lo
    uint32_t Lcg(uint32_t p) const { uint64_t s = (uint64_t)R(p) * 0x6AC690C5u + R(p + 4); W(p, (uint32_t)s); W(p + 4, (uint32_t)(s >> 32)); return (uint32_t)s; }
    // a call whose ECX is a stack-built request block (D2Client 0xB30F0): the call record, then the block records
    // unit-path coordinate: static-path unit types read the path block, others call the D2Common getter (0 without a path)
    uint32_t PD(uint32_t u, uint32_t rva, uint32_t off) const { int32_t ty = (int32_t)R(u); uint32_t p = R(u + 0x2c); if (ty == 2 || (ty > 3 && ty <= 5)) return R(p + off); return p ? H(rva, p) : 0u; }
    uint32_t HB(uint32_t rva, uint32_t p) const { uint32_t r = h(ctx, rva, 0, p, 0, 0, 0, 0, 0, 0, 0, 0); if (blk) blk(ctx, rva, p, 23); return r; }
    // missiles/monsters table row (stride 0x1A4) by index, 0 when out of range (the 0xB64 / 0xB6C pair)
    uint32_t Row(int32_t i) const {
        if (i < 0 || (int32_t)R(dt + 0xb6c) <= i) return 0;
        return R(dt + 0xb64) + (uint32_t)i * 0x1a4;
    }
    uint32_t UnitRow(uint32_t u) const { return u ? Row((int32_t)R(u + 4)) : 0; }
};
}  // namespace cl
