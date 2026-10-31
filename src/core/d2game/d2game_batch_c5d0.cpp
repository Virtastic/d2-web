// 1.13c D2Game+0xC5D0 (sub_C5D0, 211 B). Disassembled convention: ECX = a1 (pGame), EDX = pUnit, [esp+4] = skillId,
// [esp+8] = p4, ret 8. A skillId inside the skills table (count *(*sgptDataTables+0xBA0), 0x23C records at +0xB98)
// sets flag 0x40 in pUnit+0xC4, runs 0xE07A0 (EAX = pUnit), looks up the owner with D2Common #10392 (via the 0xA598
// thunk) on pUnit+0x2C; a non-null owner that is not pUnit and passes 0xC110 (ESI = owner) goes to 0xA00B0 with
// EAX = a nine-dword block {pUnit, owner, skillId, p4, 0, -1, 0, 0x6E, module+0xC000}; result 1.
#include <cstdint>

struct D2GameC5D0Env {
    int32_t skillCount;                        // *(*sgptDataTables + 0xBA0)
    const void* skillTable;                    // *(*sgptDataTables + 0xB98)
    void (*e07a0)(void* unit);                 // 0xE07A0, EAX = unit
    void* (*ord10392)(int32_t id);             // 0xA598 -> Ordinal 10392, stdcall(1)
    bool (*c110)(void* unit);                  // 0xC110, ESI = unit
    void (*a00b0)(const uint32_t* args);       // 0xA00B0, EAX = block
    uint32_t moduleC000;                       // module base + 0xC000
};

// 1.13c D2Game+0xC5D0
int32_t d2_fn_d2game_c5d0(void* a1, uint8_t* pUnit, int32_t skillId, int32_t p4, const D2GameC5D0Env* env)
{
    (void)a1;
    if (skillId < 0 || skillId >= env->skillCount || !(skillId * 0x23C + (intptr_t)env->skillTable)) return 0;
    if (pUnit) *(uint32_t*)(pUnit + 0xC4) |= 0x40;
    env->e07a0(pUnit);
    uint8_t* u = (uint8_t*)env->ord10392(*(int32_t*)(pUnit + 0x2C));
    if (u == pUnit || !u || !env->c110(u)) return 0;
    uint32_t blk[9] = { (uint32_t)(uintptr_t)pUnit, (uint32_t)(uintptr_t)u, (uint32_t)skillId, (uint32_t)p4, 0, 0xFFFFFFFFu, 0, 0x6E, env->moduleC000 };
    env->a00b0(blk);
    return 1;
}
