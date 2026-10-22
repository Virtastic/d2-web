// src/core/d2multi/d2multi_crt_1000.cpp -- D2Multi.dll's statically linked MSVC 2003 CRT leaf routines, from
// the decompiles. Locks are no-ops (single wasm thread); the per-thread _token/locale are one global/the C
// locale. Not ported: _sprintf (needs the 0x2334 formatter + __flsbuf) and __cinit (walks linker tables).
#include <cstdint>
#include <cstddef>
#include <unistd.h>

typedef void (*d2m_fn)(void);

static char* g_strtokToken;  // _ptiddata->_token

// 1.13c D2Multi+0x1058  _xtoa: digits of val in radix (lowercase), '-' first when neg (val negated)
void d2multi_xtoa(uint32_t val, char* buf, uint32_t radix, int neg) {
    char* p = buf;
    if (neg) { *p++ = '-'; buf = p; val = (uint32_t)-(int32_t)val; }
    char* last;
    do {
        uint32_t d = val % radix;
        val /= radix;
        *p = (char)(d < 10 ? d + '0' : d + 'W');
        last = p++;
    } while (val != 0);
    *p = 0;
    while (buf < last) { char t = *last; *last-- = *buf; *buf++ = t; }
}

// 1.13c D2Multi+0x1096  __itoa
char* d2multi_itoa(int value, char* dest, int radix) {
    d2multi_xtoa((uint32_t)value, dest, (uint32_t)radix, radix == 10 && value < 0);
    return dest;
}

// 1.13c D2Multi+0x10C0  _atol (C locale: _SPACE is 9..13 and 32)
long d2multi_atol(const char* s) {
    auto sp = [](uint8_t c) { return c == 32 || (c >= 9 && c <= 13); };
    while (sp((uint8_t)*s)) ++s;
    uint32_t sign = (uint8_t)*s;
    const uint8_t* p = (const uint8_t*)s + 1;
    uint32_t c = sign;
    if (sign == '-' || sign == '+') c = *p++;
    int32_t n = 0;
    while (c >= '0' && c <= '9') { n = (int32_t)((uint32_t)n * 10u + (c - '0')); c = *p++; }
    return sign == '-' ? -n : n;
}

// 1.13c D2Multi+0x1150  _strncpy: copy up to count bytes, zero-pad after the terminator
char* d2multi_strncpy(char* dest, const char* src, size_t count) {
    size_t i = 0;
    for (; i < count && src[i]; ++i) dest[i] = src[i];
    for (; i < count; ++i) dest[i] = 0;
    return dest;
}

// 1.13c D2Multi+0x1274  strtok: delimiter bitmap, token continues from the saved pointer
uint8_t* d2multi_strtok(uint8_t* str, const uint8_t* delim) {
    uint8_t map[32] = {};
    uint8_t b;
    do { b = *delim++; map[b >> 3] |= (uint8_t)(1 << (b & 7)); } while (b);
    if (!str) str = (uint8_t*)g_strtokToken;
    while ((map[*str >> 3] & (1 << (*str & 7))) && *str) ++str;
    uint8_t* p = str;
    for (;; ++p) {
        if (!*p) break;
        if (map[*p >> 3] & (1 << (*p & 7))) { *p++ = 0; break; }
    }
    g_strtokToken = (char*)p;
    return str != p ? str : nullptr;
}

// 1.13c D2Multi+0x1333  ___crtExitProcess: CorExitProcess (when mscoree is loaded) then ExitProcess
void d2multi_crtExitProcess(int code) { _exit(code); }

// 1.13c D2Multi+0x1363  takes CRT lock 8 via __lock (no-op, one thread)
void d2multi_crt_lock8(void) {}

// 1.13c D2Multi+0x136C  releases CRT lock 8 via 0x31F1 (no-op, one thread)
void d2multi_crt_unlock8(void) {}

// 1.13c D2Multi+0x1375  __initterm: call every non-null entry in [first, last)
void d2multi_initterm(d2m_fn* first, d2m_fn* last) {
    for (; first < last; ++first)
        if (*first) (*first)();
}

// 1.13c D2Multi+0x14A6  doexit's unlock tail: releases lock 8 unless the caller kept it (param_3)
void d2multi_doexit_unlock(int retainLock) { if (!retainLock) d2multi_crt_unlock8(); }
