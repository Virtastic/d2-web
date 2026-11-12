// 1.13c D2Client+0xC1A80: gate / dispatch of one input event index. Disassembly: EAX = mode (0 or 2 act, anything else returns 1), EBX = event index, plain ret.
// Walks the 0x26 slot table (0xFAD80) and dispatches each armed slot's action for the event through a five-way switch (jump table at 0xC1C20).
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <emscripten.h>

struct D2cC1a80Env {
    void (*fatal)(uint32_t code);                 // 0xBF92/0xBF80/0x3664: format + exit; never returns in retail
    uint32_t (*tick)();                           // GetTickCount (IAT 0xCEF5C)
    void (*f56210)(uint32_t player);              // ESI = player
    void (*c4a30)(uint32_t al);                   // EAX low byte
    void (*c4660)();
    void (*f7b20)(uint32_t edi);                  // EDI
    void (*f72790)(uint32_t idx, uint32_t edx);   // ECX = idx, EDX, one stack word 0 (ret 4)
};
// Module globals pinned in one block: count 0xDD5D0, 0xFAD50, 0x11BC08, 0xFADE0, player 0x11BBFC, cfg 0x11B980, 0x11C39C..0x11C3A4, 0xF8C9C, 0x113A68,
// 0x11971D, 0x119721, slot table 0xFAD80 (0x26 dwords), action-array pointers 0xDD538 (0x26 dwords).
struct D2cC1a80St {
    uint32_t count, lim, bc08, fade0, player, cfg, c39c, c3a0, c3a4, f8c9c, g3a68, g1971d, g19721, slot[0x26], act[0x26];
};
static_assert(sizeof(D2cC1a80St) == 0x164, "c1a80 state layout");
static_assert(offsetof(D2cC1a80St, slot) == 0x34 && offsetof(D2cC1a80St, act) == 0xcc && offsetof(D2cC1a80St, g1971d) == 0x2c, "c1a80 state layout");
static uint32_t c1_rd(uint32_t p, uint32_t o) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2c_c1a80(const D2cC1a80Env* e, D2cC1a80St* g, uint32_t mode, uint32_t idx) {
    if (g->count != 0x26) { e->fatal(0x139b); return 0; }
    if (mode != 0) {
        if (mode != 2 || g->slot[idx] != 0) return 1;
    }
    bool gate = true;   // false: fall to the common loop at 0xC1B6B
    if (idx == 5 || idx == 9) {
        // 0xFADE0 is the slot table's own entry 0x18 (0xFAD80 + 0x60), not a separate word: read it there.
        if (g->bc08 && idx == 5 && !g->slot[0x18]) return 0;
        if (g->lim > e->tick()) return 0;
        if (idx == 9) {
            const uint32_t pl = g->player;
            if (!pl) return 0;
            if (c1_rd(pl, 0) == 0 && c1_rd(pl, 0x10) == 0x11) {
                e->f56210(pl); e->c4a30(0x41);
                uint8_t* cfg = (uint8_t*)(uintptr_t)g->cfg;
                if (!(cfg[0x1eb] & 4)) return 0;
                e->c4660();
                cfg[0x1eb] |= 8;
                g->c3a0 = 0; g->c39c = 0; g->f8c9c = 0; g->c3a4 = 1;
                return 0;
            }
        }
        gate = false;
    }
    (void)gate;
    if (g->g3a68 != 0 && idx != 0xa && idx != 0x13 && idx != 0x11 && idx != 6 && idx != 7) return 0;
    for (uint32_t i = 0; (int32_t)i < (int32_t)g->count; ++i) {
        if (g->slot[i] != 1) continue;
        const uint32_t v = c1_rd(g->act[i], idx * 4);
        if (v > 4 || v == 3) { if (idx != 0) return 0; e->fatal(0x13ea); return 0; }
        if (v == 1) e->f72790(i, 1);
        else if (v == 2) return 0;
        else if (v == 4 && g->g19721 != 0) e->f7b20(g->g1971d);
    }
    return 1;
}
}
