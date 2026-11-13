// Extra callee seams of the 1.13c D2Game AI think ports 0x83550, 0x843F0, 0x84800, 0x851E0, 0x874A0 (the callees D2Game77dEnv lacks),
// and the helpers those ports share: the unit seed roll, the monster-data pointer and the command post. Each port takes a
// D2Game77dEnv (the shared callees) and a D2GameAiExt (these); the FN probes bind both to HookEnv recorders.
#pragma once
#include <cstdint>
#include <cstring>
#include "d2game_77d_env.h"

struct D2GameAiExt {
    // 0xF3000(ecx = target, edx = unit, game, 7), ret 8.
    void     (*subF3000)(uint32_t target, uint32_t unit, uint32_t game, uint32_t seven);
    // Import thunks (stdcall): 0x2A514(unit) = Ordinal_10830; 0x2ACBE(classId, 0, 0) = Ordinal_10035.
    uint32_t (*th2A514)(uint32_t unit);
    uint32_t (*th2ACBE)(uint32_t classId, uint32_t z0, uint32_t z1);
    // 0x2A83E(unit, id, 0) = Ordinal_10689 (stdcall).
    void     (*th2A83E)(uint32_t unit, uint32_t id, uint32_t zero);
    // 0x1D30(ecx = index, ebx = value): plain ret; a data-table lookup.
    uint32_t (*sub1D30)(uint32_t index, uint32_t value);
    // 0x7C6E0(eax = target, ecx = unit, game, MonStats row), ret 8.
    uint32_t (*sub7C6E0)(uint32_t target, uint32_t unit, uint32_t game, uint32_t row);
    // 0xB1BC0(eax = unit, ecx = game, ebx = out record or 0, stack &dist), ret 4: a target unit or 0; writes the distance.
    uint32_t (*subB1BC0)(uint32_t unit, uint32_t game, uint32_t ebx, uint32_t pDist);
    // The value of the import slot D2Game+0xF829C: the address of D2Common's sgptDataTables pointer.
    uint32_t (*dataTablesSlot)(void);
    // The fatal exit (Ordinal_10265(0x110), Ordinal_10024, _exit(-1)) of an out-of-range monster-data value.
    void     (*fatal)(void);
};

namespace aiext {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
inline int32_t rdS16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 2); return v; }
inline uint8_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
inline void wr8(uint32_t a, uint32_t o, uint8_t v) { *(uint8_t*)(uintptr_t)(a + o) = v; }
// One step of the unit seed (unit+0x20 lo, +0x24 hi): s = lo * 0x6AC690C5 + hi; returns the new lo.
inline uint32_t step(uint32_t unit) {
    const uint64_t s = (uint64_t)rd32(unit, 0x20) * 0x6AC690C5ull + rd32(unit, 0x24);
    wr32(unit, 0x20, (uint32_t)s); wr32(unit, 0x24, (uint32_t)(s >> 32));
    return (uint32_t)s;
}
inline uint32_t roll100(uint32_t unit) { return step(unit) % 100u; }
// 0x1180(eax = n, ecx = seed): 0 when n < 1; a power of two masks, else the low dword mod n.
inline uint32_t roll(uint32_t unit, int32_t n) {
    if (n < 1) return 0;
    const uint32_t lo = step(unit);
    return (n & (n - 1)) ? lo % (uint32_t)n : lo & (uint32_t)(n - 1);
}
// The monster data of a unit: type 1 with unit+0x14 set -> *(unit+0x14)+0x2C, else 0.
inline uint32_t mdata(uint32_t unit) {
    if (unit == 0 || rd32(unit, 0) != 1) return 0;
    const uint32_t d = rd32(unit, 0x14);
    return d ? rd32(d, 0x2c) : 0;
}
// Ordinal_11103(unit, 0), then the 8-dword command {kind, unit, target, ..., byte 0x15 = 100 or 0x65} posted through 0x75E10(game, cmd, 1).
// 0x65 when the 0x1320 record's byte +0x104 meets the mask dword at gdwBitMasks + maskOff.
inline void postCmd(const D2Game77dEnv* e, uint32_t game, uint32_t unit, uint32_t target, uint32_t kind, uint32_t maskOff) {
    e->ordinal11103(unit, 0);
    uint8_t cmd[0x20];
    std::memset(cmd, 0, sizeof cmd);
    std::memcpy(cmd, &kind, 4); std::memcpy(cmd + 4, &unit, 4);
    uint8_t bl = 100;
    if (unit != 0) {
        const uint32_t rec = e->sub1320(rd32(unit, 4));
        if (rec != 0 && (rd8(rec, 0x104) & rd32(e->globalF8BE0(), maskOff)) != 0) bl = 0x65;
    }
    cmd[0x15] = bl;
    std::memcpy(cmd + 8, &target, 4);
    e->sub75E10(game, cmd, 1);
}
// The idle-wander tail shared by several think steps: one seed step picks 5 or 6 from its low byte; mdata+0x18 = it, byte +0x20 = mode.
inline void wander(uint32_t unit, uint8_t mode) {
    const uint32_t lo = step(unit);
    uint32_t v = (lo & 0xff) < 0x80 ? 5 : 6;
    if (v == 1) v = 7;
    const uint32_t m = mdata(unit);
    if (v != 0) wr32(m, 0x18, v);
    wr8(m, 0x20, mode);
}
}
