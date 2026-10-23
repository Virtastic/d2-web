// 1.13c Storm+0x0667B: MSVC CRT _strtoxl, C locale. flags: 1=unsigned 2=neg 4=overflow 8=digits.
#include <cstdint>

extern "C"
uint32_t Storm_Strtoxl667B(const uint8_t* s, const uint8_t** endp, uint32_t base, uint32_t flags, int32_t* err) {
    auto sp = [](uint8_t c) { return c == ' ' || (c >= 9 && c <= 13); };
    auto dg = [](uint8_t c) { return c >= '0' && c <= '9'; };
    auto al = [](uint8_t c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); };
    uint32_t acc = 0;
    const uint8_t* p = s;
    uint8_t c = *p++;
    while (sp(c)) c = *p++;
    if (c == '-') { flags |= 2; c = *p++; }
    else if (c == '+') c = *p++;
    if ((int32_t)base < 0 || base == 1 || (int32_t)base > 36) {
        if (endp) *endp = s;
        return 0;
    }
    if (base == 0) {
        if (c != '0') base = 10;
        else if (*p == 'x' || *p == 'X') base = 16;
        else base = 8;
    }
    if (base == 16 && c == '0' && (*p == 'x' || *p == 'X')) { c = p[1]; p += 2; }
    uint32_t maxdiv = 0xffffffffu / base;
    for (;; c = *p++) {
        uint32_t d;
        if (dg(c)) d = c - '0';
        else if (al(c)) d = (c >= 'a' ? c - 0x20 : c) - 0x37;
        else d = base;
        if (d >= base) break;
        if (acc < maxdiv || (acc == maxdiv && d <= 0xffffffffu % base)) { acc = acc * base + d; flags |= 8; }
        else flags |= 12;
    }
    p--;
    if (!(flags & 8)) {
        if (endp) p = s;
        acc = 0;
    } else if ((flags & 4) || (!(flags & 1) && (((flags & 2) && acc > 0x80000000u) || (!(flags & 2) && acc > 0x7fffffffu)))) {
        *err = 0x22;  // ERANGE
        acc = (flags & 1) ? 0xffffffffu : ((flags & 2) ? 0x80000000u : 0x7fffffffu);
    }
    if (endp) *endp = p;
    return (flags & 2) ? (uint32_t)-(int32_t)acc : acc;
}
