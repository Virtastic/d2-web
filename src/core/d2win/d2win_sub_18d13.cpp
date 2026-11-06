// src/core/d2win/d2win_sub_18d13.cpp -- D2Win.dll CRT _cftoe (MSVC 7.1): double to "d.ddde+xxx" (3-digit exponent).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" int32_t* d2win_sub_194D0(double v, int32_t* out, char* buf);   // _fltout2, 1.13c D2Win+0x194D0

extern "C" void d2win_fptostr_1939f(char* dst, int32_t nd, int32_t* flt, const char* m);   // __fptostr, 1.13c D2Win+0x1939F

// 1.13c D2Win+0x18D13
// _cftoe(double* val, char* buf, int ndec, int caps): _fltout2 to 17 digits, __fptostr into buf past the sign and
// point slots (ndec+1 digits), then __cftoe2 (D2Win+0x18C65, flag 0): sign, point after the first digit,
// "e+000" (or "E") after the ndec decimals, exponent decpt-1 added digit by digit. Returns buf.
extern "C" EMSCRIPTEN_KEEPALIVE
char* d2win_crt_cftoe_18d13(const double* val, char* buf, int32_t ndec, int32_t caps) {
    int32_t flt[4];
    char dg[24];
    d2win_sub_194D0(*val, flt, dg);
    d2win_fptostr_1939f(buf + (ndec > 0) + (flt[0] == '-'), ndec + 1, flt, dg);
    char* p = buf;
    if (flt[0] == '-') *p++ = '-';
    if (ndec > 0) { p[0] = p[1]; p[1] = '.'; p++; }
    char* q = p + ndec + 1;
    strcpy(q, "e+000");
    if (caps) q[0] = 'E';
    if (dg[0] != '0') {   // flt[3] is dg (a 32-bit slot in retail's STRFLT)
        int32_t e = flt[1] - 1;
        if (e < 0) { e = -e; q[1] = '-'; }
        if (e > 99) { q[2] = (char)(q[2] + e / 100); e %= 100; }
        if (e > 9) { q[3] = (char)(q[3] + e / 10); e %= 10; }
        q[4] = (char)(q[4] + e);
    }
    return buf;
}
