// 1.13c Storm+0x8644 (__crtLCMapStringA, cdecl 8 args + state blob g): the port with the Win32 NLS seams replaced by a fixed
// ASCII / code page 1252 / locale 0x409 model (the capture feeds ASCII only, so retail's NLS answers are the same).
// g: +0 mode (DAT 42494), +4 default locale (42778), +8 default code page (42788), +0xC mode after the call.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct S8644Env {
    int (*LCMapStringW)(uint32_t, uint32_t, const uint16_t*, int, uint16_t*, int);
    int (*LCMapStringA)(uint32_t, uint32_t, const char*, int, char*, int);
    int (*MultiByteToWideChar)(uint32_t, uint32_t, const char*, int, uint16_t*, int);
    int (*WideCharToMultiByte)(uint32_t, uint32_t, const uint16_t*, int, char*, int);
    uint32_t (*GetLastError)();
    int32_t (*LocaleCodePage)(uint32_t);
    char* (*Convert)(uint32_t from, uint32_t to, const char* src, int* len, char* dst, int dstLen);
    int mode;
    uint32_t defLocale, defCodePage;
};
int Storm_8644(S8644Env& e, uint32_t locale, uint32_t flags, const char* src, int len, char* dst, int dstLen,
               uint32_t cp, int errCheck);
namespace {
uint32_t mapc(uint32_t f, uint32_t c) {
    if ((f & 0x300) == 0x200 && c >= 'a' && c <= 'z') return c - 32;
    if ((f & 0x300) == 0x100 && c >= 'A' && c <= 'Z') return c + 32;
    return c;
}
bool okflags(uint32_t f) { uint32_t m = f & 0x300; return m == 0x100 || m == 0x200; }
int lmw(uint32_t, uint32_t f, const uint16_t* s, int n, uint16_t* d, int dn) {
    if (!okflags(f) || n <= 0) return 0;
    if (!dn) return n;
    if (dn < n) return 0;
    for (int i = 0; i < n; i++) d[i] = (uint16_t)mapc(f, s[i]);
    return n;
}
int lma(uint32_t, uint32_t f, const char* s, int n, char* d, int dn) {
    if (!okflags(f) || n == 0) return 0;
    if (n < 0) n = (int)strlen(s) + 1;
    if (!dn) return n;
    if (dn < n) return 0;
    for (int i = 0; i < n; i++) d[i] = (char)mapc(f, (uint8_t)s[i]);
    return n;
}
int mbw(uint32_t, uint32_t, const char* s, int n, uint16_t* d, int dn) {
    if (n == 0) return 0;
    if (n < 0) n = (int)strlen(s) + 1;
    if (!dn) return n;
    if (dn < n) return 0;
    for (int i = 0; i < n; i++) d[i] = (uint8_t)s[i];
    return n;
}
int wmb(uint32_t, uint32_t, const uint16_t* s, int n, char* d, int dn) {
    if (!dn) return n;
    if (dn < n) return 0;
    for (int i = 0; i < n; i++) d[i] = (char)s[i];
    return n;
}
uint32_t gle() { return 0; }
int32_t lcp(uint32_t) { return 1252; }
char* conv(uint32_t, uint32_t, const char*, int*, char*, int) { return nullptr; }
}  // namespace
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_8644(uint32_t* r) {   // r[0..3] locale flags src len, r[5..8] dst dstLen cp err, r[9] g; r[4] result
    uint32_t* g = (uint32_t*)(uintptr_t)r[9];
    S8644Env e = { lmw, lma, mbw, wmb, gle, lcp, conv, (int)g[0], g[1], g[2] };
    r[4] = (uint32_t)Storm_8644(e, r[0], r[1], (const char*)(uintptr_t)r[2], (int)r[3], (char*)(uintptr_t)r[5], (int)r[6], r[7], (int)r[8]);
    g[3] = (uint32_t)e.mode;
}
