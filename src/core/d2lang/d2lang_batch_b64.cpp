// D2Lang batch b64: Unicode:: helpers. Hidden globals/registers become explicit params.
#include <cstdint>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include <initializer_list>

// One 16-byte row of the UTF-8 table at D2Lang+0x102E8 (terminated by cmask==0).
struct Utf8Row { int16_t cmask, cval, shift, pad; int32_t lmask; int32_t lval; };

// 1.13c D2Lang+0x8bb0 (helper of utf8 width): EAX=p (hidden register) -> explicit leading param.
static int utf8SeqLen(const uint8_t* p, int n, const Utf8Row* tab)
{
    int i = 0;
    if (n < 1) return -1;
    uint8_t b = *p;
    uint32_t acc = b;
    if (tab->cmask == 0) return -1;
    while (++i, ((int)tab->cmask & b) != (int)tab->cval) {
        if (n <= i) return -1;
        uint8_t c = *++p;
        if (((c ^ 0x80) & 0xc0) != 0) return -1;
        ++tab;
        acc = acc << 6 | (c ^ 0x80);
        if (tab->cmask == 0) return -1;
    }
    return (tab->lval <= (int32_t)((uint32_t)tab->lmask & acc)) ? i : -1;
}

// 1.13c D2Lang+0x8dc0 (helper of toUtf): EAX=out (hidden register) -> explicit leading param.
static int utf8Encode(uint8_t* out, uint16_t ch, const Utf8Row* tab)
{
    int n = 0;
    uint32_t c = ch;
    for (;; ++tab) {
        if (tab->cmask == 0) return -1;
        ++n;
        if ((int)c <= tab->lmask) break;
    }
    int sh = tab->shift;
    *out = (uint8_t)((int)c >> (sh & 0x1f)) | (uint8_t)tab->cval;
    while (sh > 0) {
        sh -= 6;
        *++out = (uint8_t)(((int)c >> (sh & 0x1f)) & 0x3f) | 0x80;
    }
    return n;
}

// 1.13c D2Lang+0x8630 (Unicode::isLineBreak): lang = STRTABLE_GetLanguage() result (D2Lang+0x9290).
int D2Lang_Unicode_isLineBreak(const uint16_t* s, uint32_t idx, uint32_t lang)
{
    if (idx == 0) return 1;
    if (lang == 6 || (7 < (int)lang && (int)lang < 10)) {
        uint16_t c = s[idx];
        bool keep = c == 0x2026 || c == 0x3001 || c == 0x3002 || c == 0xff01 || c == 0xff0c ||
                    c == 0xff0e || c == 0xff1f || c == 0xff61 || c == 0xff64 ||
                    (c >= 0x61 && c <= 0x7a) || (c >= 0xff41 && c <= 0xff5a) ||
                    (c >= 0x41 && c <= 0x5a) || (c >= 0xff21 && c <= 0xff3a) ||
                    (c >= 0x30 && c <= 0x39) || (c >= 0xff10 && c <= 0xff19);
        return keep ? 0 : 1;
    }
    if (s[idx] < 0x100 && isspace(s[idx])) return 0;
    if (s[idx - 1] < 0x100 && isspace(s[idx - 1])) return 1;
    return 0;
}

// 1.13c D2Lang+0x8d70 (Unicode::unicodenwidth): count UTF-8 characters in at most n bytes.
uint32_t D2Lang_Unicode_unicodenwidth(const char* s, int n, const Utf8Row* tab)
{
    uint32_t cnt = 0;
    if (0 < n) {
        while (*s != '\0') {
            int len = utf8SeqLen((const uint8_t*)s, n, tab);
            if (len == -1) { ++s; len = -1; }
            else {
                for (int i = 0; i < len; ++i) if (*s++ == '\0') return cnt;
                ++cnt;
                len = -len;
            }
            n += len;
            if (n < 1) return cnt;
        }
    }
    return cnt;
}

// 1.13c D2Lang+0x8e80 (Unicode::toUtf)
char* D2Lang_Unicode_toUtf(char* out, const uint16_t* in, int cap, const Utf8Row* tab)
{
    int n = 0;
    while (*in != 0) {
        if (cap - 1 <= n) break;
        int k = utf8Encode((uint8_t*)out + n, *in, tab);
        if (k == -1) break;
        ++in;
        n += k;
    }
    out[n] = '\0';
    return out;
}

