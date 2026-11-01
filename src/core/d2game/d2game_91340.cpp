// 1.13c D2Game+0x91340 -- pick a variant word from an entry of the 0x23C-byte table at dt+0xB98.
// EAX = index (signed; < 0 or >= dt+0xBA0 -> -1), stack (ret 4) = owner. dt = [[0x6FD1829C]]; the mask pointer is [0x6FD18BE0]
// (byte +8 tested against entry byte +4). Default result: sign-extended word entry+0x48. When the mask hits and the entry's
// words +0x80 (< dt+0xC4) and +0x54 (< dt+0xBD4) are in range, 0xA5D4(owner, word80) gives an object and 0xA712(object, word54, 0)
// a rank: rank > 1 returns word entry+0x4A (rank 2) or entry+0x4C (rank >= 3), otherwise the default word.
#include "d2game_8d6_common.h"
struct D2Game91340Env {
    uint32_t (*sub5D4)(uint32_t owner, uint32_t idx);                        // 0xA5D4
    int32_t (*sub712)(uint32_t obj, uint32_t idx, uint32_t zero);            // 0xA712
};
// 1.13c D2Game+0x91340
int32_t d2game_port_91340(int32_t index, uint32_t owner, uint32_t dt, uint32_t maskPtr, const D2Game91340Env* env)
{
    if (index < 0 || index >= (int32_t)d2g8::r32(dt, 0xBA0)) return -1;
    const uint32_t e = (uint32_t)index * 0x23C + d2g8::r32(dt, 0xB98);
    if (!e) return -1;
    const int32_t def = (int16_t)d2g8::r16(e, 0x48);
    if (!(d2g8::r8(maskPtr, 8) & d2g8::r8(e, 4))) return def;
    const int32_t w80 = (int16_t)d2g8::r16(e, 0x80);
    if (w80 < 0 || w80 >= (int32_t)d2g8::r32(dt, 0xC4)) return def;
    const int32_t w54 = (int16_t)d2g8::r16(e, 0x54);
    if (w54 < 0 || w54 >= (int32_t)d2g8::r32(dt, 0xBD4)) return def;
    const uint32_t obj = env->sub5D4(owner, (uint32_t)w80);
    if (!obj) return def;
    const int32_t rank = env->sub712(obj, (uint32_t)w54, 0);
    if (rank <= 1) return def;
    return (int16_t)d2g8::r16(e, 0x46 + 2 * (rank < 3 ? rank : 3));
}
