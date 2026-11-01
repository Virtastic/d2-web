// Roll the rare affix pair from the drop record (+0x68 prefix, +0x74 suffix), by item format; one of them must
// come out set, else the assertion. Then mark the item identified. ESI = P, stack = drop (ret 4). Returns 1.
#include <cstdint>
#include "d2game_itemqual.h"
static uint32_t d2g_10240_set(const D2GItemQualEnv* env, uint32_t P, uint32_t b, uint32_t d, int32_t e) {
    if (env->imp_b37e(P) >= 1) return env->sub144d0(P, 1, b, 1, d, (uint32_t)e, 0);
    return env->sub148a0(P, 1, b, 1, d, (uint32_t)e);
}
extern "C" uint32_t d2g_10240(uint32_t P, void* drop, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x10240
    const uint8_t* c = (const uint8_t*)drop;
    uint32_t bx = 0, bp = 0, flag = 0;
    const int32_t c68 = *(const int32_t*)(c + 0x68);
    if (c68 > 0) {
        bp = d2g_10240_set(env, P, 1, 1, c68);
        env->imp_b3c6(P, bp, 0);
    } else {
        const int32_t c74 = *(const int32_t*)(c + 0x74);
        if (c74 <= 0) {
            bx = d2g_10240_set(env, P, 0, 1, c74);
            env->imp_b3c6(P, bx, 0);
            if ((uint16_t)bx == 0) flag = 1;
        }
    }
    const int32_t c74 = *(const int32_t*)(c + 0x74);
    if (c74 > 0) {
        bx = d2g_10240_set(env, P, 1, 0, c74);
        env->imp_b3b4(P, bx, 0);
    } else {
        const int32_t c68b = *(const int32_t*)(c + 0x68);
        if (c68b <= 0) {
            bp = d2g_10240_set(env, P, flag, 0, c68b);
            env->imp_b3b4(P, bp, 0);
        }
    }
    if ((uint16_t)bp == 0 && (uint16_t)bx == 0) env->fatal(P);
    env->imp_a65e(P, 0x10, 0);
    return 1;
}
