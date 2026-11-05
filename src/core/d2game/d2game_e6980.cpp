// 1.13c D2Game+0xe6980, injectable callees. Registers: ECX = game (byte +0x6d picks the player slot), EDX = unit; one stack word ignored (ret 4).
// 0xA4C0 returns the player block (slot words at +0x10); 0xA568(value, 0x15, 0xD) tests the quest flag; 0x9BEB0 (EAX = unit) gives the party id
// (its low 16 bits are -1 for none); 0x9C160 registers callback 0xE37D0. Always returns 0.
#include <cstdint>
#include <cstring>
struct GE6980Env {
    uint32_t (*a4c0)(uint32_t unit);
    uint32_t (*a568)(uint32_t value, uint32_t quest, uint32_t flag);
    uint32_t (*f9beb0)(uint32_t unit);   // EAX = unit
    void (*f9c160)(uint32_t eax, uint32_t ebx, uint32_t esi, uint32_t cb);   // EAX = party id, EBX = 0, ESI = game, callback RVA on the stack (ret 4)
};
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
// 1.13c D2Game+0xe6980
uint32_t g_e6980(uint32_t ecx, uint32_t edx, uint32_t w0, const GE6980Env& env)
{
    (void)w0;
    uint32_t plr = env.a4c0(edx);
    uint32_t slot = rd32(plr + 0x10 + 4 * (rd32(ecx + 0x6c) >> 8 & 0xFF));
    if (env.a568(slot, 0x15, 0xD) == 0 || edx == 0) return 0;
    if (rd32(edx + 0x80) == 0 || rd32(edx) != 0) return 0;
    uint32_t pid = env.f9beb0(edx);
    if ((pid & 0xFFFF) != 0xFFFF) env.f9c160(pid, 0, ecx, 0xE37D0);
    return 0;
}
