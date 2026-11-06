// src/core/d2win/d2win_sub_18e1f.cpp -- D2Win.dll CRT _cftof (MSVC 7.1): %f conversion of a double.
#include <cstdint>
#include <emscripten.h>

extern "C" int32_t* d2win_sub_194D0(double v, int32_t* out, char* buf);                          // _fltout2, +0x194D0
extern "C" void d2win_fptostr_1939f(char* dst, int32_t nd, int32_t* flt, const char* m);          // __fptostr, +0x1939F
extern "C" char* d2win_cftof2_18d83(char* buf, int32_t nd, int32_t flag, const int32_t* flt);     // __cftof2, +0x18D83

// 1.13c D2Win+0x18E1F
// _cftof(double* val, char* buf, int ndec): _fltout2, __fptostr of decpt+ndec digits past the sign slot, then
// __cftof2(buf, ndec, 0). Returns buf.
extern "C" EMSCRIPTEN_KEEPALIVE
char* d2win_crt_cftof_18e1f(const double* val, char* buf, int32_t ndec) {
    int32_t flt[4];
    char dg[24];
    d2win_sub_194D0(*val, flt, dg);
    d2win_fptostr_1939f(buf + (flt[0] == '-'), flt[1] + ndec, flt, dg);
    return d2win_cftof2_18d83(buf, ndec, 0, flt);
}
