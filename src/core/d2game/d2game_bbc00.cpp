// D2Game+0xBBC00 -- seal check for the type-0x17 quest (called from the 0x36C50 dispatcher for level 0x6C).
// EBX = unit, EDI = game, no stack args, plain ret. Reads the unit's coordinates into a stack pair (#10141 through
// the 0xA544 thunk), finds the type-0x17 node in the game's quest list (game+0x10F4 -> head, next at +0xF4) and
// compares the pair against the three (x, y) slots of the node's record (+0x18) at +0x24, +0x2C, +0x34. On the
// first match it calls 0x36820 with ECX = 2, EAX = the matching data-table word (+0xB28 / +0xB2A / +0xB2C of
// *sgptDataTables), ESI = the matched slot, stack (game, unit); a null result arms 0x8EA80 (ECX game, EDX unit,
// stack 7, game+0xA8 + 10, 0, 0, 0). Callees are taken through an env.
#include <cstdint>
#include <cstring>

struct D2GameBBC00Env {
    void     (*coords)(uint32_t unit, uint32_t* xy);                                           // 0xA544, stdcall 2
    uint32_t (*f36820)(uint32_t ecx, uint32_t eax, uint32_t esi, uint32_t game, uint32_t unit); // ret 8
    void     (*f8EA80)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e); // ret 0x14
    uint32_t dataTables;                                                                        // *sgptDataTables
};

static inline uint32_t bbc00_rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t bbc00_rd16(uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }

// 1.13c D2Game+0xBBC00
extern "C" void D2Game_BBC00(uint32_t unit, uint32_t game, const D2GameBBC00Env* e)
{
    uint32_t xy[2];
    e->coords(unit, xy);
    const uint32_t list = bbc00_rd32(game + 0x10f4);
    if (!list) return;
    uint32_t n = bbc00_rd32(list);
    if (!n) return;
    while (bbc00_rd32(n) != 0x17) {
        n = bbc00_rd32(n + 0xf4);
        if (!n) return;
    }
    const uint32_t rec = bbc00_rd32(n + 0x18);
    uint32_t slot = 0, word = 0;
    for (uint32_t i = 0; i < 3; i++) {
        const uint32_t s = rec + 0x24 + 8 * i;
        if (xy[0] == bbc00_rd32(s) && xy[1] == bbc00_rd32(s + 4)) { slot = s; word = bbc00_rd16(e->dataTables + 0xb28 + 2 * i); break; }
    }
    if (!slot) return;
    if (e->f36820(2, word, slot, game, unit) == 0)
        e->f8EA80(game, unit, 7, bbc00_rd32(game + 0xa8) + 10, 0, 0, 0);
}
