// 1.13c D2Game+0xBC7D0 -- EAX = value handed to 0xAC40, EBX = out (nullable), ESI = out max, EDI = out min, stack word = pGame (ret 4).
// Looks up a record by game+0x70 (0xAC40: stack words id, eax, 0, 0); null record zeroes the outs and returns -1; otherwise min = word +0x114,
// max = word +0x116, *ebx = dword +8 when ebx is non-null, result = max - min + 1. A result of -1 is the retail fatal-exit path (seam).
#include <cstdint>
#include <cstring>
struct GBC7D0Env {
    uint32_t (*lookup)(uint32_t id, uint32_t eax);   // 0xAC40 (stdcall 4; EAX is stack word 1)
    void (*fatal)();                                 // 0xA2DA/0xA2C2 report, then exit(-1)
};
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t rd16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
// 1.13c D2Game+0xBC7D0
int32_t g_bc7d0(uint32_t eax, uint32_t* ebx, uint32_t* esi, uint32_t* edi, uint32_t game, const GBC7D0Env& env)
{
    const uint32_t rec = env.lookup(rd32(game + 0x70), eax);
    if (!rec) { *edi = 0; *esi = 0; *ebx = 0; return -1; }
    *edi = rd16(rec + 0x114);
    *esi = rd16(rec + 0x116);
    if (ebx) *ebx = rd32(rec + 8);
    const int32_t n = (int32_t)(*esi - *edi) + 1;
    if (n == -1) env.fatal();
    return n;
}
