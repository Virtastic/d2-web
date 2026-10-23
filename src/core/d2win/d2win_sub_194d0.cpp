// src/core/d2win/d2win_sub_194d0.cpp -- 1.13c D2Win.dll CRT _fltout2 wrapper, from the cached decompile.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Win+0x194D0
// CRT _fltout2(double, STRFLT out, char* buf): converts the double to 17 significant digits
// (___dtold + $I10_OUTPUT), out[0]=sign char ('-' or ' '), out[1]=decimal point position,
// out[2]=conversion flags, out[3]=buf (digits copied in). Returns out.
// ponytail: flags (out[2]) = 1 for finite values only; INF/NaN codes of $I10_OUTPUT not modelled.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t* d2win_sub_194D0(double v, int32_t* out, char* buf) {
    char t[40];
    snprintf(t, sizeof t, "%.16e", v < 0 ? -v : v);   // d.dddddddddddddddde+XX
    char dg[20]; int n = 0;
    for (char* p = t; *p && *p != 'e'; p++) if (*p != '.') dg[n++] = *p;
    dg[n] = 0;
    int e = atoi(strchr(t, 'e') + 1);
    out[0] = (v < 0 || (v == 0 && std::signbit(v))) ? '-' : ' ';
    out[1] = (int16_t)(v == 0 ? 0 : e + 1);
    out[2] = 1;
    strcpy(buf, dg);
    out[3] = (int32_t)(intptr_t)buf;   // 32-bit retail layout
    return out;
}
