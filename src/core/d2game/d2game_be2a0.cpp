// src/core/d2game/d2game_be2a0.cpp -- 1.13c D2Game+0xBE2A0: roll the extra vendor stock of one
// NPC visit. Written from the 1.13c disassembly (image base 0x6FC20000). Convention: EAX = list owner (its +8 is the head of the
// per-visit record list), ECX = the player unit, stack words game and the room/owner word handed to the item creator; ret 8.
// Callees are seams on Be2a0Env in retail argument order; the probe (src/core/fn/d2game_be2a0.cpp) records them.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
inline uint8_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
}

struct Be2a0Env {
    uint32_t (*vendorTable)();                                    // thunk 0xAC3A (no args)
    uint32_t (*vendorRecord)(uint32_t act);                       // thunk 0xA7BA (byte game+0x6D)
    uint32_t (*codeLookup)(uint32_t code, uint32_t* out);         // thunk 0xAA42 (#10450)
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);   // thunk 0xA2E6: ECX pool, EDX size, stack file, line, 0
    uint32_t (*poolWord)(uint32_t pool, uint32_t z);              // thunk 0xA448
    uint32_t (*unitStat)(uint32_t unit, uint32_t stat, uint32_t layer);   // thunk 0xA42A
    uint32_t (*itemRecord)(uint32_t id);                          // thunk 0xA53E (#10695)
    uint32_t (*classRoll)(uint32_t level, uint32_t game, uint32_t cls, uint32_t fa, uint32_t fb);   // 0xBD040: EAX, ECX, 3 words
    uint32_t (*createItem)(uint32_t level, uint32_t a, uint32_t b, const uint32_t* w8);   // 0x11980: EAX, ECX, EDX, 8 words
    void     (*t_a92e)(uint32_t item, uint32_t z);
    uint32_t (*t_abf2)(uint32_t item);
    uint32_t (*t_a9ac)(uint32_t item);
    uint32_t (*t_a940)(uint32_t item);
    uint32_t (*t_aa06)(uint32_t item);
    void     (*setStat)(uint32_t item, uint32_t stat, uint32_t v, uint32_t layer);   // thunk 0xA472
    void     (*f_d2380)(uint32_t game, uint32_t z, uint32_t item);  // EAX, ECX, 1 word
    uint32_t (*t_a8a4)(uint32_t item, uint32_t a, uint32_t line);   // thunk 0xA8A4: item, 0x100, 0xBA, file
    void     (*f_d5690)(uint32_t esi, uint32_t game);              // ESI, 1 word
    uint32_t (*t_aa00)(uint32_t item);
    void     (*t_a65e)(uint32_t item, uint32_t a, uint32_t b);
    uint32_t (*pathY)(uint32_t path);                              // thunk 0xA3B8
    uint32_t (*pathX)(uint32_t path);                              // thunk 0xA454
    uint32_t (*place)(uint32_t line, const uint32_t* w8);          // 0xD76A0: ECX file, EDX line, 8 words
    void     (*freeItem)(uint32_t game, uint32_t item);            // 0x0EE70: EAX game, ESI item
    uint32_t* datF7C;                                              // DAT_6fd31f7c ('uma ' class, cached)
    uint32_t* datF80;                                              // DAT_6fd31f80 ('irin' class, cached)
};

