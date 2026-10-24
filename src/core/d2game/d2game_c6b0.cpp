// src/core/d2game/d2game_c6b0.cpp -- 1.13c D2Game.dll sub_C6B0 (RE-DONE, alpha lane), from the cached decompile
// build-wasm/ghidra-d2game/decomp/0xC6B0.c. Image base 0x6FC20000. unaff_EDI is the skill id (explicit parameter).
// Callees are seams: 0xAC9A is the Ordinal_10786 thunk; 0xA00B0 takes ambient registers (none visible);
// 0x448E0 is SkillDruid sub_6FCFE0E0-adjacent helper taking (unit, rec, skillId, unaff_EBX).
#include <cstdint>
#include <cstring>

struct D2GameC6B0Env {
    const uint8_t* dataTables;                               // *sgptDataTables_exref; +0xB98 skills base, +0xBA0 count, stride 0x23C
    void*    (*f0A00B0)();                                   // 0xA00B0
    void     (*o10786)(void* unit, uint32_t v);              // via 0xAC9A
    void     (*f0448E0)(void* unit, void* r, int skillId, uint32_t ebx); // 0x448E0
};

// see 1.13c D2Game+0x0C6B0
void sub_C6B0(D2GameC6B0Env& e, void* unit, int skillId, uint32_t ebx) {
    uint32_t cnt, base, v;
    std::memcpy(&cnt, e.dataTables + 0xba0, 4);
    std::memcpy(&base, e.dataTables + 0xb98, 4);
    if (skillId < 0 || skillId >= (int)cnt) return;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)base + (size_t)skillId * 0x23c;
    if (!rec) return;
    std::memcpy(&v, rec + 0x60, 4);
    e.o10786(unit, v);
    if (void* r = e.f0A00B0()) e.f0448E0(unit, r, skillId, ebx);
}
