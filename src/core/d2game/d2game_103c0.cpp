// Roll the magic affix pair from the drop record (+0x68 prefix, +0x74 suffix; a negative index skips that roll),
// by item format. Returns 1 when either affix was set, then marks the item identified. Stack (ret 8): pP, drop.
#include <cstdint>
#include "d2game_itemqual.h"
static uint32_t d2g_103c0_set(const D2GItemQualEnv* env, uint32_t h, uint32_t b, uint32_t d, uint32_t e) {
    if (env->imp_b37e(h) >= 1) return env->sub144d0(h, 1, b, 1, d, e, 0);
    return env->sub148a0(h, 1, b, 1, d, e);
}
extern "C" uint32_t d2g_103c0(const uint32_t* pP, void* drop, const D2GItemQualEnv* env) {  // 1.13c D2Game+0x103C0
    const uint8_t* c = (const uint8_t*)drop;
    const int32_t c68 = *(const int32_t*)(c + 0x68);
    uint32_t fl = 0, ebx = 0, ebp = 0;
    bool second = true;
    if (c68 >= 0) {
        ebx = d2g_103c0_set(env, *pP, c68 > 0, 1, (uint32_t)c68);
        env->imp_b3c6(*pP, ebx, 0);
        if ((uint16_t)ebx == 0) fl = 1;
        else if (*(const int32_t*)(c + 0x74) < 0) second = false;
    } else fl = 1;
    if (second) {
        const int32_t c74 = *(const int32_t*)(c + 0x74);
        if (c74 > 0) fl = 1;
        ebp = d2g_103c0_set(env, *pP, fl, 0, (uint32_t)c74);
        env->imp_b3b4(*pP, ebp, 0);
    }
    if ((uint16_t)ebx == 0 && (uint16_t)ebp == 0) return 0;
    env->imp_a65e(*pP, 0x10, 0);
    env->sub13de0(*pP, drop);
    return 1;
}
