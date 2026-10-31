// 1.13c D2Game+0x6D7B0 AITHINK_IdleSpawnRoll and 0x6CE10 AITHINK_IdleRoll90. Written from the 1.13c disassembly.
// Both take ecx = think context (ctx[0] game, [1] unit, [2] target, [3] seed ptr, [4] monster id) and return 0.
#include <cstdint>

namespace ordns_6d7b0 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10331"))) uint32_t Ordinal_10331(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10826"))) uint32_t Ordinal_10826(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10708"))) uint32_t Ordinal_10708(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_11129"))) void Ordinal_11129(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10136"))) uint32_t Ordinal_10136(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_11090"))) void Ordinal_11090(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10408"))) void Ordinal_10408(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10688"))) uint32_t Ordinal_10688(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc98430"))) void FUN_6fc98430(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc2b510"))) uint32_t FUN_6fc2b510(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc8b570"))) int FUN_6fc8b570(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void FUN_6fcaea80(...);
}
extern "C" void D2GAME_QUEST_DispatchIndexedQuestEvent_6A050(int32_t index, const int32_t* esi, uint32_t edi, const void* tables);
extern "C" int* sgptDataTables_exref;

namespace {
using namespace ordns_6d7b0;
inline uint32_t rd32(uintptr_t b, uint32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
inline uint32_t* w32(uintptr_t b, uint32_t o) { return reinterpret_cast<uint32_t*>(b + o); }
inline uint32_t lcg(uint32_t* seed) {
    uint64_t v = static_cast<uint64_t>(seed[0]) * 0x6ac690c5ull + seed[1];
    seed[0] = static_cast<uint32_t>(v); seed[1] = static_cast<uint32_t>(v >> 32);
    return seed[0];
}
inline uint32_t* seedOf(const int32_t* ctx) { return reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(ctx[3]))); }
// mode-1 send at frame (record+0xDC >> 8) + 1 + game+0xA8 on the record of the monster id.
inline void sendMode1(const int32_t* ctx, uintptr_t rec) {
    uintptr_t game = static_cast<uint32_t>(ctx[0]);
    FUN_6fcaea80(game, static_cast<uint32_t>(ctx[1]), 1, (rec ? (rd32(rec, 0xDC) >> 8) : 0) + 1 + rd32(game, 0xA8), 0, 0, 0);
}
}

// 1.13c D2Game+0x6D7B0
extern "C" int D2GAME_AI_IdleSpawnRoll_6D7B0(int32_t* ctx)
{
    uintptr_t unit = static_cast<uint32_t>(ctx[1]);
    if (unit && rd32(unit, 0x10)) return 0;
    Ordinal_10826(Ordinal_10331(unit));  // result unused by the original
    uintptr_t tgt = static_cast<uint32_t>(ctx[2]);
    if (tgt && rd32(tgt, 0) == 0) {
        // ecx = target, edx = 1, stack -1 (register-convention ordinal)
        uint32_t item = Ordinal_10708(tgt, 1, -1);
        if (item) {
            Ordinal_11129(tgt, unit);
            uint32_t id = unit ? rd32(unit, 0xC) : 0xFFFFFFFFu;
            uint32_t r = Ordinal_10136(item, 2, id, 0);
            // eax = game, edi = target, stack (item, r)
            FUN_6fc98430(static_cast<uint32_t>(ctx[0]), tgt, item, r);
        }
    }
    Ordinal_11090(static_cast<uint32_t>(ctx[1]), 1);
    if (ctx[1]) *w32(static_cast<uint32_t>(ctx[1]), 0xC4) &= 0xFFFFFFFDu;
    Ordinal_10408(static_cast<uint32_t>(ctx[1]));
    uint32_t* seed = seedOf(ctx);
    uint32_t oldHi = seed[1];
    uint32_t lo = lcg(seed);
    if ((lo % 10000) & 0xFFFFE000u) {
        uint32_t idx = FUN_6fc2b510(ctx, 8);  // eax = context, stack 8
        D2GAME_QUEST_DispatchIndexedQuestEvent_6A050(static_cast<int32_t>(idx), ctx, oldHi,
                                                     reinterpret_cast<const void*>(static_cast<uintptr_t>(static_cast<uint32_t>(*sgptDataTables_exref))));
    }
    if (lcg(seed) % 100 <= 0x14) FUN_6fc8b570(ctx, 0);
    sendMode1(ctx, Ordinal_10688(static_cast<int32_t>(ctx[4])));
    return 0;
}

// 1.13c D2Game+0x6CE10
extern "C" int D2GAME_AI_IdleRoll90_6CE10(int32_t* ctx)
{
    uintptr_t unit = static_cast<uint32_t>(ctx[1]);
    if (unit && rd32(unit, 0x10)) return 0;
    uint32_t id = unit ? rd32(unit, 4) : 0xFFFFFFFFu;
    uintptr_t rec = Ordinal_10688(id);
    if (static_cast<int32_t>(lcg(seedOf(ctx)) % 100) < 0x5A) {
        if (FUN_6fc8b570(ctx, 0)) Ordinal_11090(static_cast<uint32_t>(ctx[1]), 5);
        return 0;
    }
    Ordinal_11090(static_cast<uint32_t>(ctx[1]), 1);
    sendMode1(ctx, rec);
    uintptr_t game = static_cast<uint32_t>(ctx[0]);
    FUN_6fcaea80(game, static_cast<uint32_t>(ctx[1]), 9, rd32(game, 0xA8) + 0x14, 0, 0, 0);
    return 0;
}
