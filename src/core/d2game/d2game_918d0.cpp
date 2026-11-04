// 1.13c D2Game+0x918D0 (image-layout port): stdcall (unit, statlist, damage), ret 0xC. An aura-style modifier list moves a
// share of the physical damage into the skill's element. Retail addresses are 32-bit words read at their byte offsets;
// callees are seams whose register conventions follow the original.
#include <cstdint>
#include <cstring>
struct D2Game918D0Env {
    int32_t  (*getStat)(uint32_t list, uint32_t stat, uint32_t layer);              // 0xA712
    uint32_t (*getSkill)(uint32_t unit, uint32_t skillId);                           // 0xA3F4 (ECX unit, EDX skill id)
    int32_t  (*skillLevel)(uint32_t unit, uint32_t skill, uint32_t one);             // 0xA400 (3 stack)
    void     (*rollElem)(uint32_t skillId, uint32_t unit, uint32_t dmg, uint32_t level);   // 0x9F210 (EAX skill id, EDI unit)
    int32_t  (*divisor)(uint32_t skillId);                                           // 0x1C50 (EAX)
    int32_t  (*evalCalc)(uint32_t unit, uint32_t calc, uint32_t skillId, uint32_t level);  // 0xAC9A
    int32_t  (*percent)(int32_t pct, int32_t hundred, int32_t base);                 // 0x14D0 (EAX pct, ECX 100, EDX base)
    void     (*addElem)(uint32_t dmg, uint32_t eType, uint32_t unit, int32_t amount);       // 0x9EFE0 (ECX dmg, EDX type)
};
static uint32_t r32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t r16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
extern "C" void d2g_918d0(uint32_t unit, uint32_t list, uint32_t dmg, uint32_t dt, const D2Game918D0Env* e)
{
    const int32_t skillId = e->getStat(list, 0x15e, 0);
    if (skillId < 0 || skillId >= (int32_t)r32(dt + 0xba0)) return;
    const uint32_t rec = r32(dt + 0xb98) + (uint32_t)skillId * 0x23c;
    if (rec == 0) return;
    int32_t level = e->getStat(list, 0x15f, 0);
    const uint32_t skill = e->getSkill(unit, (uint32_t)skillId);
    const int32_t sl = e->skillLevel(unit, skill, 1);
    if (sl > level) level = sl;
    if (level <= 0) return;
    const int32_t aura = (int16_t)r16(rec + 0x54);
    if (aura < 0 || aura >= (int32_t)r32(dt + 0xbd4)) return;
    const int32_t v = e->getStat(list, (uint32_t)aura, 0);
    if (v <= 0) return;
    e->rollElem((uint32_t)skillId, unit, dmg, (uint32_t)level);
    if (*(const uint8_t*)(uintptr_t)(rec + 0x1dc) == 4 && v == 3) {
        const int32_t d = e->divisor((uint32_t)skillId);
        if (d) { const int32_t add = (int32_t)r32(dmg + 0x30) / d; const uint32_t n = r32(dmg + 0x34) + (uint32_t)add; memcpy((void*)(uintptr_t)(dmg + 0x34), &n, 4); }
    }
    *(uint8_t*)(uintptr_t)(dmg + 5) |= 0x40;
    int32_t pct = e->evalCalc(unit, r32(rec + 0x138), (uint32_t)skillId, (uint32_t)level);
    if (pct <= 0) return;
    if (pct >= 100) pct = 100;
    const int32_t base = (int32_t)r32(dmg + 8);
    int32_t x = e->percent(pct, 100, base);
    if (x >= base) x = base;
    const uint32_t nb = (uint32_t)(base - x); memcpy((void*)(uintptr_t)(dmg + 8), &nb, 4);
    e->addElem(dmg, *(const uint8_t*)(uintptr_t)(rec + 0x1dc), unit, x);
}
