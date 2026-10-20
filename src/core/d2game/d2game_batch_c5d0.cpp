// 1.13c D2Game+0x0C5D0 (sub_C5D0, 211 B), RE-DONE alpha lane, from the cached decompile.
// __fastcall(ecx=a1, edx=pUnit, stack/3rd=skillId). If skillId is inside the skills table (count at
// *(*sgptDataTables+0xBA0), records 0x23C bytes at +0xB98) it sets flag 0x40 in pUnit+0xC4, calls
// 0xE07A0(a1), looks up Ordinal 10392 (via 0xA598) on pUnit+0x2C; when that unit is non-null and
// not pUnit and 0xC110 accepts it, 0xA00B0 runs (register args, opaque here) and the result is 1.
#include <cstdint>

struct D2GameC5D0Env {
    int32_t skillCount;                        // *(*sgptDataTables + 0xBA0)
    const void* skillTable;                    // *(*sgptDataTables + 0xB98)
    void (*e07a0)(void* a1);                   // 0xE07A0
    void* (*ord10392)(int32_t id);             // 0xA598 -> Ordinal 10392
    bool (*c110)(void* unit);                  // 0xC110
    void (*a00b0)();                           // 0xA00B0
};

int32_t d2_fn_d2game_c5d0(void* a1, uint8_t* pUnit, int32_t skillId, const D2GameC5D0Env* env)
{
    if (skillId < 0 || skillId >= env->skillCount || !(skillId * 0x23C + (intptr_t)env->skillTable)) return 0;
    if (pUnit) *(uint32_t*)(pUnit + 0xC4) |= 0x40;
    env->e07a0(a1);
    void* u = env->ord10392(*(int32_t*)(pUnit + 0x2C));
    if (u != pUnit && u && env->c110(u)) { env->a00b0(); return 1; }
    return 0;
}
