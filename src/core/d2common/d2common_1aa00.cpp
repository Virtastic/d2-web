// 1.13c D2Common+0x1AA00 (retail-layout, 32-bit image): where and as what a monster-spawning unit (a sarcophagus, an egg sac, a hole ...) puts its spawn.
// stdcall(unit, pId, pX, pY, pMode, param6, callback), ret 0x1C. unit: type +0 (must be 1), class id +4, data +0x2C, +0x20 (an area the 0x10B0 helper reads), +0x5C, flags +0xC8.
// The monsters table is sgptDataTables (global 0x99E1C) +0xA78 (rows, stride 0x1A8; the base id is the s16 at +2) with count +0xA80; the spawn id candidates are
// "K when K < count else -1". The class id picks the case (by base id): 0xCE, 0x11C, 0x12A use 0x53850 to look up a chain id (EBX of the id helper 0x1930), 0xE4 asks
// 0x2FE10 (room) / 0x1A770, 0x10B and 0x12A read the u16 pair of unit+0x2C +0x10/+0x12, 0x141 depends on class 0x2C7, 0x14E, 0x1E4, 0x219, 0x220 and 0x20E/0x210
// (callback, null is fatal) use the chain byte of the monstats row at global 0xA0950 [class] +0x4B. 0x219 also marks bit masks in unit+0x5C when the table
// has more than 0x92 entries (+0xC4) and calls 0x665C0. Anything else answers id 0 (or -1 without rows), position 0,0, mode 1.
#include <cstdint>
#include <cstdlib>
struct D2Common1AA00Env {
    uint32_t dt;                 // sgptDataTables
    uint32_t mons;               // global 0xA0950: rows pointer, count at +8
    uint32_t bits;               // global 0x890B0: bit-mask table
    uint32_t (*pick)(uint32_t id, uint32_t chain);                   // 0x1930 (ECX = id, EBX = chain)
    uint32_t (*unitX)(uint32_t unit);                                // 0x1050 (ECX = unit)
    uint32_t (*unitY)(uint32_t unit);                                // 0x1080
    uint32_t (*roll)(uint32_t k, uint32_t area);                     // 0x10B0 (EAX = k, ECX = area)
    void (*chainInfo)(uint32_t id, uint32_t* chain);                 // 0x53850 (id, 0, &chain)
    uint32_t (*room)(uint32_t unit);                                 // 0x7FE10
    uint32_t (*roomPick)(uint32_t room, uint32_t id);                // 0x1A770
    void (*after)(uint32_t unit);                                    // 0x665C0
};
extern "C" void D2Common_1AA00_SpawnInfoRaw(const D2Common1AA00Env* e, uint32_t unit, uint32_t pId, uint32_t pX, uint32_t pY, uint32_t pMode,
                                            uint32_t param6, uint32_t cb, uint32_t (*callCb)(uint32_t cb, uint32_t unit))
{
    auto W = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
    auto S = [](uint32_t a) { return static_cast<int32_t>(*reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a))); };
    auto H = [](uint32_t a) { return static_cast<uint32_t>(*reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(a))); };
    const int32_t count = S(e->dt + 0xA80);
    auto sel = [&](int32_t k) { return static_cast<uint32_t>(count > k ? k : -1); };
    int32_t base = -1;
    if (unit && W(unit) == 1) {
        base = S(unit + 4);
        if (base >= 0 && base < count) {
            const uint32_t row = static_cast<uint32_t>(base) * 0x1A8u + W(e->dt + 0xA78);
            if (row) base = static_cast<int16_t>(*reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(row + 2)));
        }
        if (base < 0 || base >= count) base = -1;
    }
    auto chainByte = [&]() -> uint32_t {   // the monstats row byte +0x4B of the unit's class id
        const int32_t id = S(unit + 4);
        if (id >= 0 && id < S(e->mons + 8)) return *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(id) * 0x1A8u + W(e->mons) + 0x4B));
        return 0;
    };
    auto done = [&](uint32_t id, uint32_t x, uint32_t y, uint32_t mode) { W(pId) = id; W(pX) = x; W(pY) = y; W(pMode) = mode; };
    auto viaChainInfo = [&](int32_t k) {   // 0xCE, 0x11C, 0x12A
        uint32_t chain = 0;
        e->chainInfo(unit ? W(unit + 4) : 0xFFFFFFFFu, &chain);
        W(pId) = e->pick(sel(k), chain);
    };
    if (base > 0x14E) {
        switch (base) {
        case 0x1E4: { const uint32_t c = chainByte(); W(pId) = e->pick(sel(0x1C5), c); W(pX) = e->unitX(unit); W(pY) = e->unitY(unit) + 3; W(pMode) = 1; return; }
        case 0x20E: case 0x210:
            if (!cb) abort();
            W(pId) = callCb(cb, unit); W(pX) = e->unitX(unit) + 2; W(pY) = e->unitY(unit) + 2; W(pMode) = 1; return;
        case 0x219: {
            const uint32_t c = chainByte();
            W(pId) = e->pick(sel(0x21C), c); W(pX) = e->unitX(unit); W(pY) = e->unitY(unit) + 2; W(pMode) = 1;
            if (S(e->dt + 0xC4) <= 0x92) return;
            const uint32_t q = W(unit + 0x5C);
            if (q && S(q + 0x10) < 0) {
                const uint32_t m = W(e->bits + 0x48), r = W(q + 0x58), v = W(r + 0x10);
                W(r + 0x10) = m | v;
                if ((m & v) == 0) {
                    int32_t t = S(e->dt + 0xC4) + 0x1F;
                    t = (t + ((t >> 31) & 0x1F)) >> 5;
                    W(W(q + 0x58) + 0x10 + static_cast<uint32_t>(t) * 4) |= W(e->bits + 0x48);
                    if (S(e->dt + 0xC4) > 0x92) {
                        const uint32_t a = W(e->dt + 0xBC) + 0x2238;
                        if (a && (*reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(e->bits)) & *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a + 0x12)))) W(unit + 0xC8) |= 8;
                    }
                }
            }
            e->after(unit);
            return;
        }
        case 0x220: {
            const uint32_t area = unit + 0x20;
            if (W(pId) == sel(0x23A)) {
                const uint32_t a = e->roll(0x18, area); const uint32_t x = a + e->unitX(unit) - 0xC; W(pX) = x;
                const uint32_t b = e->roll(0x18, area); const uint32_t y = b + e->unitY(unit) - 0xC; W(pY) = y; W(pMode) = 1; return;
            }
            const uint32_t k = sel(0x232);
            const uint32_t chain = e->roll(2, area) + param6;
            W(pId) = e->pick(k, chain);
            const uint32_t a = e->roll(0x18, area); W(pX) = a + e->unitX(unit) - 0xC;
            const uint32_t b = e->roll(0x18, area); W(pY) = b + e->unitY(unit) - 0xC; W(pMode) = 4; return;
        }
        default: break;
        }
    } else if (base == 0x14E) {
        const uint32_t c = chainByte();
        W(pId) = e->pick(sel(0x72), c);
        const uint32_t t = W(unit), d = W(unit + 0x2C);
        const bool a = t == 2 || (static_cast<int32_t>(t) > 3 && static_cast<int32_t>(t) <= 5);
        W(pX) = (a ? W(d + 0xC) : (d ? H(d + 2) : 0)) - 2;
        W(pY) = (a ? W(d + 0x10) : (d ? H(d + 6) : 0)) - 2;
        W(pMode) = 1; return;
    }
    switch (base) {
    case 0xCE: viaChainInfo(0xF); W(pX) = e->unitX(unit); W(pY) = e->unitY(unit) + 3; W(pMode) = 1; return;
    case 0xE4: { const uint32_t k = sel(0x60); W(pId) = e->roomPick(e->room(unit), k); W(pX) = e->unitX(unit); W(pY) = e->unitY(unit) + 2; W(pMode) = 1; return; }
    case 0x10B: { const uint32_t d = W(unit + 0x2C); done(sel(6), H(d + 0x10), H(d + 0x12), 8); return; }
    case 0x11C: viaChainInfo(0x44); W(pX) = e->unitX(unit) + 8; W(pY) = e->unitY(unit); W(pMode) = 8; return;
    case 0x12A: { viaChainInfo(0x12D); const uint32_t d = W(unit + 0x2C); W(pX) = H(d + 0x10); W(pY) = H(d + 0x12); W(pMode) = 1; return; }
    case 0x141: W(pId) = (unit && W(unit + 4) == 0x2C7) ? sel(0x2C8) : sel(0x13); W(pX) = e->unitX(unit); W(pY) = e->unitY(unit); W(pMode) = 1; return;
    default: done(count > 0 ? 0u : 0xFFFFFFFFu, 0, 0, 1); return;
    }
}
