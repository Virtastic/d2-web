// 1.13c D2Game+0x83CE0: Brute AI tick. ECX = game, EDX = unit, stack (ret 4) = param (+8 target word, +0x18 combat flag, +0x1C monstats row).
// 1.13c differs from the D2MOO 1.10f AITHINK_Fn007_Brute: two LCG rolls against the monstats shorts at +0x62 / +0x68 (indexed by game+0x6D), 75E10 with a 0x20-byte request,
// F2540 mode changes. Callees are seams (thunk RVAs): A496 #11103, 1320 (EAX = class, returns the monstats row), 75E10 (game, request, 1), F2540 (ECX game, EDX unit, 6 stack words),
// F22C0 (EAX game, ESI unit, stack v), A718 #11027 (life percent). `g` is the value of the module global at D2Game+0xF8BE0 (a struct whose +0x10 / +0x14 masks test row+0x104).
// The fatal-exit path (clamped percent out of range) cannot fire and is not modelled; a unit without a monster block is not modelled either (retail writes through null).
#include <cstdint>
#include <cstring>
struct D2G83CE0Seams {
    void     (*fA496)(uint32_t unit, uint32_t z);
    uint32_t (*f1320)(uint32_t cls);
    void     (*f75E10)(uint32_t game, const uint32_t* req, uint32_t one);
    void     (*fF2540)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
    void     (*fF22C0)(uint32_t game, uint32_t unit, uint32_t v);
    int32_t  (*fA718)(uint32_t unit);
};
static uint32_t r32(uint32_t p, uint32_t o = 0) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static void w32(uint32_t p, uint32_t o, uint32_t v) { memcpy((void*)(uintptr_t)(p + o), &v, 4); }
static int32_t s16(uint32_t p, uint32_t o) { int16_t v; memcpy(&v, (const void*)(uintptr_t)(p + o), 2); return v; }
static uint32_t roll(uint32_t unit) {   // seed (lo, hi) at unit+0x20: x = lo * 0x6AC690C5 + hi; returns lo of x
    const uint64_t x = (uint64_t)r32(unit, 0x20) * 0x6AC690C5u + r32(unit, 0x24);
    w32(unit, 0x20, (uint32_t)x); w32(unit, 0x24, (uint32_t)(x >> 32));
    return (uint32_t)x;
}
static uint32_t block(uint32_t unit) {  // unit+0x14 +0x2C, or 0
    if (unit != 0 && r32(unit, 0) == 1 && r32(unit, 0x14) != 0) return r32(r32(unit, 0x14), 0x2c);
    return 0;
}
extern "C" void d2game_83ce0(uint32_t game, uint32_t unit, uint32_t param, uint32_t g, const D2G83CE0Seams* s)
{
    const uint32_t ms = r32(param, 0x1c), u4 = r32(param, 8);
    const uint32_t diff = *(const uint8_t*)(uintptr_t)(game + 0x6d);
    if (r32(param, 0x18) == 0) {
        int32_t pct = s->fA718(unit);
        if (pct < 0x28) pct = 0x28; else if (pct > 100) pct = 100;
        const int32_t v = 100 - pct;
        if (v != 0) w32(block(unit), 0x1c, (uint32_t)v);
        w32(block(unit), 0x18, 0xd);
        s->fF2540(game, unit, u4, 2, 0, 0, 1, 7);
        return;
    }
    const int32_t a = s16(ms, 0x62 + diff * 2);
    if ((int32_t)(roll(unit) % 100) < a) {
        const uint32_t type = (int32_t)(roll(unit) % 100) < s16(ms, 0x68 + diff * 2) ? 4 : 5;
        s->fA496(unit, 0);
        uint32_t req[8] = { type, unit, u4, 0, 0, 0, 0, 0 };
        uint32_t bl = 0x64;
        const uint32_t row = s->f1320(r32(unit, 4));
        if (row != 0 && (*(const uint8_t*)(uintptr_t)(row + 0x104) & r32(g, type == 4 ? 0x10 : 0x14)) != 0) bl = 0x65;
        ((uint8_t*)req)[0x15] = (uint8_t)bl;
        s->f75E10(game, req, 1);
        return;
    }
    if ((int32_t)(roll(unit) % 100) >= a) { s->fF22C0(game, unit, 0xf); return; }
    const uint32_t mode = (roll(unit) & 0xff) < 0x80 ? 5 : 6;
    const uint32_t b = block(unit);
    w32(b, 0x18, mode);
    *(uint8_t*)(uintptr_t)(b + 0x20) = 4;
    s->fF2540(game, unit, u4, 2, 0, 0, 1, 0);
}
