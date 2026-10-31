// src/core/d2game/d2game_be20.cpp -- 1.13c D2Game+0xBE20, the blood-mana charge (decompile build-wasm/ghidra-d2game/decomp/0xBE20.c; the register
// convention is from the disassembly). Registers: EAX = mana cost, ESI = pUnit; plain ret. State 0x72, stat 6 (hit points, 24.8 fixed point).
// Callees (seams, all stdcall import thunks): 0xA5D4 (unit, state) -> statlist, 0xAE62 (statlist) -> skill id, 0xA42A (unit, stat, layer) -> value,
// 0xA4D2 (unit, stat, delta, layer) add, 0xA472 (unit, stat, value, layer) set, 0xABB6 (unit, statlist), 0xABAA (statlist) free.
#include <cstdint>
#include <cstring>

struct D2GameBE20Env {
    const uint8_t* dataTables;                                         // *sgptDataTables: +0xB98 records (0x23C), +0xBA0 count
    void*    (*a5d4)(void* unit, int32_t state);
    int32_t  (*ae62)(void* statlist);
    int32_t  (*a42a)(void* unit, int32_t stat, int32_t layer);
    void     (*a4d2)(void* unit, int32_t stat, int32_t delta, int32_t layer);
    void     (*a472)(void* unit, int32_t stat, int32_t value, int32_t layer);
    void     (*abb6)(void* unit, void* statlist);
    void     (*abaa)(void* statlist);
};

// 1.13c D2Game+0x0BE20
int32_t d2_fn_d2game_be20(int32_t cost, void* unit, const D2GameBE20Env* e)
{
    int32_t id = e->ae62(e->a5d4(unit, 0x72));
    if (e->a42a(unit, 6, 0) < cost) {
        if (void* sl = e->a5d4(unit, 0x72)) { e->abb6(unit, sl); e->abaa(sl); }
        e->a472(unit, 6, 0x100, 0);
        return 0;
    }
    e->a4d2(unit, 6, -cost, 0);
    uint32_t cnt, base;
    std::memcpy(&cnt, e->dataTables + 0xba0, 4);
    std::memcpy(&base, e->dataTables + 0xb98, 4);
    const uint8_t* rec = (id >= 0 && id < (int32_t)cnt) ? (const uint8_t*)(uintptr_t)base + (size_t)id * 0x23c : nullptr;
    int32_t param; std::memcpy(&param, rec + 0x158, 4);
    if (e->a42a(unit, 6, 0) < param * 0x100) {
        if (void* sl = e->a5d4(unit, 0x72)) { e->abb6(unit, sl); e->abaa(sl); }
    }
    return 1;
}
