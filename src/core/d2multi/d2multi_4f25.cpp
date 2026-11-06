// 1.13c D2Multi+0x4F25 ___free_lc_time with _free as a seam; retail's order [1..6],[0],[8..0xD],[7],[0xE..0x2A].
#include <cstdint>

struct D2MultiFreeLcTimeEnv { void (*free_)(uint32_t); };

extern "C" void d2multi_free_lc_time_env(const uint32_t* p, const D2MultiFreeLcTimeEnv* env) {
    if (!p) return;
    static const int order[] = {1,2,3,4,5,6,0,8,9,10,11,12,13,7};
    for (int i : order) env->free_(p[i]);
    for (int i = 0xe; i <= 0x2a; i++) env->free_(p[i]);
}
