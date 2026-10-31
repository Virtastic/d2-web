// src/core/d2game/d2game_batch_ca20.cpp -- 1.13c D2Game+0xCA20 (decompile build-wasm/ghidra-d2game/decomp/0xCA20.c; the register
// convention and the block layout are from the disassembly). Registers: ECX = pGame, EDX = pUnit; [esp+4] = skillId, [esp+8] = level; ret 8.
// The nine-dword stack block handed to 0x9EEB0 (ESI = block, [esp] = pGame, ret 4) is
//   {0x3002, unit, 0x11D, X1, X2, value, level, 0, module+0xC8D0}
// where X1/X2 come from the two import thunks 0xA454 / 0xA3B8 (stdcall 1, argument unit+0x2C) or, for unit types 2 and 4..5, from unit+0x2C -> +0xC / +0x10.
// Callees (all seams): 0xA454, 0xA3B8, 0x9EEB0.
#include <cstdint>
#include <cstring>

struct D2GameCA20Env {
    const uint8_t* dataTables;                               // *sgptDataTables: +0xB98 records (0x23C), +0xBA0 count
    uint32_t (*thunkA454)(uint32_t p);
    uint32_t (*thunkA3B8)(uint32_t p);
    void     (*func9EEB0)(const uint32_t* blk, void* game);
    uint32_t moduleC8D0;                                     // module base + 0xC8D0 (callback address stored in the block)
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// 1.13c D2Game+0xCA20
uint32_t d2_game_ca20(void* pGame, const uint8_t* pUnit, int32_t skillId, int32_t level, const D2GameCA20Env* env)
{
    const int32_t type = (int32_t)rd32(pUnit, 0);
    const uint32_t sub = rd32(pUnit, 0x2C);
    const bool direct = type == 2 || (type > 3 && type <= 5);
    uint32_t x1, x2;
    if (direct) x1 = rd32((const void*)(uintptr_t)sub, 0xC);
    else x1 = sub ? env->thunkA454(sub) : 0;
    if (direct) x2 = rd32((const void*)(uintptr_t)sub, 0x10);
    else x2 = sub ? env->thunkA3B8(sub) : 0;
    uint32_t v = 0;
    if (skillId >= 0 && skillId < (int32_t)rd32(env->dataTables, 0xBA0)) {
        const uint8_t* rec = (const uint8_t*)(uintptr_t)rd32(env->dataTables, 0xB98) + (size_t)skillId * 0x23C;
        if (rec && level > 0) v = (uint32_t)(level - 1) * rd32(rec, 0x15C) + rd32(rec, 0x158);
    }
    uint32_t blk[9] = { 0x3002, (uint32_t)(uintptr_t)pUnit, 0x11D, x1, x2, v, (uint32_t)level, 0, env->moduleC8D0 };
    env->func9EEB0(blk, pGame);
    return 1;
}
