// D2Multi batch crt1: statically linked MSVC CRT helpers. Win32 file/heap/SEH state does not exist
// under wasm; each function keeps retail's observable contract on the host libc.
// ponytail: GetStringTypeA is a C-locale classifier (CT_CTYPE1 only); SEH unwind is a no-op.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

extern "C" {

// 1.13c D2Multi+0x463D
int32_t d2_multi__lseek_lk(uint32_t fd, int32_t off, uint32_t whence) {
    off_t r = lseek((int)fd, off, (int)whence);   // origin 0/1/2 == SEEK_SET/CUR/END
    return r < 0 ? -1 : (int32_t)r;
}

// 1.13c D2Multi+0x4C2B
void* d2_multi___heap_alloc(size_t size) {
    return malloc(size ? size : 1);               // retail: HeapAlloc, size 0 -> 1
}

// 1.13c D2Multi+0x4D6B
int32_t d2_multi____crtGetStringTypeA(void*, uint32_t infoType, const char* src, int32_t cch,
                                      uint16_t* out, int32_t, int32_t) {
    if (!src || !out || infoType != 1) return 0;  // CT_CTYPE1
    if (cch < 0) cch = (int32_t)strlen(src) + 1;
    for (int32_t i = 0; i < cch; i++) {
        uint8_t c = (uint8_t)src[i];
        uint16_t t = 0;
        if (c >= 'A' && c <= 'Z') t = 0x101;                       // upper|alpha
        else if (c >= 'a' && c <= 'z') t = 0x102;                  // lower|alpha
        else if (c >= '0' && c <= '9') t = 0x4;                    // digit
        else if (c == ' ') t = 0x140;                              // space|blank
        else if (c >= 9 && c <= 13) t = 0x20 | 0x8;                // cntrl|space
        else if (c < 32 || c == 127) t = 0x20;                     // cntrl
        else if (c < 127) t = 0x10;                                // punct
        if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9')) t |= 0x80;
        if (c == '\t') t |= 0x40;
        out[i] = t;
    }
    return 1;
}

// 1.13c D2Multi+0x5200   (strcat)
char* d2_multi_sub_5200(char* dst, const char* src) { strcpy(dst + strlen(dst), src); return dst; }

// 1.13c D2Multi+0x52F0
size_t d2_multi__strcspn(const char* s, const char* ctl) {
    uint8_t bits[32] = {0};
    for (; *ctl; ctl++) bits[(uint8_t)*ctl >> 3] |= 1 << ((uint8_t)*ctl & 7);
    size_t n = 0;
    for (; s[n] && !(bits[(uint8_t)s[n] >> 3] >> ((uint8_t)s[n] & 7) & 1); n++) {}
    return n;
}

// 1.13c D2Multi+0x5340
void* d2_multi__memset(void* d, int v, size_t n) {
    uint8_t* p = (uint8_t*)d;
    while (n--) *p++ = (uint8_t)v;
    return d;
}

// 1.13c D2Multi+0x5430   (retail copies overlap-safely)
void* d2_multi__memcpy(void* d, const void* s, size_t n) { return memmove(d, s, n); }

// 1.13c D2Multi+0x58B0
void d2_multi___global_unwind2(void*) {}

// 1.13c D2Multi+0x58F2
void d2_multi___local_unwind2(void*, int32_t) {}

// 1.13c D2Multi+0x5986
void d2_multi_sub_5986(void) {}

// 1.13c D2Multi+0x599E   (codepage in EAX -> LANGID)
uint32_t d2_multi_sub_599E(int32_t cp) {
    switch (cp) { case 0x3a4: return 0x411; case 0x3a8: return 0x804; case 0x3b5: return 0x412; case 0x3b6: return 0x404; }
    return 0;
}

// 1.13c D2Multi+0x59CD
uint8_t d2_multi_setSBCS_tab[0x104];
void d2_multi_setSBCS(void) { memset(d2_multi_setSBCS_tab, 0, sizeof d2_multi_setSBCS_tab); }

}
