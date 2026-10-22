// Fog's statically linked MSVC CRT string/file entry points. Only those whose contract
// is independent of the MSVC FILE/locale internals are ported; the rest are CRT plumbing.
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>
// 1.13c Fog+0x2C0B: _fclose
extern "C" int fog_fclose(FILE* f) { return fclose(f); }
// 1.13c Fog+0x2C5C: _sprintf (writes the formatted text and the NUL, returns the char count)
extern "C" int fog_sprintf(char* dst, const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int n = vsprintf(dst, fmt, ap);
    va_end(ap);
    return n;
}
// 1.13c Fog+0x2CB4: __stricmp (C-locale branch: ASCII fold, difference of the first mismatching folded bytes)
extern "C" int fog_stricmp(const char* a, const char* b) {
    int x, y;
    do {
        x = (unsigned char)*a++; y = (unsigned char)*b++;
        if (x >= 'A' && x <= 'Z') x += 32;
        if (y >= 'A' && y <= 'Z') y += 32;
    } while (x && x == y);
    return x - y;
}
// 1.13c Fog+0x2D90: _strncpy (copy up to n, zero-pad the rest, no NUL if the source fills n)
extern "C" char* fog_strncpy(char* dst, const char* src, size_t n) {
    size_t i = 0;
    for (; i < n && src[i]; i++) dst[i] = src[i];
    for (; i < n; i++) dst[i] = 0;
    return dst;
}
