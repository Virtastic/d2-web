// 1.13c D2Game monster AI steps (plain transcriptions of the cached decompiles, raw struct offsets as in
// d2game_batch_86900.cpp). Callees are weak externs named after their 1.13c address; the wasm link resolves
// them. Pointer-valued fields are read as 32-bit words and widened to intptr_t.
#include <cstdint>
#include <cstring>

#define W(a, o) (*(int32_t*)((intptr_t)(a) + (o)))
#define S(a, o) (*(int16_t*)((intptr_t)(a) + (o)))
#define UB(a, o) (*(uint8_t*)((intptr_t)(a) + (o)))
#define PTR(a, o) ((intptr_t)(uint32_t)W(a, o))
#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)

EXT(d2game_aisteps_e03a0);   // 0xE03A0 SUNIT_GetServerUnit (unit, id) -> unit
EXT(FUN_6fcfeee0);   // 0xDEEE0 (unit, unit, unit|0, flag): end the attack
EXT(FUN_6fd007a0);   // 0xE07A0 (unit)
EXT(FUN_6fd12380);   // 0xF2380 AITACTICS_UseSkill (unit)
EXT(FUN_6fd120c0);   // 0xF20C0 (value)
EXT(FUN_6fd12220);   // 0xF2220 AITACTICS_ChangeModeAndTargetUnit (unit, target)
EXT(FUN_6fd122c0);   // 0xF22C0 (value)
EXT(FUN_6fc21180);   // 0x1180 ITEMS_RollLimitedRandomNumber (seed*) -> roll
EXT(d2game_aisteps_ord10392);  // D2Common ordinal, (value) -> value

// 1.13c D2Game+0x7CB10 (AI_RollGatedSkillState): the monster AI step for gated skills. ai context (p3):
// +0 control, +0x08 target record, +0x14 threshold, +0x18 timer, +0x1c state; p1 monster; p2 unit (seed +0x20).
extern "C" void d2game_7cb10(intptr_t p1, intptr_t p2, intptr_t p3)
{
    const int32_t iVar1 = W(p3, 0x18);
    const int32_t iVar2 = W(p3, 0x14);
    const intptr_t iVar3 = PTR(p3, 0x08);
    const intptr_t iVar4 = PTR(p3, 0x00);
    const intptr_t iVar5 = PTR(p3, 0x1c);
    const uint32_t idx = (uint32_t)UB(p1, 0x6d) * 2;
    intptr_t iVar6 = 0;
    intptr_t uVar8 = 0;
    uint32_t r;

    // Roll: seed = lo*0x6ac690c5 + hi, stored back; returns seed mod 100 (low 32 bits).
    auto Roll100 = [&]() -> uint32_t {
        uint64_t s = (uint64_t)(uint32_t)W(p2, 0x20) * 0x6ac690c5ull + (uint32_t)W(p2, 0x24);
        memcpy((void*)(p2 + 0x20), &s, 8);
        return (uint32_t)s % 100;
    };
    auto dead = [&] { FUN_6fcfeee0(p1, p2, 0, 1); };
    auto set1 = [&] {
        FUN_6fd12380(p2);
        FUN_6fd120c0(8);
        W(iVar4, 0x18) = (int32_t)S(iVar5, 0x62 + idx) * 0x19 + W(p1, 0xa8);
        W(iVar4, 0x1c) = 1;
    };
    auto set2 = [&] {
        FUN_6fd12380(p2);
        W(iVar4, 0x18) = (int32_t)S(iVar5, 0x68 + idx) * 0x19 + W(p1, 0xa8);
        W(iVar4, 0x1c) = 2;
    };

    if (p2 == 0 || W(p2, 0) != 1 || PTR(p2, 0x14) == 0 ||
        (iVar6 = PTR(PTR(p2, 0x14), 0x28)) == 0 || W(iVar6, 0x28) == 0) {
        dead();
        return;
    }
    uVar8 = d2game_aisteps_e03a0(PTR(iVar6, 0x28), (uint32_t)W(iVar6, 0x2c));
    if (uVar8 == 0) { dead(); return; }
    const int32_t kind = W(uVar8, 0x10);
    if (kind == 0xc) {
        if (Roll100() < 0x28) {
            FUN_6fd007a0(p1);
            uint32_t uVar9 = (uint32_t)d2game_aisteps_ord10392((uint32_t)W(uVar8, 0x2c));
            FUN_6fcfeee0(p1, p2, (uVar9 == (uint32_t)uVar8) ? 0 : (intptr_t)uVar9, 1);
            return;
        }
    }
    if (S(iVar5, 0x170) >= 0) {
        if (W(iVar4, 0x1c) == 0) { set1(); return; }
        if (S(iVar5, 0x170) >= 0 && W(iVar4, 0x1c) == 2 && W(iVar4, 0x18) < W(p1, 0xa8)) {
            if (S(iVar5, 0x74 + idx) < iVar2) { set1(); return; }
            if (iVar1 == 0) {
                r = (uint32_t)FUN_6fc21180(p2 + 0x20);
                if ((int)r < (int)S(iVar5, 0x5c + idx)) { set1(); return; }
            }
            if (kind == 0xe) {
                r = (uint32_t)FUN_6fc21180(p2 + 0x20);
                if ((int)r < 0x32) { set1(); return; }
            }
        }
    }
    if (S(iVar5, 0x172) >= 0 && W(iVar4, 0x1c) == 1 && W(iVar4, 0x18) < W(p1, 0xa8)) {
        if (iVar1 != 0 || iVar2 < (int32_t)S(iVar5, 0x74 + idx)) { set2(); return; }
        if (kind != 0xe) {
            r = (uint32_t)FUN_6fc21180(p2 + 0x20);
            if ((int)r < 5) { set2(); return; }
        }
    }
    if (W(iVar4, 0x1c) == 1) {
        FUN_6fd120c0((int32_t)S(iVar5, 0x6e + idx));
        return;
    }
    if (iVar1 != 0) {
        r = (uint32_t)FUN_6fc21180(p2 + 0x20);
        if ((int)r < (int)S(iVar5, 0x56 + idx)) {
            FUN_6fd12220(p1, iVar3);
            return;
        }
    }
    FUN_6fd122c0((int32_t)S(iVar5, 0x6e + idx));
}
