// src/core/d2game/d2game_sub_bf90.cpp -- 1.13c D2Game.dll +0xBF90, pinned against the retail disassembly (the decompile's call order and
// arguments were wrong: it shows #10243/#10554(h,unit)/#10748(h,r)/#11031, the code calls 0xAC94(h,r), 0xACDC(h,L1), 0xAD0C(h,L2), 0xAB20(h,L3)).
// EDX = pUnit, ECX unused, two stack words passed through to 0x9EE50, ret 8. Callees are seams: D2Common #10511 (thunk 0xAABA), 0x9EE50 (register
// convention: EAX=&L2, ECX=&L1, EDX=&L3, then pUnit, 1, w0, w1; returns r), #10554 (0xAC94), #10748 (0xACDC), #11031 (0xAD0C), #10243 (0xAB20).
#include <cstdint>
#include <emscripten.h>

struct D2GameBf90Env {
    uint32_t (*o10511)(void* unit);
    uint32_t (*fn9EE50)(uint32_t* l2, uint32_t* l1, uint32_t* l3, void* unit, uint32_t one, uint32_t w0, uint32_t w1);
    void     (*o10554)(uint32_t h, uint32_t v);
    void     (*o10748)(uint32_t h, uint32_t v);
    void     (*o11031)(uint32_t h, uint32_t v);
    void     (*o10243)(uint32_t h, uint32_t v);
};

// 1.13c D2Game+0xBF90
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_bf90(uint32_t /*ecx*/, void* unit, uint32_t w0, uint32_t w1, const D2GameBf90Env* env)
{
    uint32_t l1 = 0, l2 = 0, l3 = 0;   // 9EE50 fills them in retail
    const uint32_t h = env->o10511(unit);
    if (h == 0) return 0;
    const uint32_t r = env->fn9EE50(&l2, &l1, &l3, unit, 1, w0, w1);
    env->o10554(h, r);
    env->o10748(h, l1);
    env->o11031(h, l2);
    env->o10243(h, l3);
    return 1;
}
