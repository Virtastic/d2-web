// Extra callee seams of the 1.13c D2Game AI think ports 0x82990, 0x85B60, 0x864A0, 0x86900 (the callees D2Game77dEnv and
// D2GameAiExt lack, or take with fewer registers than these callers load). Each port takes D2Game77dEnv, D2GameAiExt and this;
// the FN probes bind all three to HookEnv recorders (src/core/fn/d2game_o2_probe.h).
#pragma once
#include <cstdint>
#include "d2game_aiext_env.h"

struct D2GameO2Ext {
    // Import thunks (stdcall): 0xA42A(unit, stat, layer) = STATLIST_GetUnitStat; 0xA472(unit, stat, value, layer) = STATLIST_SetUnitStat.
    uint32_t (*thA42A)(uint32_t unit, uint32_t stat, uint32_t layer);
    void     (*thA472)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer);
    // 0xF22C0(eax = game, esi = unit, stack value): these callers load esi with the unit.
    void     (*subF22C0s)(uint32_t game, uint32_t unit, uint32_t value);
    // 0xF2380 with the full EAX its callers leave (the skill word sign-extended, its low byte replaced by the mode byte); ecx unit, ebx game.
    void     (*subF2380a)(uint32_t eax, uint32_t unit, uint32_t game, uint32_t skill, uint32_t pTarget, uint32_t z0, uint32_t z1);
    // 0xF2F30(eax = byte, ecx = target, edx = unit, stack game, byte), ret 8.
    void     (*subF2F30)(uint32_t target, uint32_t unit, uint32_t game, uint32_t byteArg);
    // The 0x85B60 callees whose results it tests: 0xF26B0(eax, ecx = unit, game, target, kind), 0xF2870(al, ecx = target, game, unit),
    // 0xF1D90(eax = mode, ecx = target, game, unit, a, b), the 0xAD78 thunk(unit, target, 4) (stdcall).
    uint32_t (*subF26B0r)(uint32_t eax, uint32_t unit, uint32_t game, uint32_t pTarget, uint32_t kind);
    uint32_t (*subF2870r)(uint32_t target, uint32_t game, uint32_t unit);
    uint32_t (*subF1D90r)(uint32_t mode, uint32_t target, uint32_t game, uint32_t unit, uint32_t a, uint32_t b);
    uint32_t (*thAD78)(uint32_t unit, uint32_t pTarget, uint32_t four);
    // 0x84720(edi = game, esi = unit, ebx = MonStats row, stack target): the target the caster turns to (local helper).
    uint32_t (*sub84720)(uint32_t game, uint32_t unit, uint32_t row, uint32_t target);
    // 0x8E6F0(eax = node): releases one unit-state node of type 2 (unit+0xDC list).
    void     (*sub8E6F0)(uint32_t node);
};
