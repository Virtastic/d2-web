// src/core/d2cmp/d2cmp_sub_fec0.cpp -- 1.13c D2CMP.dll sub_FEC0 transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2cmp/decomp/0xFEC0.c). No prior art (D2MOO D2CMP is stubs).
#include <cstdint>
#include <cstdlib>
#include <cstring>

// 1.13c D2CMP+0x0FEC0 (391 bytes): flat colour/shade fill of a run-coded frame (data at ctx+0x20).
// ctx+4 is the width limit; stride < it is fatal (retail prints string 0x74f then _exit(-1); the
// message is a seam). Skips rowSkip*2 lines, then draws bottom-up: positive byte n = n literal
// pixels (filled with `color`, or remapped through table[dst*256+color] when table != 0, clipped to
// [clipL,clipR) when either is non-zero), negative byte = skip (low 7 bits) or, when zero, end of line.
extern "C"
void d2cmp_sub_fec0(const uint8_t* ctx, uint8_t* dst, uint32_t stride, int x, int clipL, int clipR,
                    int rowSkip, int rows, const uint8_t* table, uint8_t color) {
    uint32_t lim; std::memcpy(&lim, ctx + 4, 4);
    if (stride < lim) _Exit(-1);
    const uint8_t* p = ctx + 0x20;
    for (int n = rowSkip * 2; n != 0; --n) {
        uint8_t b;
        do {
            for (;;) { b = *p++; if ((int8_t)b < 0) break; p += (int8_t)b; }
        } while ((b & 0x7f) != 0);
    }
    int lx = x;
    uint8_t* ls = dst - stride;  // previous-row start
    for (;;) {
        uint8_t b = *p++;
        if ((int8_t)b < 0) {
            b &= 0x7f;
            if (b == 0) {
                dst = ls - 1;
                x = lx - 1;
                if (--rows == 0) return;
                do {
                    b = *p++;
                    if ((int8_t)b < 1) b &= 0x7f; else p += (int8_t)b;
                } while (b != 0);
                lx = x; ls = dst - stride;
            } else { x += (int8_t)b; dst += (int8_t)b; }
            continue;
        }
        int n = b;
        uint8_t* end = dst + n;
        int cnt = n;
        uint8_t* d = dst;
        if (clipL != 0 || clipR != 0) {
            int s = clipL - x; if (s < 0) s = 0;
            int e = clipR - x; if (e < 1) e = 0;
            if (n <= e) e = n;
            cnt = e - s;
            d = dst + s;
        }
        if (cnt > 0) {
            if (table == nullptr) std::memset(d, color, cnt);
            else for (int i = 0; i < cnt; ++i) d[i] = table[d[i] * 0x100 + color];
        }
        x += n; p += n; dst = end;
    }
}
