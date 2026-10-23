// src/core/d2game/d2game_batch_50470.cpp -- 1.13c D2Game.dll sub_50470 (RE-DONE, alpha lane), from build-wasm/ghidra-d2game/decomp/0x50470.c.
// Builds a 0x68-byte arg block for skill 0x47 (skills.txt row at +0x9F04 base: 0x47*0x23C) and hands it to 0x50250.
// Ghidra shows register/stack operands (iStack_c, piStack_8, iStack_4, iStack_10) without a definition; they are
// explicit parameters here in the order they are read. Callees are seams on the Env.
#include <cstdint>
#include <cstring>

struct D2GameSub50470Env {
    const uint8_t* dataTables;                                                  // +0xB98/+0xBA0 skills (0x23C), +0xBCC/+0xBD4 stat cost (0x144)
    void     (*o10786a)(void* unit, uint32_t calc, int skillId);                // first Ordinal_10786 call (result unused)
    void     (*o10237)(uint8_t v);                                              // Ordinal_10237(*(byte*)(unitB+0x6d))
    uint32_t (*o10786b)(void* unit, uint32_t calc, int skillId, int skillLvl);  // SKILLS_EvaluateSkillFormula
    void     (*f50250)(void* a, int32_t* block);                                // 0x50250
};

// 1.13c D2Game+0x50470
void sub_50470(D2GameSub50470Env& e, void* p2, void* unitB, void* unitC, int skillLvl, int32_t iStack10, void* piStack8, const uint8_t* iStack4) {
    uint32_t cnt, base, stc, stcN;
    std::memcpy(&cnt, e.dataTables + 0xba0, 4);
    if (cnt <= 0x47) return;
    std::memcpy(&base, e.dataTables + 0xb98, 4);
    if (base == (uint32_t)-0x9ea4) return;
    std::memcpy(&stc, e.dataTables + 0xbcc, 4);
    std::memcpy(&stcN, e.dataTables + 0xbd4, 4);
    const uint8_t* rec = (const uint8_t*)(uintptr_t)base;
    uint32_t v9f04; std::memcpy(&v9f04, rec + 0x9f04, 4);
    e.o10786a(p2, v9f04, 0x47);
    e.o10237(((const uint8_t*)unitB)[0x6d]);
    int32_t blk[0x1a] = {};            // 0x68 bytes: [0]=iStack_7c, [6..11]=aiStack_60 ids, [12..17]=auStack_48 levels, [0x16]=iStack_30
    blk[0] = iStack10;
    int n = 0;
    for (int i = 0; i < 6; ++i) {
        int16_t id; std::memcpy(&id, rec + 0x9ef8 + i * 2, 2);
        if (id < 0 || id >= (int)stcN) { blk[6 + n] = -1; goto done; }
        if (!((uintptr_t)stc + (size_t)id * 0x144)) break;
        uint32_t calc; std::memcpy(&calc, rec + 0x9f0c + i * 4, 4);
        blk[6 + n] = id;
        blk[12 + n] = (int32_t)e.o10786b(unitC, calc, 0x47, skillLvl);
        ++n;
    }
    if (n == 6) {}                      // loop ran out: no -1 terminator (retail falls through to LAB_6fc70599)
done:
    int16_t s82; std::memcpy(&s82, iStack4 + 0x82, 2);
    blk[0x16] = s82;
    e.f50250(piStack8, blk);
}
