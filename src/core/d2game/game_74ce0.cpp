// 1.13c D2Game+0x74CE0: apply the AI parameter block's velocity to the unit's
// stat list as STAT_VELOCITYPERCENT (0x43), then let D2Common recompute the animation rate. Reads EAX = pAiParam
// (+0x10) and EDI = pUnit plus the unit's pool/owner id (+0x08/+0x0C) for the 0x4004 stat list it allocates when
// the unit has none with flag 0x4000 yet (that one is then posted to the unit, reset 1).
// 1:1 with the runtime disassembly at 6fc94ce0 (`mov ebx,[eax+0x10]` = the field, `push 0x4000` = the lookup
// flag, `push 0x4004/1/0` + the pool = the allocator's args, `or ecx,0xffffffff` = owner id -1 for a null unit,
// `push 0x43` = STAT_VELOCITYPERCENT, `push 0x10d` + the module literal 0x6FD1BD10 = __LINE__/__FILE__) and with the same
// block inlined in the vendored 1.10f source (MonsterMode.cpp:1933), which is the same rule.
#include "game_74ce0.h"

namespace {
constexpr uint32_t kGetStatListFlag = 0x4000;     // STATLIST_GetStatListFromUnitAndFlag: the anim/velocity list
constexpr uint32_t kNewStatListFilter = 0x4004;   // that flag plus the 0x4 the allocated list is created with
constexpr uint32_t kStatVelocityPercent = 0x43;   // STAT_VELOCITYPERCENT (src/core/d2moo/include/D2StatList.h:81)
constexpr uint32_t kOwnerType = 1;                // the allocator's nUnitType
constexpr uint32_t kOwnerIdNone = 0xFFFFFFFFu;    // the allocator's owner id for a null unit (the `or ecx,-1`)
constexpr uint32_t kPostReset = 1;                // PostStatToStatList's third argument, as the call site pushes it
constexpr uint32_t kFileRva = 0xFBD10;            // __FILE__ = the module literal 0x6FD1BD10 = D2Game (base 0x6FC20000) + 0xFBD10
constexpr uint32_t kLine = 269;                   // __LINE__ = 0x10D
}

uint32_t GAME_74CE0(D2G74CE0AiParam* pAiParam, D2G74CE0Unit* pUnit, D2G74CE0GetStatListFn getStatList,
                    D2G74CE0AllocStatListFn allocStatList, D2G74CE0PostStatListFn postStatList,
                    D2G74CE0SetStatFn setStat, D2G74CE0UpdateAnimFn updateAnim)
{
    const uint32_t nVelocity = pAiParam->nVelocity;
    if (!nVelocity)
    {
        return 0;
    }

    const uint32_t pUnit32 = (uint32_t)(uintptr_t)pUnit;
    uint32_t pStatList = getStatList(pUnit32, kGetStatListFlag);
    if (!pStatList)
    {
        // The pool/owner id come from the unit; with no unit EAX still holds the 0x4000 lookup's result, which is
        // 0 on this path (that is why the disassembly needs no `xor eax,eax` here), and ECX is forced to -1.
        pStatList = allocStatList(pUnit ? pUnit->pMemoryPool : 0, kNewStatListFilter, 0, kOwnerType,
                                  pUnit ? pUnit->dwUnitId : kOwnerIdNone);
        if (!pStatList)
        {
            return 0;
        }

        postStatList(pUnit32, pStatList, kPostReset);
    }

    setStat(pStatList, kStatVelocityPercent, nVelocity, 0);
    updateAnim(pUnit32, 0xB2000000u | kFileRva, kLine);   // 0xB2000000|rva: the project's module-address form

    return 0;   // void in retail; the caller discards EAX
}
