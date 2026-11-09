// 1.13c D2Game+0xEE7B0: EAX = spawn record (+0 game, +0xC monster class), ECX = the spawned unit; no stack words.
// The class goes through the monstats row (dt+0xA78, stride 0x1A8, count dt+0xA80; row +0x2 = base id). The base id
// picks the boss/quest setup: UMods through 0x232B0 (EBX = umod, ESI = game, stack words unit, flag), quest-list walks
// (game+0x10F4 -> head; node +0 type, +0xF4 next) handing the found node to 0x33F80 (ESI = node, EDI = unit, stack word
// game), unit flag bits, and for base 0x21C the inlined 0xEE6F0 tail (four static-table entries per unit class at
// D2Game+0xFB218 + class*8, stride 0x18, each through 0xAA42 and 0xAFCE0). `tbl` stands for D2Game+0xFB218.
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;

// 1.13c D2Game+0xEE6F0 (tail-jumped from the 0x21C case; EAX = unit)
static void ee6f0(uintptr_t unit, uintptr_t tbl, const D2GameS3aEnv* env)
{
    if (!unit) {
        const uint32_t r = (uint32_t)cl(env, 0xA2DA, 0, 0, 0, 0, 0, { 0xa2a });
        cl(env, 0xA2C2, 0, 0, 0, 0, 0, { env->addr(0xF9FF9), r });
        cl(env, 0x2A3C, 0, 0, 0, 0, 0, { 0xffffffffu });   // exit; never returns in retail
        return;
    }
    const uint32_t uh = env->h(unit);
    const uint32_t id = rd32(unit, 4);
    const uint32_t game = rd32(unit, 0x80);
    const uint32_t loc0 = (uint32_t)cl(env, 0xA42A, uh, 0, 0, 0, 0, { uh, 0xc, 0 });
    uintptr_t e = tbl + (uintptr_t)id * 8;
    for (int k = 0; k < 4; k++, e += 0x18) {
        uint32_t loc1 = 0, loc[2];
        loc[0] = rd32(e, 0); loc[1] = rd32(e, 4);
        const uint32_t rec = (uint32_t)cl(env, 0xAA42, 0, 0, 0, 0, 0, { loc[0], (uint32_t)(uintptr_t)&loc1 });
        const uint8_t diff = rd8(env->p(game), 0x6d);
        if (diff == 1) loc[0] = rd32(env->p(rec), 0x88);
        else if (diff == 2) loc[0] = rd32(env->p(rec), 0x8c);
        clB(env, loc, 2, 0xAFCE0, (uint32_t)(uintptr_t)loc, 0, 0, 0, 0, { game, loc0, 4 });
    }
}

// 1.13c D2Game+0xEE7B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_EE7B0(uintptr_t rec, uintptr_t unit, uintptr_t dt, uintptr_t tbl, const D2GameS3aEnv* env)
{
    const uint32_t gh = rd32(rec, 0);
    const uintptr_t game = env->p(gh);
    const int32_t cls = (int32_t)rd32(rec, 0xc);
    int32_t id = cls;
    if (cls >= 0 && cls < (int32_t)rd32(dt, 0xa80)) {
        const uint32_t rh = rd32(dt, 0xa78) + (uint32_t)cls * 0x1a8;   // retail tests the 32-bit sum
        if (rh) id = (int16_t)rd16(env->p(rh), 2);
    }
    if (id < 0 || id >= (int32_t)rd32(dt, 0xa80)) return;
    const uint32_t u = env->h(unit);
    auto umod = [&](uint32_t m, uint32_t flag = 1) { cl(env, 0x232B0, 0, 0, 0, m, gh, { u, flag }); };
    auto give = [&](uint32_t type) {   // walk the quest list for `type`; 0x33F80 on the node; true when found
        const uintptr_t n = quest(env, game, type);
        if (n) clE(env, u, 0x33F80, 0, 0, 0, 0, env->h(n), { gh });
        return n != 0;
    };
    switch (id) {
    case 0x124: umod(0x1f, 0); return;
    case 0x9c:
        if (cls == 0x2c3) { umod(0x17); umod(6); umod(0x1d); return; }
        umod(0x16); give(6); return;
    case 0xaf:
        cl(env, 0x1130, 0, u, 0, 0, 0);
        cl(env, 0x1100, 0, u, 0, 0, 0);
        return;
    case 0xd3:
        if (cls == 0x2c4) { umod(6); umod(0x12); return; }
        give(0xd); give(9); return;
    case 0xe5: give(8); return;
    case 0xf2:
        if (cls == 0x2c0) { umod(0x16); umod(0x1e); umod(0x11); umod(8); umod(6); return; }
        give(0x14); umod(0x16); return;
    case 0xf3:
        if (cls == 0x2c1) { umod(0x16); umod(8); umod(6); return; }
        umod(0x16); give(0x17); return;
    case 0xfa:
        give(0xc);
        if (!unit) return;
        wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x800);
        if (rd32(unit, 0) == 1) {
            const uint32_t d = rd32(unit, 0x14);
            if (d) wr8(env->p(d), 0x5c, rd8(env->p(d), 0x5c) | 1);
        }
        return;
    case 0x100:
        if (cls == 0x2c2) { umod(6); umod(0x12); return; }
        umod(0x16); give(0x16); return;
    case 0x10b:
        umod(0xc); umod(0x16); give(2);
        cl(env, 0xA3CA, 0, 0, 0, 0, 0, { u, 0x76, 1 });
        return;
    case 0x11c: umod(0x17); umod(0x16); return;
    case 0x154: case 0x155: case 0x156: case 0x157:
        if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x20000);
        return;
    case 0x16e:
        give(0x13);
        if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x20000);
        umod(0x16); return;
    case 0x192: umod(0x16); return;
    case 0x197: give(0x11); return;
    case 0x199: give(0x18); return;
    case 0x1b2: give(0x20); return;
    case 0x20e: give(0x22); umod(0x16); return;
    case 0x21c: ee6f0(unit, tbl, env); return;
    case 0x220:
        if (cls == 0x2c5) { umod(0x16); umod(0x12); umod(8); umod(6); return; }
        give(0x24); umod(0x16); return;
    default: return;
    }
}
