// src/core/d2game/d2game_1bdf0.cpp -- 1.13c D2Game.dll sub_1BDF0 written from the disassembly.
// EAX = pUnit, one stack word pGame (callee pops it). Looks up the unit's tag-3 list node, creating it through the
// seam when missing, then fills the node's two coordinates and runs the class-selected check pair.
#include <cstdint>
#include <cstring>

struct D2Game1BDF0Env {
    void (*create)(uint32_t game, uint32_t unit, uint32_t local);         // 0x21290: EAX game, ECX unit, stack &{3,0,0,0,0}
    uint32_t (*getX)(uint32_t unit);                                       // 0x1100: ECX unit
    uint32_t (*getY)(uint32_t unit);                                       // 0x1130: ECX unit
    uint32_t (*checkA)(uint32_t game, uint32_t unit);                      // 0x60400 / 0x56a00: ECX game, EDX unit
    void (*applyA)(uint32_t game, uint32_t unit);                          // 0x663a0 / 0x5cc30: ECX game, EDX unit
    uint32_t (*checkB)(uint32_t game, uint32_t unit);
    void (*applyB)(uint32_t game, uint32_t unit);
    void (*ord)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c);        // import 0xA472, 4 stack words
    void (*notify)(uint32_t unit, uint32_t game, uint32_t x, uint32_t y);  // 0xF21E0: EAX 0xc, EDI unit, stack game, x, y
    void (*done)(uint32_t game, uint32_t unit, uint32_t one);              // 0xF22C0: EAX game, ESI unit, stack 1
};

static inline uint32_t rd32_1bdf0(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr32_1bdf0(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }

static uint32_t node3_1bdf0(uint32_t unit)
{
    if (!unit || rd32_1bdf0(unit, 0) != 1) return 0;
    const uint32_t pd = rd32_1bdf0(unit, 0x14); if (!pd) return 0;
    const uint32_t ai = rd32_1bdf0(pd, 0x28); if (!ai) return 0;
    const uint32_t head = rd32_1bdf0(ai, 0x20); if (!head) return 0;
    uint32_t n = rd32_1bdf0(head, 0);
    while (rd32_1bdf0(n, 8) != 3 && n != head) n = rd32_1bdf0(n, 0);
    return rd32_1bdf0(n, 8) == 3 ? n : 0;
}

// 1.13c D2Game+0x1BDF0
extern "C" uint32_t d2game_1bdf0_port(uint32_t unit, uint32_t game, const D2Game1BDF0Env* e)
{
    if (!node3_1bdf0(unit)) e->create(game, unit, 0xB3000000u);
    const uint32_t node = node3_1bdf0(unit);
    if (!node || rd32_1bdf0(node, 0xc) || rd32_1bdf0(node, 0x10)) return 0;
    wr32_1bdf0(node, 0xc, e->getX(unit) + 3);
    wr32_1bdf0(node, 0x10, e->getY(unit) + 3);
    const bool b = rd32_1bdf0(unit, 4) == 0x20f;
    if ((b ? e->checkB : e->checkA)(game, unit) == 0) {
        (b ? e->applyB : e->applyA)(game, unit);
        e->ord(unit, 6, 0, 0);
        const uint32_t y = e->getY(unit);
        const uint32_t x = e->getX(unit);
        e->notify(unit, game, x, y);
    }
    wr32_1bdf0(node, 0x14, 1);
    wr32_1bdf0(node, 0x18, 0);
    e->done(game, unit, 1);
    return 1;
}
