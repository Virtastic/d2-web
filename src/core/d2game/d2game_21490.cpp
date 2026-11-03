// D2Game+0x21490 (stdcall ret 8, EAX = unit, EBX = callback, two stack words p1 (unread) and p2): the per-key callback walk.
// Disassembled order: IsBadCodePtr(EBX) (bad = fatal 0x24E); unit null / type != 1 / +0x14 null / +0x28 null / rec[0x28] zero returns;
// sought = 0xE03A0(ecx = rec[0x28], edx = rec[0x2C], eax = rec[0x30]); null or type != 1 returns; inner = sought[0x14][0x28] (a null
// +0x14 gives a null inner, which retail dereferences); base = inner[0x28]; the walk list head is inner[0x34] (NOT base[0x34]).
// Per node (next = node[4], key = node[0]): a null base is fatal 0x2D3; the bucket base[0x1320 + (key & 0x7F) * 4] chain (next at
// +0xE4) is searched for entry[0xC] == key; the first match calls EBX(p2) once; the walk then continues with the saved next.
// 1.10f->1.13c delta: the 0xE03A0 lookup takes the third key rec[0x30] in EAX, and the list head lives on the inner record.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game21490Env {
    int32_t  (*isBad)(uint32_t fn);
    uint32_t (*subE03A0)(uint32_t ecx, uint32_t edx, uint32_t eax);
    void     (*callback)(uint32_t fn, uint32_t p2);
    void     (*fatal)(uint32_t line);
};

static inline uint32_t g21490(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x21490
extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_21490(uint32_t unit, uint32_t cb, uint32_t p2, const D2Game21490Env* e)
{
    if (e->isBad(cb)) e->fatal(0x24E);
    if (!unit || g21490(unit, 0) != 1) return;
    const uint32_t u14 = g21490(unit, 0x14);
    if (!u14) return;
    const uint32_t rec = g21490(u14, 0x28);
    if (!rec || !g21490(rec, 0x28)) return;
    const uint32_t sought = e->subE03A0(g21490(rec, 0x28), g21490(rec, 0x2C), g21490(rec, 0x30));
    if (!sought || g21490(sought, 0) != 1) return;
    const uint32_t s14 = g21490(sought, 0x14);
    if (!s14) return;                          // retail dereferences a null inner record
    const uint32_t inner = g21490(s14, 0x28);
    if (!inner) return;                        // retail dereferences it
    const uint32_t base = g21490(inner, 0x28);
    uint32_t node = g21490(inner, 0x34);
    while (node) {
        const uint32_t next = g21490(node, 4);
        const uint32_t key = g21490(node, 0);
        if (!base) e->fatal(0x2D3);
        for (uint32_t en = g21490(base, 0x1320 + (key & 0x7F) * 4); en; en = g21490(en, 0xE4))
            if (g21490(en, 0xC) == key) { e->callback(cb, p2); break; }
        node = next;
    }
}
