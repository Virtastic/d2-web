// 1.13c D2CMP+0x61AB (___wctomb_mt, VS2003 CRT): cdecl (locale, mbch, wch). Same body as D2Common+0x73E2,
// which this delegates to (the old crt file named a 4-arg symbol that does not exist in D2Common).
#include <cstdint>

extern "C" int32_t d2common_wctomb_mt_73e2(uint32_t locale, char* mbch, uint16_t wch);

// 1.13c D2CMP+0x61AB
extern "C" int32_t d2cmp_61ab(uint32_t locale, char* mbch, uint16_t wch) {
    return d2common_wctomb_mt_73e2(locale, mbch, wch);
}
