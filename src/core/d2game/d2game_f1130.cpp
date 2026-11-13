// 1.13c D2Game+0xF1130 -- player pair relation update. ECX = game, EDX = unit A, stack: unit B, param3, out ptr (ret 0xC).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void ff1130(GE& e, uint32_t game, uint32_t A, uint32_t B, uint32_t p3, uint32_t out)
{
    uint32_t loc = p3;   // retail hands F10F0 the address of its param3 slot
    auto outPath = [&] { HookEnv::wr32(out, 1); };
    auto okBlock = [&] { e.h(0xF05B0, B, A, 0, 0, 0, game); e.h(0xF05B0, A, B, 0, 0, 0, game); };
    const uint32_t rm = e.h(0xA478, 0, 0, 0, 0, 0, A);
    if (!rm || !e.h(0xA808, 0, 0, 0, 0, 0, rm)) { outPath(); return; }
    if ((int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, A, 0xC, 0) < 9) { outPath(); return; }
    if ((int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, B, 0xC, 0) < 9) { outPath(); return; }
    e.h(0xF02F0, B, A, 0, 0, 0, game);
    e.h(0xF02F0, A, B, 0, 0, 0, game);
    uint32_t r = e.h(0xF02A0, 0, 0, 0, 0, B, 8); e.edi(0xF02A0, A);
    if (r == p3) { okBlock(); return; }
    r = e.h(0xF02A0, 0, 0, 0, 0, B, 8); e.edi(0xF02A0, A);
    if (r == 0 && (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, B, 0xC, 0) < 9) return;
    if (p3 == 0) {
        e.h(0xF4210, B); e.edi(0xF4210, A);
        const uint32_t id = A ? HookEnv::rd32(A + 0xC) : 0xFFFFFFFFu;
        if (B && HookEnv::rd32(B) == 0) {
            const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, B);
            e.h(0xF0720, 0, 0, id, 0, 0, HookEnv::rd32(pd + 0x9C), 4);
        } else
            e.h(0xF0720, 0, 0, id, 0, 0, 0, 4);
        return;
    }
    const uint32_t v = e.h(0xF00E0, 0, 0, 0, 0, B); e.edi(0xF00E0, A);
    const uint32_t t = e.tick(0xC0001000u);
    if (v > t) {
        e.h(0xDFFD0, 0, 0, 0, 0, A, 0x13, A);
        e.h(0x70BF0, 0, 0, 0x10);
        okBlock();
        return;
    }
    const uint32_t t2 = e.tick(0xC0001000u);
    e.h(0xF0090, B, 0, 0, 0, 0, t2 + 0xEA60u);
    e.h(0xF0FF0, B, 0, 0, 0, 0, game, A);
    e.h(0x386F0, A);
    const uint32_t s = e.h(0xDFA60, A);
    if ((s & 0xFFFF) == 0xFFFF) return;
    if ((s & 0xFFFF) != (e.h(0xDFA60, B) & 0xFFFF)) return;
    e.h(0xF10F0, 0, game, A, 0, 0, 0, 0, e.stk(&loc));
}
}
