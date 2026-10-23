// 1.13c Fog+0x08E70: statically linked MSVC CRT strcpy(dst, src); returns dst.
// Retail copies word-at-a-time with a zero-byte detector; the observable result
// (bytes up to and including the NUL written, dst returned) is the same.
extern "C" char* fog_strcpy_8e70(char* dst, const char* src) {  // 1.13c Fog+0x08E70
    char* d = dst;
    while ((*d++ = *src++) != 0) {}
    return dst;
}
