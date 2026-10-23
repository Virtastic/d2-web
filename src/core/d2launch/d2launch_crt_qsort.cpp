// src/core/d2launch/d2launch_crt_qsort.cpp -- 1.13c D2Launch CRT leaves: 16-bit byte swap loop, shortsort, qsort.
#include <cstdint>
#include <cstddef>

typedef int (*LaunchCmp)(const void*, const void*);

// 1.13c D2Launch+0x01000 -- in-place byte swap of n 16-bit words (ESI = buf, EDI = n)
extern "C" void d2launch_swap16(uint16_t* buf, uint32_t n) {
    for (uint32_t i = 0; i < n; ++i) buf[i] = (uint16_t)((buf[i] << 8) | (buf[i] >> 8));
}

static void lc_swap(uint8_t* a, uint8_t* b, size_t w) {
    if (a == b) return;
    while (w--) { uint8_t t = *a; *a++ = *b; *b++ = t; }
}

// 1.13c D2Launch+0x010B0 -- shortsort (hi in EAX); selection sort, max to the end
extern "C" void d2launch_shortsort(uint8_t* lo, uint8_t* hi, size_t w, LaunchCmp cmp) {
    while (hi > lo) {
        uint8_t* mx = lo;
        for (uint8_t* p = lo + w; p <= hi; p += w)
            if (cmp(p, mx) > 0) mx = p;
        lc_swap(mx, hi, w);
        hi -= w;
    }
}

// 1.13c D2Launch+0x01120 -- _qsort (median of 3, explicit stack of 30)
extern "C" void d2launch_qsort(void* base, size_t num, size_t w, LaunchCmp cmp) {
    if (num < 2 || w == 0) return;
    uint8_t *lo = (uint8_t*)base, *hi = lo + (num - 1) * w;
    uint8_t *stkl[30], *stkh[30];
    int sp = 0;
    for (;;) {
        size_t n = (size_t)(hi - lo) / w + 1;
        if (n <= 8) {
            d2launch_shortsort(lo, hi, w, cmp);
        } else {
            uint8_t* mid = lo + (n >> 1) * w;
            if (cmp(lo, mid) > 0) lc_swap(lo, mid, w);
            if (cmp(lo, hi) > 0) lc_swap(lo, hi, w);
            if (cmp(mid, hi) > 0) lc_swap(mid, hi, w);
            uint8_t *l = lo, *g = hi;
            for (;;) {
                if (mid > l) {
                    do { l += w; } while (l < mid && cmp(l, mid) <= 0);
                }
                if (!(mid > l) ) {
                    do { l += w; } while (l <= hi && cmp(l, mid) <= 0);
                }
                do { g -= w; } while (g > mid && cmp(g, mid) > 0);
                if (g < l) break;
                lc_swap(l, g, w);
                if (mid == g) mid = l;
            }
            g += w;
            if (mid < g) {
                do { g -= w; } while (g > mid && cmp(g, mid) == 0);
            } else {
                do { g -= w; } while (g > lo && cmp(g, mid) == 0);
            }
            if (g - lo >= hi - l) {
                if (lo < g) { stkl[sp] = lo; stkh[sp] = g; ++sp; }
                if (l < hi) { lo = l; continue; }
            } else {
                if (l < hi) { stkl[sp] = l; stkh[sp] = hi; ++sp; }
                if (lo < g) { hi = g; continue; }
            }
        }
        if (--sp < 0) return;
        lo = stkl[sp]; hi = stkh[sp];
    }
}
