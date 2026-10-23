// 1.13c D2Win+0x06E3F: read a locale's LOCALE_IDEFAULTANSICODEPAGE (0x1004) as a number; -1 on failure.
#include <cstdint>
#include <cstdlib>

// getLocaleInfoA = kernel32 GetLocaleInfoA seam: returns chars written (0 = failure), fills buf[6].
int32_t d2win_sub_6e3f(uint32_t lcid, int (*getLocaleInfoA)(uint32_t, uint32_t, char*, int)) {
    char buf[7];
    buf[6] = 0;
    if (getLocaleInfoA(lcid, 0x1004, buf, 6) == 0) return -1;
    return (int32_t)atol(buf);
}
