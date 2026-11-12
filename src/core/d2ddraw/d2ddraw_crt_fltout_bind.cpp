// src/core/d2ddraw/d2ddraw_crt_fltout_bind.cpp -- the two CRT callees of D2DDraw's _cftoe / _cftof / _cftog
// (src/core/d2ddraw/d2ddraw_batch_b144.cpp), bound so those three reach a real implementation.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct d2ddraw_crt_fp_STRFLT { int32_t sign, decpt, flag; char* man; };
char* d2ddraw_crt_fp___fptostr(char* buf, int count, d2ddraw_crt_fp_STRFLT* pt);   // d2ddraw_crt_fp_9ed0.cpp

extern "C" {

// 1.13c D2DDraw+0x0a001 _fltout2: the sign, decimal point and up to 17 significant digits of the double (lo, hi) into buf.
// Retail converts through ___dtold and $I10_OUTPUT (17 digits, trailing zeros dropped); as for the same CRT function in
// D2CMP (src/core/d2cmp/d2cmp_batch_d250.cpp) the host's exact 17-digit conversion supplies the digits. Zero is "0" with
// decpt 0; infinities and NaNs are $I10_OUTPUT's "1#INF" / "1#IND" / "1#QNAN" / "1#SNAN" with decpt 1.
void FUN_6f8ca001(uint32_t lo, uint32_t hi, d2ddraw_crt_fp_STRFLT* out, char* buf) {
    const uint64_t b = ((uint64_t)hi << 32) | lo;
    const uint64_t e = (b >> 52) & 0x7ff, m = b & 0xfffffffffffffull;
    out->sign = (b >> 63) ? '-' : ' ';
    out->flag = 1;
    out->man = buf;
    if (e == 0x7ff) {
        out->decpt = 1;
        strcpy(buf, !m ? "1#INF" : ((b >> 63) && m == 0x8000000000000ull) ? "1#IND" : (m >> 51) ? "1#QNAN" : "1#SNAN");
        return;
    }
    if (!e && !m) { out->decpt = 0; strcpy(buf, "0"); return; }
    const uint64_t mag = b & 0x7fffffffffffffffull;
    double x; memcpy(&x, &mag, 8);
    char t[40]; snprintf(t, sizeof t, "%.16e", x);   // d.dddddddddddddddde[+-]xx
    int32_t n = 0;
    for (const char* q = t; *q && *q != 'e'; ++q) if (*q != '.') buf[n++] = *q;
    while (n > 1 && buf[n - 1] == '0') --n;
    buf[n] = 0;
    out->decpt = (int32_t)strtol(strchr(t, 'e') + 1, nullptr, 10) + 1;
}

// 1.13c D2DDraw+0x09ed0 __fptostr, under the name the three callers use.
void __fptostr(char* buf, int digits, d2ddraw_crt_fp_STRFLT* f) { (void)d2ddraw_crt_fp___fptostr(buf, digits, f); }

}  // extern "C"
