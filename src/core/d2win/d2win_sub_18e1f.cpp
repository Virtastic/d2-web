// src/core/d2win/d2win_sub_18e1f.cpp -- D2Win.dll CRT _cftof (MSVC 7.1): %f conversion of a double. Callees are seams.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2Win18e1fEnv {
    // 0x194D0 _fltout2: (double lo, hi) -> fills strflt {sign char at +0 via .sign, decpt at +8}, digit buffer
    void (*fltout2)(uint32_t lo, uint32_t hi, int32_t* strflt, uint32_t* buf);
    // 0x1939F __fptostr(buf, ndigits, strflt, ...)
    void (*fptostr)(char* buf, int32_t ndigits, int32_t* strflt, uint32_t lo, uint32_t hi);
    // 0x18D83 __cftof2(buf, ndigits, flag)
    void (*cftof2)(char* buf, size_t ndigits, int32_t flag);
};

// 1.13c D2Win+0x18E1F
extern "C" EMSCRIPTEN_KEEPALIVE
char* d2win_crt_cftof_18e1f(const D2Win18e1fEnv* e, const uint32_t* dbl, char* buf, size_t ndigits) {
    int32_t strflt[2];          // local_30 = sign ('-' = 0x2d), local_2c = decimal point position
    uint32_t tmp[6];
    e->fltout2(dbl[0], dbl[1], strflt, tmp);
    e->fptostr(buf + (strflt[0] == 0x2d), (int32_t)(strflt[1] + ndigits), strflt, dbl[0], dbl[1]);
    e->cftof2(buf, ndigits, 0);
    return buf;
}
