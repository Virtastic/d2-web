// 1.13c D2Game+0xEE260: EAX = spawn record (+0xC monster class), EDI = the spawned unit. The class goes through the
// monstats row (dt+0xA78, stride 0x1A8, count dt+0xA80; row +0x2 = base id); the base id selects the boss UMod the
// unit gets through 0x232B0 (EBX = umod, stack words unit, flag).
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xEE260
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_EE260(uintptr_t rec, uintptr_t unit, uintptr_t dt, const D2GameS3aEnv* env)
{
    const int32_t n = (int32_t)rd32(dt, 0xa80);
    int32_t id = (int32_t)rd32(rec, 0xc);
    if (id >= 0 && id < n) {
        const uint32_t rh = rd32(dt, 0xa78) + (uint32_t)id * 0x1a8;   // retail tests the 32-bit sum
        if (rh) id = (int16_t)rd16(env->p(rh), 2);
    }
    if (id < 0 || id >= (int32_t)rd32(dt, 0xa80)) return;
    const uint32_t u = env->h(unit);
    auto umod = [&](uint32_t m, uint32_t flag) { cl(env, 0x232B0, 0, 0, 0, m, 0, { u, flag }); };
    switch (id) {
    case 0x5b: umod(0x14, 0); return;
    case 0x18: umod(0xd, 0); return;
    case 0x60: umod(0xa, 0); return;
    case 0x146: case 0x147: case 0x148: case 0x149: case 0x14a: case 0x162: umod(0xe, 0); return;
    case 0x154: case 0x155: case 0x156: case 0x157: umod(0xf, 0); return;
    case 0xd3: umod(0xb, 0); umod(0x16, 1); return;
    case 0x21c: case 0x21d: case 0x21e: umod(0x16, 1); return;
    case 0x1cd: umod(0x21, 0); return;
    case 0x1b4: umod(0x22, 0); return;
    case 0x1f5: umod(0x23, 0); return;
    default: return;
    }
}
