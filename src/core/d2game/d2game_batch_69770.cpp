// 1.13c D2Game+0x69770 0x697A0 0x69800 0x69860 0x698A0 0x6AB60
// Small fastcall (ecx, edx) skill-effect callbacks. Written from the 1.13c disassembly; the Ghidra views of
// 6AB60 drop the 6A960 register/stack arguments and the 89EE0 register arguments.
// c = effect context (+4 game, +8 unit); e = record (+0 byte table index, +4 value, +8, +0xC).
#include <cstdint>

namespace ordns_69770 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10920"))) uintptr_t Ordinal_10920(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10973"))) int Ordinal_10973(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10551"))) void Ordinal_10551(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10186"))) int Ordinal_10186(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10913"))) int Ordinal_10913(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10418"))) void Ordinal_10418(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10578"))) uint32_t Ordinal_10578(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10887"))) void Ordinal_10887(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10261"))) void Ordinal_10261(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10265"))) uint32_t Ordinal_10265(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10024"))) void Ordinal_10024(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc22e10"))) char* FUN_6fc22e10(...);   // __strrev
__attribute__((weak, import_module("env"), import_name("FUN_6fc22e50"))) char* FUN_6fc22e50(...);   // _strncpy
__attribute__((weak, import_module("env"), import_name("FUN_6fcc00b0"))) uint32_t FUN_6fcc00b0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc89ee0"))) uint32_t FUN_6fc89ee0(...); // esi=unit, ebx=value, stack=selector
__attribute__((weak, import_module("env"), import_name("FUN_6fcb6fa0"))) void FUN_6fcb6fa0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc8a930"))) void FUN_6fc8a930(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc8a900"))) void FUN_6fc8a900(...);
}

// Effect table at D2Game+0x1A5F8: 24 entries of three dwords {?, selector (-1 = none), arg}; count at +0x27D4C.
struct EffEntry { int32_t a, sel, arg; };
extern "C" EffEntry g_D2Game_effTable[] __attribute__((weak));
extern "C" int32_t g_D2Game_effCount __attribute__((weak));

namespace {
using namespace ordns_69770;
inline uint32_t r32(uintptr_t b, uint32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
[[noreturn]] void fatal(int line) { Ordinal_10024("", Ordinal_10265(line), line); __builtin_trap(); }

// 6A960: ecx=game, edx=unit, five stack dwords; fills a 9-dword block and calls 6FCC00B0 (eax = block).
uint32_t make6A960(uint32_t g, uint32_t u, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    uint32_t b[9] = {g, u, 0, 0, a1, a3, a2, a4, a5};
    return FUN_6fcc00b0(b);
}
const EffEntry& entry(uint8_t idx, int line)
{
    if (static_cast<int32_t>(idx) >= g_D2Game_effCount) fatal(line);
    return g_D2Game_effTable[idx];
}
}

// 1.13c D2Game+0x69770
extern "C" void D2GAME_69770(uintptr_t c)
{
    char* s = reinterpret_cast<char*>(Ordinal_10920(r32(c, 8)));
    if (s) FUN_6fc22e50(s, FUN_6fc22e10(s), 0x10);
}

// 1.13c D2Game+0x697A0 DrainManaIntoLife: move e[4] percent of mana (stat 8) out, e[8] percent of that into life (stat 6)
extern "C" void D2GAME_DrainManaIntoLife_697A0(uintptr_t c, uintptr_t e)
{
    uint32_t cur = Ordinal_10973(r32(c, 8), 8, 0);
    uint32_t amt = (r32(e, 4) * cur) / 100u;
    Ordinal_10551(r32(c, 8), 8, -static_cast<int32_t>(amt), 0);
    Ordinal_10551(r32(c, 8), 6, (r32(e, 8) * amt) / 100u, 0);
}

// 1.13c D2Game+0x69800 DrainLifeIntoMana: same with the 8.8 fixed-point life stat 6 and mana stat 8
extern "C" void D2GAME_DrainLifeIntoMana_69800(uintptr_t c, uintptr_t e)
{
    int32_t cur = Ordinal_10973(r32(c, 8), 6, 0);
    uint32_t amt = (r32(e, 4) * static_cast<uint32_t>(cur >> 8)) / 100u;
    uint32_t amt256 = amt << 8;
    Ordinal_10551(r32(c, 8), 6, -static_cast<int32_t>(amt256), 0);
    Ordinal_10551(r32(c, 8), 8, (r32(e, 8) * amt256) / 100u, 0);
}

// 1.13c D2Game+0x69860 : bring stat 8 up to Ordinal_10186(unit)
extern "C" void D2GAME_69860(uintptr_t c)
{
    int32_t cur = Ordinal_10973(r32(c, 8), 8, 0);
    int32_t d = Ordinal_10186(r32(c, 8)) - cur;
    if (d) Ordinal_10551(r32(c, 8), 8, d, 0);
}

// 1.13c D2Game+0x698A0 : bring stat 6 up to Ordinal_10913(unit)
extern "C" void D2GAME_698A0(uintptr_t c)
{
    int32_t cur = Ordinal_10973(r32(c, 8), 6, 0);
    int32_t d = Ordinal_10913(r32(c, 8)) - cur;
    if (d) Ordinal_10551(r32(c, 8), 6, d, 0);
}

// 1.13c D2Game+0x6AB60 : spawn effect 0xB from the table row's arg, then notify the unit
extern "C" void D2GAME_6AB60(uintptr_t c, uintptr_t e)
{
    const EffEntry& t = g_D2Game_effTable[*reinterpret_cast<uint8_t*>(e)];
    make6A960(r32(c, 4), r32(c, 8), r32(e, 0xC), 0, 0xB, t.arg, reinterpret_cast<uintptr_t>(&FUN_6fc8a930));
    uintptr_t u = r32(c, 8);
    Ordinal_10418(u);
    if (u && r32(u, 0) == 0) FUN_6fcb6fa0(r32(u, 0x80), u);
}
