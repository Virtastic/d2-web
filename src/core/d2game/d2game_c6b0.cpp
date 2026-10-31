// src/core/d2game/d2game_c6b0.cpp -- 1.13c D2Game+0xC6B0, the skill-record trigger (decompile build-wasm/ghidra-d2game/decomp/0xC6B0.c,
// disassembly for the register convention). Registers: EDX = block word 1, EDI = skill id, EBX = block word 3; [esp+4] = pUnit; ret 4.
// Callees (all seams): 0xAC9A (IAT thunk of Ordinal 10786, stdcall 4: unit, rec+0x60, skillId, ebx -> r),
// 0xA00B0 (EAX = nine-dword stack block -> eax; plain ret), 0x448E0 (EAX = record, stack unit, r, skillId, ebx, ret 0x10).
#include <cstdint>
#include <cstring>

struct D2GameC6B0Env {
    const uint8_t* dataTables;                                                  // *sgptDataTables: +0xB98 records (0x23C), +0xBA0 count
    uint32_t (*o10786)(void* unit, uint32_t name, int32_t skillId, uint32_t ebx);
    void*    (*f0A00B0)(const uint32_t* blk);
    void     (*f0448E0)(const uint8_t* rec, void* unit, void* r, int32_t skillId, uint32_t ebx);
};

// 1.13c D2Game+0x0C6B0
void d2_fn_d2game_c6b0(uint32_t edx, int32_t skillId, uint32_t ebx, void* unit, const D2GameC6B0Env* e)
{
    uint32_t cnt, base;
    std::memcpy(&cnt, e->dataTables + 0xba0, 4);
    std::memcpy(&base, e->dataTables + 0xb98, 4);
    if (skillId < 0 || skillId >= (int32_t)cnt) return;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)base + (size_t)skillId * 0x23c;
    if (!rec) return;
    uint32_t name; std::memcpy(&name, rec + 0x60, 4);
    uint32_t r = e->o10786(unit, name, skillId, ebx);
    int16_t w; std::memcpy(&w, rec + 0x82, 2);
    uint32_t blk[9] = { (uint32_t)(uintptr_t)unit, edx, (uint32_t)skillId, ebx, r, 0xFFFFFFFFu, 0, (uint32_t)(int32_t)w, 0 };
    if (void* res = e->f0A00B0(blk)) e->f0448E0(rec, unit, res, skillId, ebx);
}
