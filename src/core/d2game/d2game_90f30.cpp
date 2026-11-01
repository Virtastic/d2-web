// 1.13c D2Game+0x90F30 -- walk the set bits of a unit's state mask and clear the ones the mask table selects.
// Stack (ret 4) = unit. Returns at once when 0xAF88(unit) is 0. Otherwise copies ceil(dt+0xC4 / 32) words from 0xAFAC(unit) to a
// local buffer, and for each set bit (lowest first, found with Fog 0xA2F8) with index n = bit + 32 * word: when
// maskTab[n & 31] & dt[+0xE4][n >> 5] is non-zero, runs 0xA5D4(unit, n) -> object (if non-zero 0xABB6(unit, object), 0xABAA(object))
// and then 0xA3CA(unit, n, 0). dt = [[0x6FD1829C]], maskTab = [0x6FD18BE0].
#include "d2game_8d6_common.h"
struct D2Game90F30Env {
    uint32_t (*subAF88)(uint32_t unit);                               // 0xAF88
    uint32_t (*subAFAC)(uint32_t unit);                               // 0xAFAC
    int32_t (*bitScan)(uint32_t word);                                // 0xA2F8
    uint32_t (*sub5D4)(uint32_t unit, uint32_t n);                    // 0xA5D4
    void (*subBB6)(uint32_t unit, uint32_t obj);                      // 0xABB6
    void (*subBAA)(uint32_t obj);                                     // 0xABAA
    void (*sub3CA)(uint32_t unit, uint32_t n, uint32_t zero);         // 0xA3CA
};
// 1.13c D2Game+0x90F30
void d2game_port_90f30(uint32_t unit, uint32_t dt, uint32_t maskTab, const D2Game90F30Env* env)
{
    if (!env->subAF88(unit)) return;
    const uint32_t src = env->subAFAC(unit);
    const int32_t bits = (int32_t)d2g8::r32(dt, 0xC4) + 0x1F;
    const int32_t nwords = (bits + (int32_t)(((uint32_t)(bits >> 31)) & 0x1F)) >> 5;
    uint32_t buf[8] = {0};
    for (int32_t i = 0; i < nwords && i < 8; ++i) buf[i] = d2g8::r32(src, (uint32_t)i * 4);
    uint32_t base = 0;
    for (int32_t i = 0; i < nwords; ++i, base += 32) {
        uint32_t w = buf[i];
        int32_t bit = env->bitScan(w);
        while (bit >= 0) {
            const uint32_t n = (uint32_t)bit + base;
            w &= ~(1u << (bit & 31));
            const uint32_t word = d2g8::r32(d2g8::r32(dt, 0xE4), (n >> 5) * 4);
            if (d2g8::r32(maskTab, (n & 0x1F) * 4) & word) {
                const uint32_t obj = env->sub5D4(unit, n);
                if (obj) { env->subBB6(unit, obj); env->subBAA(obj); }
                env->sub3CA(unit, n, 0);
            }
            bit = env->bitScan(w);
        }
    }
}