// 1.13c D2Game+0xBE2A0
void d2game_be2a0(const Be2a0Env* E, uint32_t owner, uint32_t player, uint32_t game, uint32_t roomArg) {
    uint32_t tbl = E->vendorTable();
    if (!tbl) return;
    uint32_t rec = E->vendorRecord(rd8(game, 0x6d));
    if (!rec) return;
    const int32_t chance = (int32_t)(rd32(rec, 0x44) + rd32(rec, 0x48) + rd32(rec, 0x4c));
    if (!*E->datF80) E->codeLookup(0x206e6972, E->datF80);
    if (!*E->datF7C) E->codeLookup(0x20756d61, E->datF7C);
    const uint32_t blk = E->alloc(rd32(game, 0x1c), 0xc, 0xd94);
    if (!blk) return;
    wr32(blk, 0, E->poolWord(rd32(game, 0x1c), 0));
    wr32(blk, 4, player ? rd32(player, 0xc) : 0xffffffffu);
    wr32(blk, 8, rd32(owner, 8));
    wr32(owner, 8, blk);
    const int32_t lvl = (int32_t)E->unitStat(player, 0xc, 0);
    const uint32_t seed = rd32(game, 0x1d24) + 8;
    auto step = [&]() -> uint32_t {   // seed {lo, hi}: (lo * 0x6AC690C5 + hi)
        uint64_t s = (uint64_t)rd32(seed, 0) * 0x6ac690c5ull + rd32(seed, 4);
        wr32(seed, 0, (uint32_t)s); wr32(seed, 4, (uint32_t)(s >> 32));
        return (uint32_t)s;
    };
    int32_t n = 0;
    do {
        int32_t idx = lvl + (int32_t)(step() % 10) - 5;
        if (idx <= 5) idx = 5; else if (idx >= 0x63) idx = 0x63;
        const int32_t cnt = (int32_t)rd32(tbl, 8 + idx * 4);
        uint32_t pick = 0;
        if (cnt > 0) {
            const uint32_t lo = step();
            pick = ((uint32_t)cnt & ((uint32_t)cnt - 1)) ? lo % (uint32_t)cnt : lo & ((uint32_t)cnt - 1);
        }
        uint32_t cls = rd32(rd32(tbl, 4), pick * 4);
        if (rd32(game, 0x70) == 0) {
            const uint32_t ir = E->itemRecord(cls);
            if (!ir) return;
            if (rd16(ir, 0xf6) >= 100) continue;
        }
        if (n < 2) cls = n ? *E->datF7C : *E->datF80;
        cls = E->classRoll((uint32_t)idx, game, cls, rd32(rec, 0x50), rd32(rec, 0x54));
        uint32_t q = 4;
        if (chance > 0) {
            const int32_t roll = (int32_t)(step() % 100000u);
            if (roll < chance) {
                const int32_t r4c = (int32_t)rd32(rec, 0x4c);
                q = roll < r4c ? 7 : 5 + (roll >= (int32_t)(rd32(rec, 0x48) + r4c) ? 1 : 0);
            }
        }
        ++n;
        const uint32_t cw[8] = { roomArg, cls, game, 4, q, 0, 0, 0 };
        const uint32_t item = E->createItem((uint32_t)idx, 1, 0, cw);
        if (!item) continue;
        E->t_a92e(item, 0);
        if (E->t_abf2(item)) {
            if (E->t_a9ac(item) && E->t_a940(item)) E->setStat(item, 0x46, E->t_aa06(item), 0);
            E->f_d2380(game, 0, item);
            if (E->t_a8a4(item, 0x100, 0xba)) E->f_d5690(0, game);
            else { const int32_t v = (int32_t)E->t_aa00(item); if (v > 0) E->setStat(item, 0x48, (uint32_t)v, 0); }
        }
        E->t_a65e(item, 0x10, 0);
        const int32_t t = (int32_t)rd32(item, 0);
        const uint32_t path = rd32(item, 0x2c);
        const bool direct = t == 2 || t == 4 || t == 5;
        const uint32_t y = direct ? rd32(path, 0x10) : path ? E->pathY(path) : 0;
        const uint32_t x = direct ? rd32(path, 0xc) : path ? E->pathX(path) : 0;
        const uint32_t pw[8] = { game, roomArg, rd32(item, 0xc), x, y, 1, 0, rd32(blk, 0) };
        if (!E->place(0xde4, pw)) { E->freeItem(game, item); return; }
    } while (n < 0xe);
}
