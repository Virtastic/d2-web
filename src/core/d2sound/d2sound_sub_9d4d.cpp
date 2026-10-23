// 1.13c D2sound+0x09D4D -- CRT x87 pow body (called from 0x9CF0). Disassembled (no decompile: Ghidra failed on it).
// Flow: y==0 -> 1; x==0 -> +-0 (y>0) or +-HUGE with SING err=2 (y<0); x<0 and y non-integer -> indefinite NaN, DOM err=1;
// otherwise fyl2x/f2xm1 pow with overflow/domain errors reported through the _matherr path (0xC349, err in eax).
// Host seam: the x87 evaluation itself is libm pow; error code is returned via *err (0 none).
#include <cmath>
#include <cstdint>

double D2sound_9D4D(double x, double y, int* err) {
    *err = 0;
    if (y == 0) return 1.0;
    if (x == 0) {
        bool odd = std::fabs(y) < 9007199254740992.0 && std::fmod(y, 2.0) != 0;
        bool neg = std::signbit(x) && odd;
        if (y < 0) { *err = 2; return neg ? -HUGE_VAL : HUGE_VAL; }
        return neg ? -0.0 : 0.0;
    }
    if (x < 0 && std::floor(y) != y && !std::isinf(y)) { *err = 1; return -NAN; }
    double r = std::pow(x, y);
    if (std::isinf(r) && !std::isinf(x) && !std::isinf(y)) *err = 3;
    return r;
}
