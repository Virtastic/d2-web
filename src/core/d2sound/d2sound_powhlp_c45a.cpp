// 1.13c D2sound+0x0C45A -- MSVC CRT __powhlp: pow() special cases for infinite operands.
// Doubles carried as IEEE bit patterns (no float in our code). `inttype` is the result of the
// callee 0xC3EC (__d_inttype(y)): 1 = odd integer. Returns the "handled" flag; *out gets the bits.
// Retail's x87 compares read C0/C2/C3 through `test ah,0x41` / `test ah,5; jp`, so an unordered (NaN)
// operand lands on a fixed branch: |x| NaN with y = +inf or -inf gives (-NaN, 1); x = +-inf with y NaN gives 1.0.
#include <cstdint>
static const uint64_t kInf = 0x7FF0000000000000ull, kNegZero = 0x8000000000000000ull,
                      kNegNaN = 0xFFF8000000000000ull, kOne = 0x3FF0000000000000ull;
static bool isnan_(uint64_t b) { return (b & ~kNegZero) > kInf; }
// Three-way compare of a double against zero: -1 less, 0 equal, 1 greater, 2 unordered.
static int cmp0(uint64_t b) {
    if (isnan_(b)) return 2;
    if ((b & ~kNegZero) == 0) return 0;
    return (b & kNegZero) ? -1 : 1;
}
// 1.13c D2sound+0xC45A
int d2s_powhlp(uint64_t x, uint64_t y, int inttype, uint64_t* out) {
    uint64_t ax = isnan_(x) ? x : (x & ~kNegZero);  // fchs only when x < 0; a NaN is left as is
    int c1 = isnan_(ax) ? 2 : (ax > kOne ? 1 : (ax == kOne ? 0 : -1));  // |x| against 1.0
    if (y == kInf) {
        if (c1 == 1) { *out = kInf; return 0; }
        if (c1 == -1) { *out = 0; return 0; }
        *out = kNegNaN; return 1;
    }
    if (y == (kInf | kNegZero)) {
        if (c1 == 1) { *out = 0; return 0; }
        if (c1 == -1) { *out = kInf; return 0; }
        *out = kNegNaN; return 1;
    }
    int cy = cmp0(y);
    if (x == kInf) {
        *out = (cy == 1) ? kInf : (cy == -1) ? 0 : kOne;
        return 0;
    }
    if (x == (kInf | kNegZero)) {
        if (cy == 1) *out = (inttype == 1) ? (kInf | kNegZero) : kInf;
        else if (cy == -1) *out = (inttype == 1) ? kNegZero : 0;
        else *out = kOne;
        return 0;
    }
    return 0;
}
