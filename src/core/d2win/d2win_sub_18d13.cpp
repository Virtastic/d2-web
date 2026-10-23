// src/core/d2win/d2win_sub_18d13.cpp -- D2Win.dll CRT _cftoe (MSVC 7.1): double -> "d.ddde+xxx" (3-digit exponent).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Win+0x18D13
// ponytail: digit generation via snprintf("%e") instead of _fltout2/__fptostr; exponent widened to 3 digits as _cftoe2 does.
extern "C" EMSCRIPTEN_KEEPALIVE
char* d2win_crt_cftoe_18d13(const double* val, char* buf, int32_t ndec, int32_t caps) {
    char t[400];
    snprintf(t, sizeof t, "%.*e", ndec < 0 ? 0 : ndec, *val);
    char* e = strchr(t, 'e');
    if (!e) { strcpy(buf, t); return buf; }
    int x = atoi(e + 1);
    int n = (int)(e - t);
    memcpy(buf, t, n);
    snprintf(buf + n, 8, "%c%c%03d", caps ? 'E' : 'e', x < 0 ? '-' : '+', x < 0 ? -x : x);
    return buf;
}
