// 1.13c D2Launch+0x93E6  sub_93E6: long cdecl(LCID). GetLocaleInfoA(lcid, 0x1004 LOCALE_IDEFAULTANSICODEPAGE, buf, 6) into a stack buffer whose
// byte 6 is cleared first; 0 from the call gives -1, otherwise the CRT atol (0x935E) of the buffer: C-locale white space (0x09-0x0D, 0x20) skipped,
// one '-' or '+', decimal digits accumulated with 32-bit wrap, negated for '-'. The import is a seam; without one there is no locale (-1).
#include <cstdint>

typedef uint32_t (*D2lGetLocaleInfoA)(uint32_t lcid, uint32_t type, char* buf, int32_t n);

static int32_t d2l_atol(const char* p) {  // 0x935E
    while (*p == ' ' || (*p >= 0x09 && *p <= 0x0D)) ++p;
    const uint8_t sign = (uint8_t)*p++;
    uint32_t c = sign;
    if (sign == '-' || sign == '+') c = (uint8_t)*p++;
    uint32_t v = 0;
    while (c >= '0' && c <= '9') { v = v * 10u + (c - '0'); c = (uint8_t)*p++; }
    return (int32_t)(sign == '-' ? 0u - v : v);
}

extern "C" int32_t d2launch_sub_93e6_with(uint32_t lcid, D2lGetLocaleInfoA gli) {
    char buf[12] = {0};  // retail: 6 bytes at [ebp-0xC], [ebp-6] = 0, then padding below the cookie
    buf[6] = 0;
    if (!gli || gli(lcid, 0x1004, buf, 6) == 0) return -1;
    return d2l_atol(buf);
}
extern "C" int32_t d2launch_sub_93e6(uint32_t lcid) { return d2launch_sub_93e6_with(lcid, nullptr); }