// 1.13c D2Lang+0xac40 (Unicode::unloadSysMap): globals 10a58 / 10a50 / 10a5c by reference.
void D2Lang_Unicode_unloadSysMap(void*& g10a58, void*& g10a50, void*& g10a5c)
{
    if (g10a58) { free(g10a58); g10a58 = nullptr; }
    if (g10a50) { free(g10a50); g10a50 = nullptr; }
    if (g10a5c) { free(g10a5c); g10a5c = nullptr; }
}

// 1.13c D2Lang+0xacf0 (Unicode::unicodeWidth): g10a5c = map ptr, g10a54 = range count, g10a58 = {lo,hi} byte pairs.
uint32_t D2Lang_Unicode_unicodeWidth(const char* s, int n, const void* g10a5c, uint16_t g10a54, const uint8_t* g10a58)
{
    if (!g10a5c) { uint32_t l = 0; while (s[l]) ++l; return l; }
    uint32_t w = 0;
    int i = 0;
    if (0 < n) {
        do {
            uint8_t b = (uint8_t)s[i];
            if (b == 0) return w;
            if (b < 0x20) ++i;
            else {
                int j = 0, single = 0;
                for (; j < (int)g10a54; ++j)
                    if (g10a58[j * 2] <= b && b <= g10a58[j * 2 + 1]) { single = 1; break; }
                i += single ? 1 : 2;
            }
            ++w;
        } while (i < n);
    }
    return w;
}

// sprintf helpers: fatal(id) stands for the Ordinal_10265/_exit(-1) path (ids 0x10d, 0x154).
static void sfatal(int) { std::_Exit(-1); }
static const char16_t* wstrstr1(const char16_t* s, char16_t c) { for (; *s; ++s) if (*s == c) return s; return nullptr; }
static void wstrncat(char16_t* d, const char16_t* s, int n) { while (*d) ++d; while (n-- > 0 && *s) *d++ = *s++; *d = 0; }

// 1.13c D2Lang+0xb250 (Unicode::sprintf): UTF-16 printf of %% %d %u %s; args are machine words.
void D2Lang_Unicode_sprintf(int cap, char16_t* out, const char16_t* fmt, ...)
{
    if (!fmt) return;
    va_list ap; va_start(ap, fmt);
    *out = 0;
    int n = 0;
    const char16_t* hit = nullptr;
    if (*fmt != 0) {
        uintptr_t v = va_arg(ap, uintptr_t);
        do {
            char16_t spec = 0;
            const char16_t* p = wstrstr1(fmt, u'%');
            if (p) { spec = p[1]; hit = p; }
            for (; fmt != hit && *fmt != 0; ++fmt) {
                if (cap <= n) { out[-1] = 0; va_end(ap); return; }
                *out++ = *fmt; ++n;
            }
            if (cap <= n) { out[-1] = 0; va_end(ap); return; }
            if (*fmt == 0) { *out = 0; va_end(ap); return; }
            if (!hit) sfatal(0x10d);
            if (spec == 0) { out[0] = u'%'; out[1] = 0; va_end(ap); return; }
            else if (spec == 0x25) {
                if (cap <= ++n) { va_end(ap); return; }
                *out++ = u'%';
            } else if (spec == 'd' || spec == 'u') {
                char tmp[16]; int len = 0;
                uint64_t u = spec == 'd' ? (uint64_t)(int64_t)(int32_t)v : (uint64_t)(uint32_t)v;
                bool neg = spec == 'd' && (int32_t)v < 0;
                if (neg) u = (uint64_t)(-(int64_t)(int32_t)v);
                do { tmp[len++] = (char)('0' + u % 10); u /= 10; } while (u);
                if (neg) tmp[len++] = '-';
                if (cap <= len + 1 + n) { va_end(ap); return; }
                for (int k = len - 1; k >= 0; --k) *out++ = (char16_t)tmp[k];
                *out = 0;
                n += len;
            } else if (spec == 's') {
                const char16_t* s = (const char16_t*)v;
                int len = 0;
                if (s) while (s[len]) ++len;
                if (!s || len == 0 || cap <= len + 1 + n) {
                    if (s) wstrncat(out, s, cap - n - 1);
                    va_end(ap); return;
                }
                for (int k = 0; k <= len; ++k) out[k] = s[k];
                out += len; n += len;
            } else sfatal(0x154);
            fmt += 2;
            v = va_arg(ap, uintptr_t);
        } while (*fmt != 0);
    }
    va_end(ap);
}
