// 1.13c D2Multi+0xEC20: EAX = src, EBX = dst. Unless src starts with '*', and when it has a '*' later, copies the text before the first '*' into dst
// and terminates it; otherwise copies all of src (SStrCopy with a 0x7fffffff limit).
#include <cstdint>
#include <cstring>

void d2multi_ec20(const char* src, char* dst) {
    if (*src != '*') {
        const char* p = strchr(src, '*');
        if (p) { size_t n = (size_t)(p - src); memcpy(dst, src, n); dst[n] = 0; return; }
    }
    strcpy(dst, src);
}
