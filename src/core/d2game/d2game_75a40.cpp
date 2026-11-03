// 1.13c D2Game+0x75A40 -- a monster of class-of-class 0x3F respawns its children around its owner's position.
// EDI = unit, stack = game, tag (ret 8). Unit type 1 and a monster index inside the table: the record's short at +2 (itself a
// monster index) must be 0x3F. Then 0xE07A0 (EAX = unit, ECX = game), the owner via 0xA598(unit+0x2c); the owner's position is
// 0xAAC6/0xAAD8 of the id when the owner is the unit or absent, else the path words +0xC/+0x10 for owner types 2, 4, 5 or
// 0xA454/0xA3B8 of its path. The link is cleared (0xA5FE(id, 0)), the seed is built (0xA616(&seed), 0xA628(&seed, "SIES"),
// hi = unit[3]) and the per-difficulty count (record +0x62 + game[0x6D]*2) of children is spawned through 0xA28F0 at +-5 jitter
// (two LCG steps by 0x6AC690C5 per child) via 0xAB2C(id, x, y); the link is then restored. No 1.10f counterpart.
#include <cstdint>
#include <cstring>

struct D2Game75A40Env {
    void (*prep)(uint32_t unit, uint32_t game);                 // 0xE07A0 (EAX = unit, ECX = game)
    uint32_t (*owner)(uint32_t id);                             // 0xA598 (stdcall 1)
    uint32_t (*idX)(uint32_t id);                               // 0xAAC6
    uint32_t (*idY)(uint32_t id);                               // 0xAAD8
    uint32_t (*pathX)(uint32_t path);                           // 0xA454
    uint32_t (*pathY)(uint32_t path);                           // 0xA3B8
    void (*setLink)(uint32_t id, uint32_t v);                   // 0xA5FE (stdcall 2)
    void (*seedInit)(uint32_t* seed);                           // 0xA616 (ECX = seed)
    void (*seedMix)(uint32_t* seed, uint32_t v);                // 0xA628 (ECX = seed, EDX = v)
    void (*place)(uint32_t id, int32_t x, int32_t y);           // 0xAB2C (stdcall 3)
    void (*spawn)(uint32_t game, uint32_t tag, uint32_t unit);  // 0xA28F0 (ECX = game, EDX = tag; unit, 0, 1, 0 x5)
};

static inline uint32_t r32(uint32_t a, uint32_t o)
{
    uint32_t v;
    std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a) + o), 4);
    return v;
}
static inline int32_t r16s(uint32_t a, uint32_t o)
{
    int16_t v;
    std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a) + o), 2);
    return v;
}

// dt = *sgptDataTables: +0xA78 monster records (0x1A8 each), +0xA80 count.
static uint32_t monRec(uint32_t dt, int32_t i)
{
    if (i < 0 || i >= static_cast<int32_t>(r32(dt, 0xA80))) return 0;
    return r32(dt, 0xA78) + static_cast<uint32_t>(i) * 0x1A8;
}

// 1.13c D2Game+0x75A40
void D2Game_75A40(uint32_t unit, uint32_t game, uint32_t tag, uint32_t dt, const D2Game75A40Env& e)
{
    if (unit == 0 || r32(unit, 0) != 1) return;
    int32_t cls = static_cast<int32_t>(r32(unit, 4));
    if (cls < 0) return;
    if (cls < static_cast<int32_t>(r32(dt, 0xA80))) {
        const uint32_t rec = monRec(dt, cls);
        if (rec != 0) cls = r16s(rec, 2);
    }
    if (cls < 0 || cls >= static_cast<int32_t>(r32(dt, 0xA80)) || cls != 0x3F) return;

    const uint32_t id = r32(unit, 0x2C);
    e.prep(unit, game);
    uint32_t own = e.owner(r32(unit, 0x2C));
    uint32_t x, y;
    if (own == unit || own == 0) {
        own = 0;
        x = e.idX(id);
        y = e.idY(id);
    } else {
        const uint32_t t = r32(own, 0), p = r32(own, 0x2C);
        const bool direct = t == 2 || (t > 3 && t < 6);
        x = direct ? r32(p, 0xC) : (p ? e.pathX(p) : 0);
        y = direct ? r32(p, 0x10) : (p ? e.pathY(p) : 0);
    }
    int32_t sx = 5, sy = 5;
    e.setLink(id, 0);
    uint32_t seed[2];
    e.seedInit(seed);
    e.seedMix(seed, 0x53454953u);
    seed[1] = r32(unit, 0xC);
    const uint32_t rec = monRec(dt, static_cast<int32_t>(r32(unit, 4)));
    uint8_t diff;
    std::memcpy(&diff, reinterpret_cast<const void*>(static_cast<uintptr_t>(game) + 0x6D), 1);
    int32_t n = rec ? r16s(rec, 0x62 + diff * 2u) : r16s(0, 0x62 + diff * 2u);
    for (; n > 0; n--) {
        uint64_t s = static_cast<uint64_t>(seed[0]) * 0x6AC690C5ull + seed[1];
        if (s & 1) sx = -sx;
        s = (s & 0xFFFFFFFFull) * 0x6AC690C5ull + (s >> 32);
        seed[0] = static_cast<uint32_t>(s);
        seed[1] = static_cast<uint32_t>(s >> 32);
        if (s & 1) sy = -sy;
        e.place(id, sx + static_cast<int32_t>(x), sy + static_cast<int32_t>(y));
        e.spawn(game, tag, unit);
    }
    e.setLink(id, own);
}
