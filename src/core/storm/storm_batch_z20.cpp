// Storm CRT pow() chain (MSVC _CIpow). x87/SSE2 bodies are expressed over doubles via <cmath>;
// FPU/MXCSR state and the retail constant tables are seams. Style of storm_batch_s21.cpp.
#include <cstdint>
#include <cmath>
#include <cstring>

extern uint32_t g_z20_sse2Flag;                         // Storm DAT_6fc468fc
bool z20_fpuMasked();                                   // (MXCSR&0x1f80)==0x1f80 && (FPUCW&0x7f)==0x7f
double z20_pow_x87(double x, double y);                 // Storm+0x3CA1D (no decompile; x87 fallback)
void z20_matherr(int type, uint32_t* a, uint16_t* cw);  // Storm+0x4059B (FUN_6fc3059b, __cdecl(type, &rec, &cw))

// 1.13c Storm+0x3C908: double -> int64 truncation (_ftol); result in edx:eax.
int64_t z20_ftol(double v) { return (int64_t)v; }

// 1.13c Storm+0x3DB49: SSE2 pow(x, y). GUESSED: retail's table-driven body (~2.8KB) replaced by std::pow.
double z20_pow_sse2_body(double x, double y) { return std::pow(x, y); }

// 1.13c Storm+0x3DB30: thunk, forwards ST1/ST0 to Storm+0x3DB49.
double z20_pow_sse2(double x, double y, double (*body)(double, double)) { return body(x, y); }
double z20_pow_sse2(double x, double y) { return z20_pow_sse2(x, y, z20_pow_sse2_body); }

// 1.13c Storm+0x3C9C0: pow dispatcher; SSE2 path iff flag set and FP exceptions all masked.
double z20_pow(double x, double y, double (*sse2Body)(double, double)) {
    if (g_z20_sse2Flag != 0 && z20_fpuMasked()) return z20_pow_sse2(x, y, sse2Body);
    return z20_pow_x87(x, y);
}
double z20_pow(double x, double y) { return z20_pow(x, y, z20_pow_sse2_body); }

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

// 1.13c Storm+0x3EA60: __fastcall(ecx=ecx, edx=type, stack cw, p4..p8). Record is
// the 8-dword _exception record at ebp-0x20: {eax (type slot), ecx (name), p5, p6 (arg1), p7, p8 (arg2),
// ST0 (retval, fstp at ebp-8)} (p4 unused); calls matherr(type, &rec, &cw), then returns the record's retval
// (fld [ebp-8]: the handler may rewrite it). The trailing fldcw restores host FPU state only.
double z20_3EA60(uint32_t ecx, int type, uint16_t cw, uint32_t p4, uint32_t p5, uint32_t p6,
                 uint32_t p7, uint32_t p8, double st0) {
    uint32_t rec[8] = {0, ecx, p5, p6, p7, p8, 0, 0};
    std::memcpy(rec + 6, &st0, 8);
    (void)p4;
    z20_matherr(type, rec, &cw);
    double ret; std::memcpy(&ret, rec + 6, 8);
    return ret;
}

// 1.13c Storm+0x3EA77 (__startOneArgErrorHandling): same, one-arg record.
double z20_startOneArgErrorHandling(int type, uint16_t cw, double st0, uint32_t a, uint32_t b, uint32_t c) {
    uint32_t rec[5] = {0, a, b, c, 0};
    z20_matherr(type, rec, &cw);
    return st0;
}

// 1.13c Storm+0x3EBA9: x87 result check; zero/denormal or Inf operand forces the error path.
// GUESSED: fscale operands are discarded and FPU status words are passed in.
void z20_3EBA9(int type, uint16_t cw, uint16_t fpuStatus, double st0, uint32_t a, uint32_t b, uint32_t c) {
    uint64_t bits; std::memcpy(&bits, &st0, 8);
    bool special = (bits & 0x7ff0000000000000ull) == 0 || ((bits >> 32) & 0x7ff00000) == 0x7ff00000;
    if (!special && (cw == 0x27f || (cw & 0x20) != 0 || (fpuStatus & 0x20) == 0)) return;
    if (type == 0x1d) z20_3EA60(a, 0x1d, cw, 0, b, c, 0, 0, st0);  // GUESSED arg mapping (3EBA9 not in this batch)
    else z20_startOneArgErrorHandling(type, cw, st0, a, b, c);
}
// 1.13c Storm+0x3CA1D: x87 pow fallback; no decompile (Ghidra fails), modelled by seam z20_pow_x87 (GUESSED).
