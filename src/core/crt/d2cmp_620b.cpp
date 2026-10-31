// 1.13c D2CMP+0x620B (_wctomb, VS2003 CRT): instruction-identical to D2Common+0x7442; delegates.
#include <cstdint>
struct D2CommonWctombEnv;
extern "C" int32_t D2Common_7442_Wctomb(char* mbch, uint16_t wch, const D2CommonWctombEnv* env);
// 1.13c D2CMP+0x620B
extern "C" int32_t d2cmp_620b(char* mbch, uint16_t wch, const D2CommonWctombEnv* env) {
    return D2Common_7442_Wctomb(mbch, wch, env);
}
