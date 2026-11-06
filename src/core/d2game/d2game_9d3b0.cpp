// 1.13c D2Game+0x9D3B0 -- sub_9D3B0, rebuilt from the retail disassembly (fastcall ECX = game, EDX = unit, two unused stack words, ret 8).
// Builds the nine-dword context {[0] = unit.pData+0x28 (AI data, unit type 1 only), [7] = monstats row (stride 0x1A8, table +0xA78/+0xA80),
// [8] = second row (stride 0x134, table +0xA90/+0xA98, indexed by the short at row+0x18)} and, when both rows exist and none of the three
// vetoes fires, calls the handler at [ctx[0] + 4] (IsBadCodePtr first: a bad pointer is the fatal path). Every callee is an Env function.
// Addresses are 32-bit words (the engine's own pointers are 32-bit), so the port reads the retail structs by byte offset.
#include <cstdint>
#include <cstring>

struct D2G9D3B0Env {
    uint32_t gTables;   // module global at D2Game+0xF829C: the address of the word holding the data-table block pointer
    uint32_t (*s9cd90)(uint32_t unit, uint32_t game, uint32_t ctx);          // EAX unit, ECX game, EBX ctx; test eax
    uint32_t (*s9d160)(uint32_t unit, uint32_t ctx, uint32_t game);          // EBX unit, ESI ctx, stack game; test eax
    uint32_t (*s9cfe0)(uint32_t ctx, uint32_t unit, uint32_t game);          // EAX ctx, ECX unit, stack game; test eax
    bool (*isBadCodePtr)(uint32_t fn);
    void (*fatal)();                                                        // Fog #10265/#10024 then _exit(-1)
    void (*handler)(uint32_t fn, uint32_t game, uint32_t unit, uint32_t ctx);   // ECX game, EDX unit, stack ctx
};
static inline uint32_t d9rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
// 1.13c D2Game+0x9D3B0
extern "C" void d2g_9d3b0(const D2G9D3B0Env* ep, uint32_t game, uint32_t unit) {
    const D2G9D3B0Env& e = *ep;
    uint32_t ctx[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (unit && d9rd(unit) == 1 && d9rd(unit + 0x14)) ctx[0] = d9rd(d9rd(unit + 0x14) + 0x28);
    const uint32_t dt = d9rd(e.gTables);
    int32_t idx = unit ? (int32_t)d9rd(unit + 4) : -1;
    if (unit && idx >= 0 && idx < (int32_t)d9rd(dt + 0xA80)) ctx[7] = (uint32_t)idx * 0x1A8u + d9rd(dt + 0xA78);
    if (unit && idx >= 0 && idx < (int32_t)d9rd(dt + 0xA80)) {
        int16_t s; std::memcpy(&s, (const void*)(uintptr_t)(d9rd(dt + 0xA78) + (uint32_t)idx * 0x1A8u + 0x18), 2);
        if (s >= 0 && s < (int32_t)d9rd(dt + 0xA98)) ctx[8] = (uint32_t)s * 0x134u + d9rd(dt + 0xA90);
    }
    if (!ctx[7] || !ctx[8]) return;
    const uint32_t c = (uint32_t)(uintptr_t)ctx;
    if (e.s9cd90(unit, game, c)) return;
    if (e.s9d160(unit, c, game)) return;
    if (e.s9cfe0(c, unit, game)) return;
    const uint32_t fn = d9rd(ctx[0] + 4);
    if (e.isBadCodePtr(fn)) { e.fatal(); return; }
    e.handler(fn, game, unit, c);
}
