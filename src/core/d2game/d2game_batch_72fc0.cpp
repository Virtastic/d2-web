// 1.13c D2Game+0x72FC0 ITEM_FindCubeRecipeAndApply. Written from the 1.13c disassembly.
// Stdcall (ret 8): arg1 = player-side context G (+0x6A,+0x6D,+0x70,+0x74), arg2 = unit (null = return).
// Walks the recipe table (Ordinal_10675 = count, 10783 = row), filters on the row's enable/difficulty/class/
// mode fields and a 14-way condition switch, then asks FUN_6fc90cf0 whether the row matches and applies it
// through FUN_6fc92220. Stops at the first row that applies.
#include <cstdint>

namespace ordns_72fc0 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10675"))) int Ordinal_10675(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10783"))) uintptr_t Ordinal_10783(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10973"))) int Ordinal_10973(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10587"))) int Ordinal_10587(...);
__attribute__((weak, import_module("env"), import_name("Ordinal_10379"))) int Ordinal_10379(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc90190"))) int FUN_6fc90190(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc210d0"))) uintptr_t FUN_6fc210d0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc90cf0"))) int FUN_6fc90cf0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc92220"))) void FUN_6fc92220(...);
}
// SYSTEMTIME fill (wYear, wMonth, wDayOfWeek, wDay, ...); the host provides it, a missing import reads as zeros.
extern "C" __attribute__((weak, import_module("env"), import_name("d2web_GetLocalTime"))) void d2web_GetLocalTime(uint16_t* st);

extern "C" int* sgptDataTables_exref;
// global dword at D2Game+0x111C14 (6fd31c14); a weak undefined data symbol reads as 0 when not provided.
extern "C" uint32_t g_D2Game_31C14 __attribute__((weak));

namespace {
using namespace ordns_72fc0;
inline uint8_t r8(uintptr_t b, uint32_t o) { return *reinterpret_cast<uint8_t*>(b + o); }
inline uint16_t r16(uintptr_t b, uint32_t o) { return *reinterpret_cast<uint16_t*>(b + o); }
inline int32_t r32(uintptr_t b, uint32_t o) { return *reinterpret_cast<int32_t*>(b + o); }
}

// 1.13c D2Game+0x72FC0
extern "C" void D2GAME_ITEM_FindCubeRecipeAndApply_72FC0(uintptr_t G, uintptr_t unit)
{
    if (!unit) return;
    int count = Ordinal_10675();
    int cls = FUN_6fc90190(r32(unit, 0x60));
    if (cls <= 0) return;

    uint16_t st[8] = {0};
    if (d2web_GetLocalTime) d2web_GetLocalTime(st);
    int32_t dayOfWeekPlus1 = st[2] + 1;   // edi at 6fc93004: wDayOfWeek + 1
    int32_t dayOfMonth = st[3];

    for (int i = 0; i < count; ++i) {
        uintptr_t rec = Ordinal_10783(i);
        if (!rec) return;
        if (r8(rec, 0) == 0 && g_D2Game_31C14 == 0) continue;
        if (r32(G, 0x70) == 0 && r16(rec, 0x12) >= 0x64) continue;
        if (r8(G, 0x6A) == 0 && r32(G, 0x74) == 0 && r8(rec, 1) != 0) continue;
        if (r8(rec, 2) > r8(G, 0x6D)) continue;
        uint8_t want = r8(rec, 3);
        if (want != 0xFF && want != static_cast<uint32_t>(r32(unit, 4))) continue;
        if (r8(rec, 0x10) != static_cast<uint32_t>(cls)) continue;

        uint32_t kind = r8(rec, 4) - 1u;
        bool pass = true;
        if (kind <= 13) {
            int32_t a = r32(rec, 8);
            int32_t b = r32(rec, 0xC);
            if (kind == 0) {
                if (a > dayOfMonth || b < dayOfMonth) pass = false;
            } else if (kind == 1) {
                if (b != dayOfWeekPlus1) pass = false;
            } else if (a >= 0 && a <= *reinterpret_cast<int32_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(*sgptDataTables_exref)) + 0xBD4)) {
                uintptr_t st = FUN_6fc210d0(a);
                if (!st) { continue; }
                int32_t want2 = b >> r8(st, 0x18);
                int32_t have;
                uint32_t sel = (kind - 2) / 4;     // 0: 10973, 1: 10587, 2: 10379
                uint32_t cmp = (kind - 2) % 4;     // 0 <, 1 >, 2 ==, 3 !=  (skip when true)
                have = sel == 0 ? Ordinal_10973(unit, a, 0) : sel == 1 ? Ordinal_10587(unit, a, 0) : Ordinal_10379(unit, a, 0);
                bool skip = cmp == 0 ? have < want2 : cmp == 1 ? have > want2 : cmp == 2 ? have == want2 : have != want2;
                if (skip) pass = false;
            }
        }
        if (!pass) continue;

        uint32_t blk[8] = {0};
        // FUN_6fc90cf0: eax = recipe row, stack = (G, blk); ebx = *(unit+0x60) is live across the call.
        if (FUN_6fc90cf0(rec, G, blk, r32(unit, 0x60))) {
            FUN_6fc92220(G, unit, rec, blk);
            return;
        }
    }
}
