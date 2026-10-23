// 1.13c Storm+0x08644 -- MSVC CRT __crtLCMapStringA: LCMapString for ANSI strings (wide path or code-page path).
#include <cstdint>
#include <cstdlib>
#include <cstring>
// seams: Win32 NLS + Storm+0xD703 (locale -> ANSI code page, -1 on error) + Storm+0xD74A (code-page convert; returns buffer or 0)
struct S8644Env {
    int (*LCMapStringW)(uint32_t, uint32_t, const uint16_t*, int, uint16_t*, int);
    int (*LCMapStringA)(uint32_t, uint32_t, const char*, int, char*, int);
    int (*MultiByteToWideChar)(uint32_t, uint32_t, const char*, int, uint16_t*, int);
    int (*WideCharToMultiByte)(uint32_t, uint32_t, const uint16_t*, int, char*, int);
    uint32_t (*GetLastError)();
    int32_t (*LocaleCodePage)(uint32_t);
    char* (*Convert)(uint32_t from, uint32_t to, const char* src, int* len, char* dst, int dstLen);
    int mode;  // DAT 42494: 0 unknown, 1 wide supported, 2 not
    uint32_t defLocale, defCodePage;  // DAT 42778 / 42788
};

int Storm_8644(S8644Env& e, uint32_t locale, uint32_t flags, const char* src, int len, char* dst, int dstLen,
               uint32_t cp, int errCheck) {
    if (e.mode == 0) {
        static const uint16_t empty[1] = {0};
        if (e.LCMapStringW(0, 0x100, empty, 1, nullptr, 0) == 0) { if (e.GetLastError() == 0x78) e.mode = 2; }
        else e.mode = 1;
    }
    if (len > 0) { int i = 0; while (i < len && src[i]) i++; len = i; }
    if (e.mode == 1) {
        if (!cp) cp = e.defCodePage;
        int wn = e.MultiByteToWideChar(cp, errCheck ? 9 : 1, src, len, nullptr, 0);
        if (!wn) return 0;
        uint16_t* w = (uint16_t*)malloc(wn * 2);
        if (!w) return 0;
        int r = 0;
        if (e.MultiByteToWideChar(cp, 1, src, len, w, wn)) {
            r = e.LCMapStringW(locale, flags, w, wn, nullptr, 0);
            if (r) {
                if (flags & 0x400) {
                    if (dstLen && r <= dstLen) e.LCMapStringW(locale, flags, w, wn, (uint16_t*)dst, dstLen);
                } else {
                    uint16_t* m = (uint16_t*)malloc(r * 2);
                    if (!m) { free(w); return r; }
                    if (e.LCMapStringW(locale, flags, w, wn, m, r))
                        r = e.WideCharToMultiByte(cp, 0, m, r, dst, dstLen);
                    free(m);
                }
            }
        }
        free(w);
        return r;
    }
    if (e.mode != 0) {
        if (!locale) locale = e.defLocale;
        if (!cp) cp = e.defCodePage;
        int32_t lcp = e.LocaleCodePage(locale);
        if (lcp == -1) return 0;
        if ((uint32_t)lcp == cp) return e.LCMapStringA(locale, flags, src, len, dst, dstLen);
        char* s = e.Convert(cp, lcp, src, &len, nullptr, 0);
        if (!s) return 0;
        uint32_t r = 0;
        int n = e.LCMapStringA(locale, flags, s, len, nullptr, 0);
        if (n) {
            char* m = (char*)calloc(n, 1);
            if (m) {
                n = e.LCMapStringA(locale, flags, s, len, m, n);
                if (n) {
                    char* o = e.Convert(lcp, cp, m, &n, dst, dstLen);
                    r = o ? n : 0;
                    if (o && o != dst) free(o);
                }
                free(m);
            }
        }
        free(s);
        return r;
    }
    return 0;
}
