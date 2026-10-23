// 1.13c D2sound+0x0C260 -- CRT x87 helper: ST0 = 2^ST0 (f2xm1 on the fraction after round-to-nearest, then fscale).
#include <cmath>

double D2sound_C260(double st0) {
    double n = std::nearbyint(st0);
    return std::scalbn(1.0 + std::expm1((st0 - n) * 0.6931471805599453), (int)n);
}
