// 1.13c D2Game+0x6CC00 UNIT_BindType3BById. Written from the 1.13c disassembly.
// Register contract: eax = unit id, bl = state byte; stack: game (first), one further dword that is never read
// (ret 8). The Ghidra view drops both register arguments.
#include <cstdint>

namespace ordns_6cc00 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10216"))) int Ordinal_10216(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10258"))) uint32_t Ordinal_10258(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10111"))) void Ordinal_10111(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_11090"))) void Ordinal_11090(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10331"))) uint32_t Ordinal_10331(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10885"))) void Ordinal_10885(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10826"))) uint32_t Ordinal_10826(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10265"))) uint32_t Ordinal_10265(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10024"))) void Ordinal_10024(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fd01570"))) uint32_t FUN_6fd01570(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fd020a0"))) void FUN_6fd020a0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcc22e0"))) uint32_t FUN_6fcc22e0(...);
}

namespace {
using namespace ordns_6cc00;
inline uint8_t* at8(uintptr_t b, uint32_t o) { return reinterpret_cast<uint8_t*>(b + o); }
inline uint32_t* at32(uintptr_t b, uint32_t o) { return reinterpret_cast<uint32_t*>(b + o); }
[[noreturn]] void fatal(int line) { Ordinal_10024("", Ordinal_10265(line), line); __builtin_trap(); }
}

// 1.13c D2Game+0x6CC00
extern "C" void D2GAME_UNIT_BindType3BById_6CC00(uint32_t id, uint8_t state, uintptr_t game, uint32_t /*unused*/)
{
    if (!game) fatal(0x11DD);
    // unit hash: 128 buckets at game+0x1520, chain link +0xE4, id at +0xC
    uintptr_t u = *at32(game, 0x1520 + (id & 0x7F) * 4);
    while (u && *at32(u, 0xC) != id) u = *at32(u, 0xE4);
    if (!u || *at32(u, 4) != 0x3B || Ordinal_10216(u, 1) != 0) return;
    uintptr_t partner = FUN_6fd01570(game, u);
    if (!partner) return;
    if (*at32(partner, 4) != 0x3B) fatal(0x11EA);
    *at8(*at32(u, 0x14), 4) = state;
    uint32_t fl = (Ordinal_10258(u) & 0xFF) | 1;
    Ordinal_10111(u, fl);
    Ordinal_11090(u, 3);
    Ordinal_10885(Ordinal_10331(partner, 2, *at32(partner, 0xC)));
    FUN_6fd020a0(game, partner);
    uintptr_t nu = FUN_6fcc22e0(game, u, state, Ordinal_10826(Ordinal_10331(u)));
    if (!nu) fatal(0x11FB);
    Ordinal_10111(nu, (Ordinal_10258(nu) & 0xFF) | 3);
}
