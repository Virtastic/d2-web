// Seam for the 1.13c D2Client 0x14FD0 / 0x151B0 / 0x15350 ports (register-ABI unit/index selectors). Every callee goes through h() with the
// registers the call site loads and the stack words it pushes, so the capture's hooklog is reproduced record for record.
#pragma once
#include <cstdint>
namespace clk {
struct Env {
    void* ctx;
    uint32_t (*h)(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi,
                  uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
    uint32_t (*rd)(void* ctx, uint32_t addr);   // 32-bit load
    uint32_t dt;                                // *sgptDataTables
    uint32_t R(uint32_t a) const { return rd(ctx, a); }
    uint32_t H(uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0,
               uint32_t a = 0, uint32_t c = 0, uint32_t d = 0) const { return h(ctx, rva, a, c, d, 0, 0, s0, s1, s2, s3, s4, s5, s6); }
};
// the data-table row 0x23C bytes wide at index idx, 0 when idx is outside [0, count)
inline uint32_t row(const Env& e, int32_t idx) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    return idx * 0x23c + e.R(e.dt + 0xb98);
}
// unit type 2, 4 and 5 keep their position in the static path block; others ask D2Common
inline bool statPath(uint32_t ty) { return ty == 2 || ((int32_t)ty > 3 && (int32_t)ty <= 5); }
// 0xA1860's EDI out block: six words, handed by pointer
struct Out { uint32_t w[6]; };
}  // namespace clk
