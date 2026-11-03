// 1.13c Fog+0x86E0: long __cdecl(LCID lcid) -- GetLocaleInfoA(lcid, 0x1004 (LOCALE_IDEFAULTANSICODEPAGE), buf, 6); 0 returns -1, else _atol(buf) (CRT 0xAFAD:
// skip C-locale white space, one optional sign, decimal digits accumulated into a wrapping 32-bit value, negated for '-').
// The import is a seam; buf is the 6-byte local followed by the zeroed byte after it (the import may store a whole dword pair).
#include <cstdint>
typedef int (*FogGetLocaleInfoFn)(void* ctx, uint32_t lcid, uint32_t lctype, char* buf, int cch);
extern "C" int32_t fog_86e0(FogGetLocaleInfoFn gli, void* ctx, uint32_t lcid) {
    char buf[8] = {0};
    if (gli(ctx, lcid, 0x1004, buf, 6) == 0) return -1;
    const unsigned char* p = (const unsigned char*)buf;
    while (*p == ' ' || (*p >= 9 && *p <= 13)) p++;
    unsigned c = *p++, sign = c;
    if (c == '-' || c == '+') c = *p++;
    uint32_t v = 0;
    while (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); c = *p++; }
    return (int32_t)(sign == '-' ? 0u - v : v);
}
