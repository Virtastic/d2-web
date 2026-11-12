// 1.13c D2Game+0xB99D0, injectable callees. Fastcall: ECX = quest record (+0x6D = difficulty byte), EDX = unit,
// one unused stack word (ret 4). 0xA4C0 = D2Common 10920 (player data), 0xA568 = D2Common 10174 (quest flag),
// 0x354F0 takes AL = 0x17, ECX = unit and the quest record on the stack.
#include <cstdint>
#include <cstring>
struct GB99D0Env {
    uint32_t (*pdata)(uint32_t unit);
    int32_t  (*qflag)(uint32_t handle, uint32_t quest, uint32_t flag);
    void     (*f554f0)(uint32_t al, uint32_t unit, uint32_t game);
};
// 1.13c D2Game+0xB99D0
void g_b99d0(uint32_t game, uint32_t unit, const GB99D0Env& env)
{
    uint32_t pd = env.pdata(unit);
    uint8_t diff = *(const uint8_t*)(uintptr_t)(game + 0x6D);
    uint32_t h; memcpy(&h, (const void*)(uintptr_t)(pd + 0x10 + (uint32_t)diff * 4), 4);
    bool done = (env.qflag(h, 0x1A, 0) != 0 || env.qflag(h, 0x1A, 0xF) != 0)
             && env.qflag(h, 0x1A, 0xD) == 0 && env.qflag(h, 0x1A, 0xE) == 0;
    if (!done) env.f554f0(0x17, unit, game);
}
