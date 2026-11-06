// src/core/d2win/d2win_sub_18e86.cpp -- D2Win.dll CRT _cftog (MSVC 7.1): %g conversion of a double.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" int32_t* d2win_sub_194D0(double v, int32_t* out, char* buf);                          // _fltout2, +0x194D0
extern "C" void d2win_fptostr_1939f(char* dst, int32_t nd, int32_t* flt, const char* m);          // __fptostr, +0x1939F
extern "C" char* d2win_cftof2_18d83(char* buf, int32_t nd, int32_t flag, const int32_t* flt);     // __cftof2, +0x18D83

// __shift (D2Win+0x18C48): slides the string at p right by n bytes.
static void shift(char* p, int32_t n) { if (n != 0) memmove(p + n, p, strlen(p) + 1); }

// __cftoe2 (D2Win+0x18C65) in its g form (flag 1): the digits sit right after the sign; open a slot, move the
// first digit left, put the point after it, then "e+000"/"E+000" after the nd digits with the exponent decpt-1.
static void cftoe2_g(char* buf, int32_t nd, int32_t caps, const int32_t* flt, const char* dg) {
    shift(buf + (flt[0] == '-'), nd > 0);
    char* p = buf;
    if (flt[0] == '-') { *p = '-'; p++; }
    if (nd > 0) { p[0] = p[1]; p++; *p = '.'; }
    char* q = p + nd;
    strcpy(q, "e+000");
    if (caps) *q = 'E';
    q++;
    if (dg[0] != '0') {
        int32_t e = flt[1] - 1;
        if (e < 0) { e = -e; *q = '-'; }
        q++;
        if (e >= 100) { *q = (char)(*q + e / 100); e %= 100; }
        q++;
        if (e >= 10) { *q = (char)(*q + e / 10); e %= 10; }
        q[1] = (char)(q[1] + e);
    }
}

// 1.13c D2Win+0x18E86
// _cftog(double* val, char* buf, int nd, int caps): _fltout2, __fptostr of nd digits past the sign slot; with the
// rounded exponent m = decpt-1, m < -4 or m >= nd takes __cftoe2 (g form), else __cftof2(buf, nd, 1) after
// dropping the last digit when rounding carried into a new digit. Trailing zeros are kept (printf crops them).
// Returns buf.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2win_sub_18E86(const double* dbl, char* buf, uint32_t n, int32_t caps) {
    int32_t flt[4];
    char dg[24];
    const int32_t nd = (int32_t)n;
    d2win_sub_194D0(*dbl, flt, dg);
    const int32_t before = flt[1] - 1;
    char* p = buf + (flt[0] == '-');
    d2win_fptostr_1939f(p, nd, flt, dg);
    const int32_t m = flt[1] - 1;
    const bool grew = before < m;
    if (m >= -4 && m < nd) {
        if (grew) { p += strlen(p); p[-1] = 0; }
        d2win_cftof2_18d83(buf, nd, 1, flt);
    } else {
        cftoe2_g(buf, nd, caps, flt, dg);
    }
    return buf;
}
