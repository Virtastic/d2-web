// src/core/d2multi/d2multi_crt_1c34.cpp -- D2Multi.dll CRT leaves (toupper, strtoxl, aulldvrm). C locale only, as in
// d2multi_crt_1500. Not ported: __CRT_INIT, entry, __amsg_exit, __flsbuf, write_*, sub_2334 (stdio/CRT-state/OS
// bound), ___isctype_mt (takes a locinfo struct + GetStringTypeA).
#include <cstdint>

// 1.13c D2Multi+0x1C34  ___toupper_mt (C locale branch: 'a'..'z' -> minus 0x20)
uint32_t d2multi_toupper(uint32_t c) { return (c > 0x60 && c < 0x7b) ? c - 0x20 : c; }

// 1.13c D2Multi+0x1D1F  _strtoxl: flags bit0 unsigned, bit1 neg(set by sign), 0x4/0x8 internal; errno=34 on overflow
uint32_t d2multi_strtoxl(const uint8_t* s, const uint8_t** end, uint32_t base, uint32_t flags, int* err) {
    auto isspace_ = [](uint8_t c) { return c == ' ' || (c >= 9 && c <= 13); };
    uint32_t val = 0;
    const uint8_t* p = s;
    uint8_t c = *p;
    while (isspace_(c)) c = *++p;
    ++p;
    if (c == '-') { flags |= 2; c = *p++; }
    else if (c == '+') c = *p++;
    if ((int32_t)base < 0 || base == 1 || (int32_t)base > 36) {
        if (end) *end = s;
        return 0;
    }
    if (base == 0) {
        if (c != '0') base = 10;
        else if (*p != 'x' && *p != 'X') base = 8;
        else base = 16;
    }
    if (base == 16 && c == '0' && (*p == 'x' || *p == 'X')) { c = p[1]; p += 2; }
    uint32_t maxq = 0xffffffffu / base, maxr = 0xffffffffu % base;
    for (;; c = *p++) {
        uint32_t d;
        if (c >= '0' && c <= '9') d = c - '0';
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) d = (c >= 'a' ? c - 0x20 : c) - 0x37;
        else break;
        if (d >= base) break;
        if (val < maxq || (val == maxq && d <= maxr)) { val = val * base + d; flags |= 8; }
        else flags |= 0xc;
    }
    --p;
    if (!(flags & 8)) {
        if (end) p = s;
        val = 0;
    } else if ((flags & 4) || (!(flags & 1) && (((flags & 2) && val > 0x80000000u) || (!(flags & 2) && val > 0x7fffffffu)))) {
        if (err) *err = 34;
        val = (flags & 1) ? 0xffffffffu : ((flags & 2) != 0) + 0x7fffffffu;
    }
    if (end) *end = p;
    return (flags & 2) ? (uint32_t)-(int32_t)val : val;
}

// 1.13c D2Multi+0x2AF0  __aulldvrm: unsigned 64-bit divide; retail returns quotient in edx:eax, remainder in ebx:ecx
struct D2MultiDivRem { uint64_t quot, rem; };
D2MultiDivRem d2multi_aulldvrm(uint64_t n, uint64_t d) { return { n / d, n % d }; }
