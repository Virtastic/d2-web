// Prior art used: MSVC VC7.1 CRT ___mbtowc_mt (decompile cache, Fog+0x9BE1); the locale info fields +4 code page, +0x14 mbcs flag, +0x28 max char length, +0x48 ctype table.
#include <cstdint>
#include <cstring>

struct FogMbtowcHost {
    int32_t (*multiByteToWideChar)(uint32_t cp, uint32_t flags, const uint8_t* s, int32_t n, uint16_t* wc, int32_t cap);   // kernel32
    void (*setErrno)(int32_t v);                                                                                         // *FUN_56A5A
    const uint8_t* (*ctype)(const uint8_t* loc);                                                                          // the table the 32-bit word at +0x48 points to
};

// 1.13c Fog+0x09BE1: ___mbtowc_mt(loc, wc, s, n). NULL or empty input, or a NUL byte, returns 0 (wc = 0); a single-byte locale (+0x14 == 0)
// gives wc = byte and 1; else one MultiByteToWideChar of 1 byte (lead bit clear) or +0x28 bytes; failure sets errno 0x2a and returns -1.
int32_t fog_mbtowc_mt_9be1(const uint8_t* loc, uint16_t* wc, const uint8_t* s, uint32_t n, const FogMbtowcHost* h)
{
    if (!s || n == 0) return 0;
    const uint8_t b = s[0];
    if (b == 0) { if (wc) *wc = 0; return 0; }
    int32_t flag, maxlen; uint32_t cp;
    memcpy(&flag, loc + 0x14, 4); memcpy(&maxlen, loc + 0x28, 4); memcpy(&cp, loc + 4, 4);
    if (flag == 0) { if (wc) *wc = b; return 1; }
    if ((h->ctype(loc)[1 + b * 2] & 0x80) == 0) {
        if (h->multiByteToWideChar(cp, 9, s, 1, wc, wc != nullptr)) return 1;
    } else if ((maxlen > 1 && maxlen <= (int32_t)n && h->multiByteToWideChar(cp, 9, s, maxlen, wc, wc != nullptr))
               || ((uint32_t)maxlen <= n && s[1] != 0)) {
        return maxlen;
    }
    h->setErrno(0x2a);
    return -1;
}
