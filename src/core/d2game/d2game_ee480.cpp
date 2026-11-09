// 1.13c D2Game+0xEE480: EAX = spawn record (+0 unit id, +0xC monster class, +0x24 flags word), stack word 0 = owner unit (may be 0), ret 4.
// Spawns the monstats row's minions (row +0x26.. ids, count row +0x2C..+0x2D through the owner seed 0x11D0 (ECX = owner+0x20, ESI = range)).
// A row whose base id (0x1870, EAX = class) is 0x105 hands the whole group to 0xEE0A0 instead. Masks are gdwBitMasks[4] / [5].
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xEE480
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_EE480(uintptr_t rec, uintptr_t owner, uintptr_t dt, uintptr_t bm, const D2GameS3aEnv* env)
{
    const int32_t id = (int32_t)rd32(rec, 0xc);
    if (id < 0 || id >= (int32_t)rd32(dt, 0xa80)) return;
    const uint32_t rh = rd32(dt, 0xa78) + (uint32_t)id * 0x1a8;   // retail tests the 32-bit sum
    if (!rh) return;
    const uintptr_t row = env->p(rh);
    uint32_t loc = 0;
    cl(env, 0xACBE, 0, 0, 0, 0, 0, { (uint32_t)id, 0, env->h((uintptr_t)&loc) });
    const uint16_t w24 = rd16(rec, 0x24);
    const uint32_t rec0 = rd32(rec, 0);
    const int16_t r26 = (int16_t)rd16(row, 0x26);
    if (r26 < 0) return;
    const int32_t n = (int32_t)rd32(dt, 0xa80);
    if (r26 >= n) return;
    const int32_t base = (int32_t)cl(env, 0x1870, (uint32_t)id, 0, 0, 0, 0);
    const uint32_t oh = owner ? env->h(owner) : 0;
    const uint32_t oid = owner ? rd32(owner, 0xc) : 0xffffffffu;
    auto count = [&]() -> uint32_t {
        const uint32_t lo = rd8(row, 0x2c), hi = rd8(row, 0x2d);
        return (int32_t)lo < (int32_t)hi ? (uint32_t)cl(env, 0x11D0, 0, oh + 0x20, 0, 0, hi - lo + 1) + lo : lo;
    };
    auto monData = [&](uintptr_t u) { const uint32_t x = rd32(u, 0x14); return env->p(x ? rd32(env->p(x), 0x28) : 0); };
    if (base >= 0 && base < n && base == 0x105) {
        const uint32_t m = rd8(row, 0xc) & rd32(bm, 0x14);
        if (owner && rd32(owner, 0) == 1) {
            const uintptr_t md = monData(owner);
            if (m) wr8(md, 8, rd8(md, 8) | 2);
            wr16(md, 8, rd16(md, 8) | 1);
            wr32(md, 0x2c, oid); wr32(md, 0x30, 1); wr32(md, 0x28, rec0);
        }
        const uint32_t c = count();
        cl(env, 0xEE0A0, (~((uint32_t)w24 >> 2)) & 1, rec0, 0, 0, 0, { rec0, oh, (uint32_t)(int32_t)r26, 1, c, 0x40 });
        return;
    }
    if (rd32(bm, 0x10) & rd8(row, 0xc)) cl(env, 0x20D90, oh, 0, 1, 0, 0, { rec0, oid, 1, rd32(bm, 0x14) & rd8(row, 0xc) });
    const int16_t r28 = (int16_t)rd16(row, 0x28);
    const int32_t wrap = (r28 >= 0 && r28 < (int32_t)rd32(dt, 0xa80)) ? 1 : 0;
    int32_t c = (int32_t)count();
    if (c <= 0) return;
    int32_t idx = 0;
    do {
        const int32_t mid = (int16_t)rd16(row, 0x26 + idx * 2);
        const uint32_t res = (uint32_t)cl(env, 0xEDDB0, (uint32_t)mid, 1, oh, 0, 0, { rec0, 4, 0x40 });
        if (res && (rd8(bm, 0x10) & rd8(row, 0xc))) {
            const uintptr_t ru = env->p(res);
            if (rd32(ru, 0) == 1) { const uintptr_t md = monData(ru); wr32(md, 0x2c, oid); wr32(md, 0x30, 1); wr32(md, 0x28, rec0); }
            clE(env, res, 0x20CF0, oh, 0, 0, 0, 0, { rec0 });   // EDI = the spawned unit
        }
        if (++idx > wrap) idx = 0;
    } while (--c != 0);
}
