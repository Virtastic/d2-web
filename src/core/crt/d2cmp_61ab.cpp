// 1.13c D2CMP+0x61AB (___wctomb_mt, VS2003 CRT): instruction-identical to D2Common+0x73E2; delegates.
#include <cstdint>
struct D2CommonWctombEnv;
extern "C" int32_t D2Common_73E2_WctombMt(uint32_t locale, char* mbch, uint16_t wch, const D2CommonWctombEnv* env);
// 1.13c D2CMP+0x61AB
extern "C" int32_t d2cmp_61ab(uint32_t locale, char* mbch, uint16_t wch, const D2CommonWctombEnv* env) {
    return D2Common_73E2_WctombMt(locale, mbch, wch, env);
}
