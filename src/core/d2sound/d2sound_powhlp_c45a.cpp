// 1.13c D2sound+0x0C45A -- MSVC CRT __powhlp: pow() special cases for infinite operands.
// Doubles carried as IEEE bit patterns (no float in our code). `inttype` is the result of the
// callee 0xC3EC (__d_inttype(y)): 1 = odd integer. Returns the "handled" flag; *out gets the bits.
#include <cstdint>
static const uint64_t kInf = 0x7FF0000000000000ull, kNegZero = 0x8000000000000000ull,
                      kNegNaN = 0xFFF8000000000000ull, kOne = 0x3FF0000000000000ull;
static bool isnan_(uint64_t b) { return (b & ~kNegZero) > kInf; }
int d2s_powhlp(uint64_t x, uint64_t y, int inttype, uint64_t* out) {
    uint64_t ax = x & ~kNegZero;  // x<0 ? -x : x (NaN stays NaN)
    bool unord = isnan_(ax);
    bool gt1 = !unord && ax > kOne, lt1 = !unord && ax < kOne;
    if (y == kInf || y == (kInf | kNegZero)) {
        bool pos = (y == kInf);
        if (pos) {
            if (gt1) { *out = kInf; return 0; }
            if (!lt1 && !unord) { *out = kNegNaN; return 1; }
            *out = 0; return 0;
        }
        if (gt1) { *out = 0; return 0; }
        if (lt1) { *out = kInf; return 0; }
        *out = kNegNaN; return 1;
    }
    if (x == kInf) {
        // +inf ** y
    } else if (x == (kInf | kNegZero)) {
        bool yle0 = !isnan_(y) && ((y & ~kNegZero) == 0 || (y & kNegZero));
        if (!yle0) { *out = (inttype == 1) ? (kInf | kNegZero) : kInf; return 0; }
        if ((y & ~kNegZero) == 0) { *out = kOne; return 0; }
        *out = (inttype == 1) ? kNegZero : 0; return 0;
    } else return 0;
    if (isnan_(y) || (!(y & kNegZero) && (y & ~kNegZero))) *out = kInf;  // y > 0 (or NaN)
    else if ((y & ~kNegZero) == 0) *out = kOne;
    else *out = 0;
    return 0;
}
