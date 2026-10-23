// src/core/d2game/d2game_batch_448e0.cpp -- 1.13c D2Game.dll sub_448E0 (RE-DONE, alpha lane), from build-wasm/ghidra-d2game/decomp/0x448E0.c.
// Skill aura-stat applicator (SkillDruid.cpp:248): for each of the 6 wAuraStat slots of a skills.txt record
// (+0x54 int16[6], calc +0x68 int32[6]) with a valid ItemStatCost row, evaluate the calc and set the stat on the list.
// in_EAX is the skills.txt record (null -> looked up from nSkillId). Callees are seams on the Env.
#include <cstdint>
#include <cstring>

struct D2GameSub448E0Env {
    const uint8_t* dataTables;                                              // *sgptDataTables_exref; +0xB98/+0xBA0 skills (0x23C), +0xBCC/+0xBD4 ItemStatCost (0x144)
    int  (*o10786)(void* unit, uint32_t calc, int skillId, int skillLvl);   // SKILLS_EvaluateSkillFormula (via 0xAC9A)
    void (*o10261)(void* statList, int stat, int value, int param);         // STATLIST_SetStat
    void (*o10188)(void* statList, int stat, int value, int param);         // STATLIST_SetStatIfListIsValid
    void (*o10819)(void* unit, const char* file, int line);                 // UNITS_UpdateAnimRateAndVelocity
};

// 1.13c D2Game+0x448E0
void sub_448E0(D2GameSub448E0Env& e, void* unit, void* statList, int skillId, int skillLvl, const uint8_t* rec) {
    if (!unit || !statList) return;
    uint32_t cnt, base, stc, stcN;
    std::memcpy(&cnt, e.dataTables + 0xba0, 4);
    std::memcpy(&base, e.dataTables + 0xb98, 4);
    if (!rec) {
        if (skillId < 0 || skillId >= (int)cnt) return;
        rec = (const uint8_t*)(uintptr_t)base + (size_t)skillId * 0x23c;
        if (!rec) return;
    }
    std::memcpy(&stc, e.dataTables + 0xbcc, 4);
    std::memcpy(&stcN, e.dataTables + 0xbd4, 4);
    for (int i = 0; i < 6; ++i) {
        int16_t stat; uint32_t calc;
        std::memcpy(&stat, rec + 0x54 + i * 2, 2);
        std::memcpy(&calc, rec + 0x68 + i * 4, 4);
        if (stat < 0 || stat >= (int)stcN || !((uintptr_t)stc + (size_t)stat * 0x144)) continue;
        int v = e.o10786(unit, calc, skillId, skillLvl);
        if (v) {
            e.o10261(statList, stat, v, 0);
            if (stat == 0x44) e.o10188(statList, 0x45, v, 0);
        }
    }
    e.o10819(unit, "..\\Source\\D2Game\\SKILLS\\SkillDruid.cpp", 0xf8);
}
