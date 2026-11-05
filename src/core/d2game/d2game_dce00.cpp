// 1.13c D2Game+0xDCE00, injectable callees. EAX = unit (EDI), ESI = table index, EBX = pass-through, stack word 0 =
// out record (ret 4). Guards the index against the table block (pointer behind the 0xF829C cell: +0xBA0 count,
// +0xB98 base of 0x23C-byte rows), reads two ordinals, scales through 0xDC530 and stores the result at record+8,
// then calls 0x9F210. 0xA796 and 0xA766 take (unit, idx, ebx, 0); 0xDC530 takes EAX = 0, ECX = a766 result,
// EDX = unit and stack (0, a796 result, 0, 0, 0x80); 0x9F210 takes EAX = idx and stack (record, ebx).
#include <cstdint>
struct GDCE00Env {
    uint32_t (*a796)(uint32_t unit, uint32_t idx, uint32_t ebx, uint32_t z);
    uint32_t (*a766)(uint32_t unit, uint32_t idx, uint32_t ebx, uint32_t z);
    uint32_t (*dc530)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    void     (*f9f210)(uint32_t eax, uint32_t rec, uint32_t ebx);
};
// 1.13c D2Game+0xDCE00; dt = the tables block (what the 0xF829C cell points at)
void g_dce00(uint32_t unit, int32_t idx, uint32_t ebx, uint32_t rec, uint32_t dt, const GDCE00Env& env)
{
    if (idx < 0) return;
    if (idx >= *(const int32_t*)(uintptr_t)(dt + 0xBA0)) return;
    if ((uint32_t)idx * 0x23Cu + *(const uint32_t*)(uintptr_t)(dt + 0xB98) == 0) return;
    const uint32_t mx = env.a796(unit, (uint32_t)idx, ebx, 0);
    const uint32_t mn = env.a766(unit, (uint32_t)idx, ebx, 0);
    const uint32_t r = env.dc530(0, mn, unit, 0, 0, mx, 0, 0x80);
    *(uint32_t*)(uintptr_t)(rec + 8) = r;
    env.f9f210((uint32_t)idx, rec, ebx);
}
