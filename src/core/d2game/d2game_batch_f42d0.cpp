// 1.13c D2Game+0xF42D0 (chance roll with level bias, then apply) and its helper 0xF4260.
// Written from the 1.13c disassembly. F42D0 is stdcall, four stack args, ret 0x10:
//   (game, unit, target, mode). unit is the rolling monster (seed at unit+0x20), target is the unit whose stats
//   bias the roll. The Ghidra signature (record in arg 3, quotient result) is wrong; there is no meaningful result.
// F4260 takes eax = hi, edi = lo, ecx = unit and returns the percent-scaled roll (the vendored F4260_scale in
// d2game_batch_F4260.cpp does not match this body).
#include <cstdint>

namespace ordns_f42d0 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10331"))) uint32_t Ordinal_10331(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10826"))) uint32_t Ordinal_10826(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10057"))) int Ordinal_10057(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10324"))) int Ordinal_10324(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10973"))) int Ordinal_10973(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10494"))) int Ordinal_10494(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10688"))) uint32_t Ordinal_10688(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10265"))) uint32_t Ordinal_10265(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10024"))) void Ordinal_10024(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc211d0"))) uint32_t FUN_6fc211d0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcfc0b0"))) void FUN_6fcfc0b0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcfe0c0"))) void FUN_6fcfe0c0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fcfcf20"))) void FUN_6fcfcf20(...);
}

namespace {
using namespace ordns_f42d0;
inline uint32_t rd32(uintptr_t b, uint32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
[[noreturn]] void fatal(int line) { Ordinal_10024("", Ordinal_10265(line), line); __builtin_trap(); }
}

// 1.13c D2Game+0xF4260. eax = hi, edi = lo, ecx = unit.
extern "C" uint32_t D2GAME_F4260_ScaledRoll(uint32_t hi, uint32_t lo, uintptr_t unit)
{
    if (lo > hi) fatal(0x14);
    uint32_t r = FUN_6fc211d0(unit + 0x20, hi - lo + 0x100) + lo;
    uint32_t id = unit ? rd32(unit, 4) : 0xFFFFFFFFu;
    uint32_t pct = *reinterpret_cast<uint8_t*>(Ordinal_10688(id) + 0x19C);
    if (pct == 0) fatal(0x1B);
    return static_cast<uint32_t>((static_cast<uint64_t>(pct * r) * 0x51EB851Full) >> 37);
}

// 1.13c D2Game+0xF42D0
extern "C" void D2GAME_F42D0_ChanceRollApply(uintptr_t game, uintptr_t unit, uintptr_t target, int mode)
{
    uint32_t room = Ordinal_10331(target);
    Ordinal_10826(room);  // result unused by the original
    if (Ordinal_10057(room)) return;
    if (Ordinal_10324(target, unit, 0x804)) return;
    uint32_t* seed = reinterpret_cast<uint32_t*>(unit + 0x20);
    if (!target) fatal(0x3F);
    int a = Ordinal_10973(target, 6, 0);
    int lo = a >> 5; if (lo <= 1) lo = 1;
    int hi = a >> 3; if (hi <= lo + 1) hi = lo + 1;
    int b = Ordinal_10973(target, 0xC, 0);
    uint8_t tmp = static_cast<uint8_t>(FUN_6fc211d0(seed, b >> 2) + static_cast<uint8_t>(b));
    int t = static_cast<int>(tmp) - (Ordinal_10973(target, 2, 0) >> 1) * 5;
    t = (t - b) * 2 - Ordinal_10973(target, 0x1F, 0) + 0x7D;
    if (t < 0x41) t = 0x41;
    uint64_t v = static_cast<uint64_t>(seed[0]) * 0x6ac690c5ull + seed[1];
    seed[0] = static_cast<uint32_t>(v); seed[1] = static_cast<uint32_t>(v >> 32);
    if (static_cast<int>(seed[0] % 100) >= t) return;
    uint32_t dmg = D2GAME_F4260_ScaledRoll(hi, lo, unit);
    if (!dmg) return;
    uint32_t rec[0x1C] = {};
    switch (mode) {
        case 1: rec[4] = dmg; rec[0x18] = 0x20; break;
        case 2: rec[7] = dmg; rec[0x18] = 0x40; break;
        case 3: rec[9] = dmg; rec[0x18] = 0x30; break;
        default: rec[2] = dmg; break;
    }
    rec[1] = 1;  // 16-bit store of 1 at +4 (the dword's upper half is already zero)
    if (Ordinal_10494(target, 0x36) == 0) rec[1] |= 4;
    FUN_6fcfc0b0(game, unit, target, rec);
    FUN_6fcfe0c0(target /*eax*/, game, unit, 0, rec);
    FUN_6fcfcf20(target /*eax*/, game, unit, rec);
}
