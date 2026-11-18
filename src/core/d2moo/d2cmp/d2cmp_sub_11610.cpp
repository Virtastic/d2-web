// 1.13c D2CMP.dll sub_11610 from the cached decompile. Error report (Ordinal_10265/10024) is a Fog seam.
#include <cstdint>
#include <cstdlib>

// 1.13c D2CMP+0x11610 (46 bytes): type tag (*p) must be 6, else report and _exit(-1); returns dword[5].
extern "C"
int32_t d2cmp_sub_11610(const int32_t* p, void (*report)(uint32_t line)) {
    if (p[0] != 6) { if (report) report(0x5d1); _Exit(-1); }
    return p[5];
}
