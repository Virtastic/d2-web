#include <cstdint>

// 1.13c Fog+0x2159C: CRT _ftol. ST0 -> int64, truncated toward zero
// (round-to-nearest, then corrected by the sign of the remainder).
// Fixed-point input: value is passed as an integer part and a 32.32 fraction
// would add float; instead take the exact x87 value as a double (|x| < 2^53).
// 1.13c Fog+0x2159C
int64_t fog_2159c_ftol(double x) {
    int64_t r = (int64_t)x; // C cast truncates toward zero
    return r;
}
