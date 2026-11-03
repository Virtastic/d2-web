// 1.13c D2Common+0x837F0 -- sub_837F0 (sub_837F0, private spec). Raw-pointer port: edi=level block, eax=row base,
// p1=column offset, p2=grid struct (+4, +0x14, +0x24 hold the cell and row-index tables), p3=window {x,y,cols,rows}.
// Returns 1 when every window cell matches the grid, 0 on the first mismatch.
#include <cstdint>

namespace {
inline uint32_t rd(uintptr_t p) { return *reinterpret_cast<const uint32_t*>(p); }
}

extern "C" uint32_t d2c_837f0_port(uintptr_t lvl, uint32_t eax, int32_t p1, uintptr_t p2, uintptr_t p3) {
    const uintptr_t s50 = rd(lvl + 0x50);
    const uintptr_t T = rd(lvl + 0x54);
    const uintptr_t U = rd(lvl + 0x58);
    const uintptr_t F1 = rd(lvl + 0xf4);
    const uintptr_t U2 = rd(lvl + 0xf8);
    const uintptr_t F2 = rd(lvl + 0xa4);
    const uintptr_t U3 = rd(lvl + 0xa8);
    const uintptr_t Q = rd(p2 + 4), Q2 = rd(p2 + 0x24), Q3 = rd(p2 + 0x14);
    const uintptr_t R0 = rd(Q), S = rd(Q + 4);
    const uintptr_t R2 = rd(Q2), S2 = rd(Q2 + 4);
    const uintptr_t R3 = rd(Q3), S3 = rd(Q3 + 4);
    const int32_t x = (int32_t)rd(p3), y = (int32_t)rd(p3 + 4);
    const int32_t cols = (int32_t)rd(p3 + 8), rows = (int32_t)rd(p3 + 12);
    for (int32_t r = 0; r < rows; ++r) {
        const uint32_t lo = eax * 4u + 4u * (uint32_t)r;          // row table index
        const uint32_t iv5 = (uint32_t)(y + r) * 4u;              // row offset index
        for (int32_t c = 0; c < cols; ++c) {
            const int32_t idx = (int32_t)rd(U + iv5) + x + c;
            if (rd(T + 4u * (uint32_t)idx) != rd(R0 + 4u * (uint32_t)((int32_t)rd(S + lo) + c + p1))) return 0;
            uint32_t u4 = (s50 && rd(s50 + 0x18) != 0) ? rd(F1 + 4u * (uint32_t)((int32_t)rd(U2 + iv5) + x + c)) : 0;
            if (u4 & 2) {
                const uint32_t u2 = rd(R2 + 4u * (uint32_t)((int32_t)rd(S2 + lo) + c + p1));
                if ((u2 & 2) == 0) return 0;
                if (((u2 ^ u4) & 0x3f0ff00u) != 0) return 0;
            }
            u4 = (s50 && rd(s50 + 0x14) != 0) ? rd(F2 + 4u * (uint32_t)((int32_t)rd(U3 + iv5) + x + c)) : 0;
            if (u4 & 1) {
                const uint32_t u2 = rd(R3 + 4u * (uint32_t)((int32_t)rd(S3 + lo) + c + p1));
                if ((u2 & 1) == 0) return 0;
                if (((u2 ^ u4) & 0x3f0ff00u) != 0) return 0;
            }
        }
    }
    return 1;
}
