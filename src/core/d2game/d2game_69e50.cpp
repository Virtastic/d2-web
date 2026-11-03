// 1.13c D2Game+0x69E50 -- roll a table action for a monster.
// CL = table index, EDI = monster, [esp+4] = value handed to the handler's ECX (ret 4). Table (RVA 0x107D50, 12-byte entries:
// handler, chance, arg), count at RVA 0x107E4C. An index at or past the count (signed compare) returns 0. Otherwise the monster's
// seed (+0x20 lo, +0x24 hi) advances to lo*0x6AC690C5 + hi (64-bit); roll = new lo % 100; roll < chance runs
// handler(ECX = stack value, EDX = monster, stack = arg) and returns its EAX, else 0. A bad handler pointer is fatal (line 0x10F5).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game69E50Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*isBadCodePtr)(uint32_t fn);
    uint32_t (*call)(uint32_t fn, uint32_t ecx, uint32_t edx, uint32_t arg);
};

static inline uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void w32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

// 1.13c D2Game+0x69E50
uint32_t D2Game_69E50(uint32_t ecx, uint32_t mon, uint32_t stackVal, uint32_t tbl, int32_t count, const D2Game69E50Env& e)
{
    const uint32_t idx = ecx & 0xff;
    if ((int32_t)idx >= count) return 0;
    const uint64_t s = (uint64_t)r32(mon + 0x20) * 0x6ac690c5ull + r32(mon + 0x24);
    w32(mon + 0x24, (uint32_t)(s >> 32));
    w32(mon + 0x20, (uint32_t)s);
    const uint32_t ent = tbl + idx * 12;
    if ((uint32_t)s % 100 >= r32(ent + 4)) return 0;
    const uint32_t fn = r32(ent);
    if (e.isBadCodePtr(fn)) { e.fatal(0x10f5); return 0; }
    return e.call(fn, stackVal, mon, r32(ent + 8));
}
