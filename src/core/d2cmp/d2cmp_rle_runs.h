// Row-run helpers of the D2CMP frame encoder (1.13c D2CMP+0x113D0 and D2CMP+0x11350), shared by the
// encoders at D2CMP+0x11000 and D2CMP+0x10E30. Each helper counts a run from `p` up to `max` bytes and
// stops at `end`; the caller advances its own pointer by the returned count only.
//   run_eq (0x113D0): counts bytes equal to c.      run_ne (0x11350): counts bytes not equal to c.
// f1 = the count reached the row end (no skip). Otherwise the rest of the run is skipped (not counted)
// and f2 = the skip reached the row end. Start at `end` or past it is retail's fatal path (`_exit(-1)`).
#pragma once
#include <cstdint>
#include <cstdlib>

extern "C" __attribute__((weak)) void d2cmp_fog_fatal(uint32_t msg);

// 1.13c D2CMP+0x113D0
static inline uint32_t d2cmp_run_eq(const uint8_t* p, const uint8_t* end, uint8_t c, uint32_t max,
                                    uint32_t* f1, uint32_t* f2) {
    if (!(p < end)) { d2cmp_fog_fatal(0x45); exit(-1); }
    const uint8_t* q = p;
    uint32_t n = 0;
    for (;;) {
        if (*q != c) break;
        if ((int32_t)n >= (int32_t)max) break;   // retail compares the count with max signed
        n++; q++;
        if (q >= end) break;
    }
    if (q == end) { *f1 = 1; *f2 = 0; return n; }
    *f1 = 0;
    while (q < end && *q == c) q++;
    *f2 = (q == end);
    return n;
}

// 1.13c D2CMP+0x11350
static inline uint32_t d2cmp_run_ne(const uint8_t* p, const uint8_t* end, uint8_t c, uint32_t max,
                                    uint32_t* f1, uint32_t* f2) {
    if (!(p < end)) { d2cmp_fog_fatal(0x6F); exit(-1); }
    const uint8_t* q = p;
    uint32_t n = 0;
    for (;;) {
        if (*q == c) break;
        if (n >= max) break;
        n++; q++;
        if (q >= end) break;
    }
    if (q == end) { *f1 = 1; *f2 = 0; return n; }
    *f1 = 0;
    while (q < end && *q == c) q++;
    *f2 = (q == end);
    return n;
}
