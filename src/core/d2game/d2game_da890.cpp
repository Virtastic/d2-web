// 1.13c D2Game+0xDA890 ItemPostPlaceNotify, injectable stat accessors.
// EAX = source unit, EDI = target unit (both register inputs). Plain ret.
#include <cstdint>
struct GDA890Env {
    int32_t (*get78)(uint32_t unit, uint32_t id, uint32_t idx);            // thunk 0xA802 (stat 0x78)
    int32_t (*get1f)(uint32_t unit, uint32_t id, uint32_t idx);            // thunk 0xA42A
    void    (*set1f)(uint32_t unit, uint32_t id, uint32_t val, uint32_t idx);   // thunk 0xA472
};
// 1.13c D2Game+0xDA890
void g_da890(uint32_t eax, uint32_t edi, const GDA890Env& env)
{
    int32_t bonus = env.get78(eax, 0x78, 0);
    if (bonus == 0) return;
    int32_t v = env.get1f(edi, 0x1F, 0) + bonus;
    if (v < 0) v = 0;
    env.set1f(edi, 0x1F, (uint32_t)v, 0);
}
