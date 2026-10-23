// 1.13c Fog+0x24390: CRT x87 helper (inside pow, caller Fog+0x2290D). ST0 = x -> ST0 = 2^x:
// r = round(x); result = fscale(1 + f2xm1(x - r), r) == 2^r * 2^(x-r).
#include <cmath>

extern "C" long double fog_exp2_24390(long double x) {  // 1.13c Fog+0x24390
    return std::exp2(x);
}
