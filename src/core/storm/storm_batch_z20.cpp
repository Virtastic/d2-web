// Storm CRT pow() chain (MSVC _CIpow). x87/SSE2 bodies are expressed over doubles via <cmath>;
// FPU/MXCSR state and the retail constant tables are seams. Style of storm_batch_s21.cpp.
#include <cstdint>
#include <cmath>
#include <cstring>

extern uint32_t g_z20_sse2Flag;                         // Storm DAT_6fc468fc
bool z20_fpuMasked();                                   // (MXCSR&0x1f80)==0x1f80 && (FPUCW&0x7f)==0x7f
double z20_pow_x87(double x, double y);                 // Storm+0x3CA1D (no decompile; x87 fallback)
void z20_matherr(int type, uint32_t* a);                // Storm+0x4059B (FUN_6fc3059b)

// 1.13c Storm+0x3C908: double -> int64 truncation (_ftol); result in edx:eax.
int64_t z20_ftol(double v) { return (int64_t)v; }

// 1.13c Storm+0x3DB49: SSE2 pow(x, y). GUESSED: retail's table-driven body (~2.8KB) replaced by std::pow.
double z20_pow_sse2_body(double x, double y) { return std::pow(x, y); }

// 1.13c Storm+0x3DB30: thunk, forwards ST1/ST0 to Storm+0x3DB49.
double z20_pow_sse2(double x, double y) { return z20_pow_sse2_body(x, y); }

// 1.13c Storm+0x3C9C0: pow dispatcher; SSE2 path iff flag set and FP exceptions all masked.
double z20_pow(double x, double y) {
    if (g_z20_sse2Flag != 0 && z20_fpuMasked()) return z20_pow_sse2(x, y);
    return z20_pow_x87(x, y);
}

// 1.13c Storm+0x3EAC0: 2^x via f2xm1/fscale.
double z20_exp2(double x) { return std::exp2(x); }

// 1.13c Storm+0x3EAD5: empty body.
void z20_3EAD5() {}
// 1.13c Storm+0x3EB5E: empty body.
void z20_3EB5E() {}

// 1.13c Storm+0x3EB05 (__fload_withFB): exponent field of the high dword; whole dword if Inf/NaN.
uint32_t z20_fload_withFB(const uint32_t* p) {
    uint32_t e = p[1] & 0x7ff00000;
    return e != 0x7ff00000 ? e : p[1];
}

// 1.13c Storm+0x3EA60: build error record, call matherr (type in ecx-arg), return ST0.
double z20_3EA60(int type, uint16_t cw, double st0, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    uint32_t rec[6] = {0, a, b, c, d, 0};
    (void)cw;
    z20_matherr(type, rec);
    return st0;
}

// 1.13c Storm+0x3EA77 (__startOneArgErrorHandling): same, one-arg record.
double z20_startOneArgErrorHandling(int type, uint16_t cw, double st0, uint32_t a, uint32_t b, uint32_t c) {
    uint32_t rec[5] = {0, a, b, c, 0};
    (void)cw;
    z20_matherr(type, rec);
    return st0;
}

// 1.13c Storm+0x3EBA9: x87 result check; zero/denormal or Inf operand forces the error path.
// GUESSED: fscale operands are discarded and FPU status words are passed in.
void z20_3EBA9(int type, uint16_t cw, uint16_t fpuStatus, double st0, uint32_t a, uint32_t b, uint32_t c) {
    uint64_t bits; std::memcpy(&bits, &st0, 8);
    bool special = (bits & 0x7ff0000000000000ull) == 0 || ((bits >> 32) & 0x7ff00000) == 0x7ff00000;
    if (!special && (cw == 0x27f || (cw & 0x20) != 0 || (fpuStatus & 0x20) == 0)) return;
    if (type == 0x1d) z20_3EA60(0x1d, cw, st0, a, b, c, 0);
    else z20_startOneArgErrorHandling(type, cw, st0, a, b, c);
}
// 1.13c Storm+0x3CA1D: x87 pow fallback; no decompile (Ghidra fails), modelled by seam z20_pow_x87 (GUESSED).
