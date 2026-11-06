// src/core/d2win/d2win_1939f.cpp -- D2Win.dll CRT __fptostr (MSVC 7.1), shared by _cftoe/_cftof/_cftog.
#include <cstdint>
#include <cstring>

// 1.13c D2Win+0x1939F
// __fptostr(dst, nd, strflt): '0' then nd digits of the strflt mantissa m (zero-padded once it ends), rounded up
// on the next digit (> '4') through any run of 9s; a carry into the leading '0' bumps strflt decpt (flt[1]),
// otherwise the string slides left over the leading '0'. m is strflt's mantissa pointer (flt[3] in retail).
extern "C" void d2win_fptostr_1939f(char* dst, int32_t nd, int32_t* flt, const char* m) {
    char* p = dst + 1;
    dst[0] = '0';
    int32_t k = nd;
    if (nd > 0) {
        k = 0;
        for (int32_t i = nd; i != 0; i--) { char c = *m; if (c == 0) c = '0'; else m++; *p++ = c; }
    }
    *p = 0;
    if (k >= 0 && *m > '4') {
        while (*--p == '9') *p = '0';
        ++*p;
    }
    if (dst[0] == '1') flt[1]++;
    else memmove(dst, dst + 1, strlen(dst + 1) + 1);
}
