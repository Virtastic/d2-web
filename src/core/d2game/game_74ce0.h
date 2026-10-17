// 1.13c D2Game+0x74CE0: the velocity-percent application the 1.13c compiler
// factorised out of D2GAME_ModeChange_6FC65220 (vendored: src/core/d2moo/src/D2Game/MONSTER/MonsterMode.cpp,
// the inlined block at line 1933). Retail's own convention: EAX = pAiParam, EDI = pUnit, and the caller
// (0x75E10, at 6FC95FAE: `mov eax,ebx; mov edi,esi; call`) is what keeps pAiParam non-null -- the function
// itself dereferences EAX without a check. Retail 32-bit layout only: the two slices below pin exactly the
// fields the function reads, the way src/core/d2game/game_74f90.h pins its block.
#pragma once
#include <cstddef>
#include <cstdint>

#pragma pack(push, 1)
struct D2G74CE0AiParam {              // D2AiParamStrc
    uint32_t pad00[4];                // +0x00..+0x0C (never read here)
    uint32_t nVelocity;               // +0x10 read: the velocity 0x74F90 resolved and stored
};
struct D2G74CE0Unit {                 // D2UnitStrc, +0x08/+0x0C only
    uint32_t pad00[2];                // +0x00/+0x04
    uint32_t pMemoryPool;             // +0x08 retail 32-bit pointer: only its value is used (the allocator's pool)
    uint32_t dwUnitId;                // +0x0C the allocator's owner id
};
#pragma pack(pop)
static_assert(offsetof(D2G74CE0AiParam, nVelocity) == 0x10 && sizeof(D2G74CE0AiParam) == 0x14, "ai param field");
static_assert(offsetof(D2G74CE0Unit, pMemoryPool) == 0x08 && offsetof(D2G74CE0Unit, dwUnitId) == 0x0C, "unit fields");

// The five callees are all `jmp [IAT]` thunks of D2Common (ordinals 10195, 11013, 10807, 10261, 10819) and
// need live retail memory, so the caller hands them in as seams. Each takes the 32-bit pointer the retail
// build used; the replay probes the low half of the wasm address (as everywhere else in src/core). The
// trailing comment is the thunk's `ret n` -- the test runner pops that many bytes, so 0xA6A0 has five args.
typedef uint32_t (*D2G74CE0GetStatListFn)(uint32_t pUnit, uint32_t nFlag);                                      // 0xA7B4, ret 8
typedef uint32_t (*D2G74CE0AllocStatListFn)(uint32_t pMemPool, uint32_t fFilter, uint32_t dwTimeout,           // 0xA6A0,
                                            uint32_t nUnitType, uint32_t nUnitGUID);                            //   ret 20
typedef uint32_t (*D2G74CE0PostStatListFn)(uint32_t pUnit, uint32_t pStatList, uint32_t nReset);                // 0xA60A, ret 12
typedef uint32_t (*D2G74CE0SetStatFn)(uint32_t pStatList, uint32_t nStatId, uint32_t nValue, uint32_t nLayer);  // 0xA76C, ret 16
typedef uint32_t (*D2G74CE0UpdateAnimFn)(uint32_t pUnit, uint32_t pFile, uint32_t nLine);                       // 0xA59E, ret 12

// Retail's own order: EAX = pAiParam, EDI = pUnit. 0x74CE0 is `void` (plain `ret`; the only caller ignores EAX).
uint32_t GAME_74CE0(D2G74CE0AiParam* pAiParam, D2G74CE0Unit* pUnit, D2G74CE0GetStatListFn getStatList,
                    D2G74CE0AllocStatListFn allocStatList, D2G74CE0PostStatListFn postStatList,
                    D2G74CE0SetStatFn setStat, D2G74CE0UpdateAnimFn updateAnim);
