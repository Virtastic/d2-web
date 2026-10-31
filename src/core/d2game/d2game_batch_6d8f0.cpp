// AI think table entries 0x6D8F0 and 0x6DE00 (1.13c D2Game, data at 0x107C00 and 0x107C2C).
// Written from the cached 1.13c decompile and the specs AITHINK_ResetToStandMode_6D8F0.md and
// AITHINK_FlagBit2Refresh_6DE00.md; both matched the decompile line for line. wasm32 pointers are 4 bytes,
// so the think context is an int array: [0] game, [1] unit, [4] monster id (param+0x10).
#include <cstdint>

namespace ordns_6d8f0 {
__attribute__((weak, import_module("env"), import_name("Ordinal_11090"))) void Ordinal_11090(void*, int);
__attribute__((weak, import_module("env"), import_name("Ordinal_10688"))) void* Ordinal_10688(int);
__attribute__((weak, import_module("env"), import_name("Ordinal_10408"))) void Ordinal_10408(void*);
__attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void* FUN_6fcaea80(void*, void*, int, int, int, int, int);
}

namespace {
using ordns_6d8f0::Ordinal_11090;
using ordns_6d8f0::Ordinal_10688;
using ordns_6d8f0::Ordinal_10408;
using ordns_6d8f0::FUN_6fcaea80;

inline uint8_t* at8(uintptr_t base, uint32_t off) { return reinterpret_cast<uint8_t*>(base + off); }
inline uint32_t* at32(uintptr_t base, uint32_t off) { return reinterpret_cast<uint32_t*>(base + off); }

inline uintptr_t unitOf(const int32_t* ctx) { return static_cast<uintptr_t>(static_cast<uint32_t>(ctx[1])); }
inline uintptr_t gameOf(const int32_t* ctx) { return static_cast<uintptr_t>(static_cast<uint32_t>(ctx[0])); }
}

// 1.13c D2Game+0x6D8F0 (caller: AI think table data reference at 0x107C00). Returns 1 in every path.
// A null unit, or one whose +0x10 word is 0, runs the body: mode 1, clear +0xC4 bit 1, a mode-1 send at
// frame (record+0xDC >> 8) + 1 + game+0xA8 on the record of the monster id at ctx[4], then ordinal 10408.
extern "C" int D2GAME_AI_ResetToStandMode_6D8F0(int32_t* ctx)
{
    uintptr_t unit = unitOf(ctx);
    if (unit == 0 || *at32(unit, 0x10) == 0) {
        Ordinal_11090(reinterpret_cast<void*>(unit), 1);
        if (unitOf(ctx) != 0) *at32(unit, 0xC4) &= ~0x2u;
        uintptr_t rec = reinterpret_cast<uintptr_t>(Ordinal_10688(ctx[4]));
        FUN_6fcaea80(reinterpret_cast<void*>(gameOf(ctx)), reinterpret_cast<void*>(unitOf(ctx)), 1,
                     static_cast<int>((*at32(rec, 0xDC) >> 8) + 1 + *at32(gameOf(ctx), 0xA8)), 0, 0, 0);
        Ordinal_10408(reinterpret_cast<void*>(unitOf(ctx)));
    }
    return 1;
}

// 1.13c D2Game+0x6DE00 (caller: AI think table data reference at 0x107C2C). Returns 1 in every path.
// A non-null unit with +0x10 != 0 is busy. Otherwise mode 1, ordinal 10408, then bit 2 of +0xC4 follows the
// record byte at +0xC4 + mode; a record with +0x141 set sends mode 1 at the same frame formula as 0x6D8F0.
extern "C" int D2GAME_AI_FlagBit2Refresh_6DE00(int32_t* ctx)
{
    uintptr_t unit = unitOf(ctx);
    if (unit != 0 && *at32(unit, 0x10) != 0) return 1;
    Ordinal_11090(reinterpret_cast<void*>(unit), 1);
    Ordinal_10408(reinterpret_cast<void*>(unit));
    uint32_t mode = (unit == 0) ? 0u : *at32(unitOf(ctx), 0x10);
    uintptr_t rec = reinterpret_cast<uintptr_t>(Ordinal_10688(ctx[4]));
    if (unitOf(ctx) != 0) {
        uintptr_t u = unitOf(ctx);
        if (*at8(rec, 0xC4 + mode) == 0) *at32(u, 0xC4) &= ~0x2u;
        else                              *at32(u, 0xC4) |= 0x2u;
    }
    rec = reinterpret_cast<uintptr_t>(Ordinal_10688(ctx[4]));
    if (*at8(rec, 0x141) != 0) {
        uintptr_t game = gameOf(ctx);
        uintptr_t rec2 = reinterpret_cast<uintptr_t>(Ordinal_10688(ctx[4]));
        FUN_6fcaea80(reinterpret_cast<void*>(game), reinterpret_cast<void*>(unitOf(ctx)), 1,
                     static_cast<int>((*at32(rec2, 0xDC) >> 8) + 1 + *at32(game, 0xA8)), 0, 0, 0);
    }
    return 1;
}
