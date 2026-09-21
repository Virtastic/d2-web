// The retail DLLs sort with the Microsoft C runtime's qsort: a median-of-three quicksort with an
// insertion-sort cutoff of 8 whose order for equal keys is what the game's tables end up in
// (the gamble selection list is sorted by item level with many ties; kind-20 phase-2 vectors
// measured the difference against musl's sort).  This is that algorithm, element for element.
#include <cstddef>
#include <cstring>

namespace {
constexpr size_t kCutoff = 8;
constexpr int kStackSize = 30;
using Compare = int (*)(const void*, const void*);

void swapElements(char* a, char* b, size_t width) {
    if (a == b) return;
    while (width--) { const char t = *a; *a++ = *b; *b++ = t; }
}
void shortSort(char* lo, char* hi, size_t width, Compare cmp) {
    while (hi > lo) {
        char* max = lo;
        for (char* p = lo + width; p <= hi; p += width) if (cmp(p, max) > 0) max = p;
        swapElements(max, hi, width);
        hi -= width;
    }
}
}  // namespace

extern "C" void d2web_msvc_qsort(void* base, size_t num, size_t width, Compare cmp) {
    if (num < 2 || width == 0) return;
    char* lostk[kStackSize]; char* histk[kStackSize]; int stkptr = 0;
    char* lo = static_cast<char*>(base);
    char* hi = lo + width * (num - 1);
recurse:
    const size_t size = (hi - lo) / width + 1;
    if (size <= kCutoff) {
        shortSort(lo, hi, width, cmp);
    } else {
        char* mid = lo + (size / 2) * width;
        if (cmp(lo, mid) > 0) swapElements(lo, mid, width);
        if (cmp(lo, hi) > 0) swapElements(lo, hi, width);
        if (cmp(mid, hi) > 0) swapElements(mid, hi, width);
        char* loguy = lo; char* higuy = hi;
        for (;;) {
            if (mid > loguy) { do { loguy += width; } while (loguy < mid && cmp(loguy, mid) <= 0); }
            if (mid <= loguy) { do { loguy += width; } while (loguy <= hi && cmp(loguy, mid) <= 0); }
            do { higuy -= width; } while (higuy > mid && cmp(higuy, mid) > 0);
            if (higuy < loguy) break;
            swapElements(loguy, higuy, width);
            if (mid == higuy) mid = loguy;
        }
        higuy += width;
        if (mid < higuy) { do { higuy -= width; } while (higuy > mid && cmp(higuy, mid) == 0); }
        if (mid >= higuy) { do { higuy -= width; } while (higuy > lo && cmp(higuy, mid) == 0); }
        if (higuy - lo >= hi - loguy) {
            if (lo < higuy) { lostk[stkptr] = lo; histk[stkptr] = higuy; ++stkptr; }
            if (loguy < hi) { lo = loguy; goto recurse; }
        } else {
            if (loguy < hi) { lostk[stkptr] = loguy; histk[stkptr] = hi; ++stkptr; }
            if (lo < higuy) { hi = higuy; goto recurse; }
        }
    }
    --stkptr;
    if (stkptr >= 0) { lo = lostk[stkptr]; hi = histk[stkptr]; goto recurse; }
}
