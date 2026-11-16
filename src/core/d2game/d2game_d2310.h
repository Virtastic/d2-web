// 1.13c D2Game+0xD2310: ESI = unit, EDI = other unit, EBX = flag; no stack arguments, no result.
// Either unit null: nothing. The flag picks the pair getter (0xA8EC when set, 0xAA90 when clear), called (unit, &l1, &l0). 0xA3D0
// (ECX = unit, EDX = l1, stack l0) answers a record; a null record or the other unit itself ends it. 0xA8C8 (unit, record) answers a
// mode: 2 and 7 end it, anything else calls 0xA2100 (ECX = unit, EDX = flag, stack l1, l0).
// The batch file's d2_fn_d2game_D2310 keeps the decompile's argument-less callees; this one carries every argument retail passes.
#pragma once
#include <cstdint>
struct D2GameD2310Env {
    void     (*pairSet)(void* unit, int32_t* l1, int32_t* l0);      // 0xA8EC
    void     (*pairClear)(void* unit, int32_t* l1, int32_t* l0);    // 0xAA90
    void*    (*record)(void* unit, int32_t l1, int32_t l0);         // 0xA3D0
    int32_t  (*mode)(void* unit, void* rec);                        // 0xA8C8
    void     (*subA2100)(void* unit, uint32_t flag, int32_t l1, int32_t l0);
};
inline void d2g_d2310(void* esi, void* edi, uint32_t ebx, const D2GameD2310Env* env)
{
    if (!esi || !edi) return;
    int32_t l0 = 0, l1 = 0;
    if (ebx) env->pairSet(esi, &l1, &l0); else env->pairClear(esi, &l1, &l0);
    void* rec = env->record(esi, l1, l0);
    if (!rec || rec == edi) return;
    const int32_t m = env->mode(esi, rec);
    if (m == 2 || m == 7) return;
    env->subA2100(esi, ebx, l1, l0);
}
