// Shared seams of the two elemental-enchant routines (D2Game+0x22320 poison, +0x22490 cold).
#pragma once
#include <cstdint>
struct D2GameEnchantEnv {
    uint32_t tables;                                                         // sgptDataTables (the struct, not the reference)
    int32_t (*getStat)(uint32_t unit, uint32_t id, uint32_t sub);            // 0xA42A (#10973)
    void (*addStat)(uint32_t unit, uint32_t id, int32_t v, uint32_t sub);    // 0xA4D2 (#10551)
    void (*resist)(uint32_t unit, uint32_t modifier, uint32_t unique);       // 0x21BE0: ECX = unit, EDX = modifier, stack unique
};
extern "C" void d2game_enchant_22320(uint32_t unit, uint32_t modifier, uint32_t unique, const D2GameEnchantEnv* env);
extern "C" void d2game_enchant_22490(uint32_t unit, uint32_t modifier, uint32_t unique, const D2GameEnchantEnv* env);
extern "C" void d2game_enchant_body_22320(uint32_t unit, uint32_t modifier, uint32_t unique, uint32_t firstStat, bool poison,
                                          const D2GameEnchantEnv* env);
