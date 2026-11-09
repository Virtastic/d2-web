// 1.13c D2Game+0xEC1E0: pet item section ('fj') of the save writer; injectable callees.
// EAX = game, ECX/EDX pass through to the pet lookup (0x96A40: EAX = a1, EBX = 1, ESI = 7), stack a1..a5 (ret 0x14):
// a2 = &cursor, a3 = end, a4/a5 forwarded. 0x537D0 gets ECX = cursor, EDX = pet[0x60] and stack game, pet, end-cursor, 0, a4, a5.
#include <cstdint>
#include <cstring>
struct GEC1E0Env {
    uint32_t (*f96a40)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi);
    int32_t  (*f537d0)(uint32_t ecx, uint32_t edx, uint32_t game, uint32_t pet, uint32_t len, uint32_t zero, uint32_t a4, uint32_t a5);
};
// 1.13c D2Game+0xEC1E0
uint32_t g_ec1e0(uint32_t game, uint32_t ecx, uint32_t edx, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, const GEC1E0Env& env)
{
    auto rd = [](uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; };
    uint32_t* pCur = (uint32_t*)(uintptr_t)a2;
    uint32_t cur = *pCur;
    if (rd(game, 0x70) == 0) return 0;
    const uint16_t tag = 0x666a;
    std::memcpy((uint8_t*)(uintptr_t)cur, &tag, 2);
    cur += 2;
    uint32_t pet = env.f96a40(a1, ecx, edx, 1, 7);
    if (pet) {
        int32_t n = env.f537d0(cur, rd(pet, 0x60), game, pet, a3 - cur, 0, a4, a5);
        if (n < 0) return 5;
        cur += (uint32_t)n;
    }
    *pCur = cur;
    return 0;
}
