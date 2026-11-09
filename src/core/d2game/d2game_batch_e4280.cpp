// Implementation batch e4280: minion/stat predicates, skill-state entry, unit list removal, cursor item
// placement and item-notify wrappers, written from the cached 1.13c decompiles (RE-DONE only, not linked
// into the engine: nothing live references these symbols, so the wasm link drops them).
// Callees are declared by retail RVA (rNNNN, D2Game base 0x6FC20000) or ordinal (oNNNNN). Register
// arguments the decompile shows as unaff_* / in_EAX are explicit parameters.
#include <cstdint>
#include <cstdlib>
#include <cstring>
using i32 = int32_t; using u32 = uint32_t; using u8 = uint8_t; using i16 = int16_t;

// Raw field access, as the decompile does it (32-bit retail pointers held in i32).
static inline i32 rd(i32 p, i32 off) { return *(i32*)(intptr_t)(p + off); }
static inline u8 rb(i32 p, i32 off) { return *(u8*)(intptr_t)(p + off); }
static inline void wr(i32 p, i32 off, i32 v) { *(i32*)(intptr_t)(p + off) = v; }
static inline void wb(i32 p, i32 off, u8 v) { *(u8*)(intptr_t)(p + off) = v; }

// the o10xxx imports keep this file's 32-bit signatures under a private namespace: other TUs declare the same env names with 64-bit words, and wasm-ld refuses two signatures on one symbol
namespace e4280_env {
__attribute__((weak, import_module("env"), import_name("o10174"))) i32 o10174(i32 unit, i32 stat, i32 idx);    // stat list lookup (unit or list, stat id, index)
__attribute__((weak, import_module("env"), import_name("o10003"))) void o10003(i32 unit, i32 stat, i32 idx);   // stat index write
__attribute__((weak, import_module("env"), import_name("o10920"))) i32 o10920(i32 game);                       // game -> player data context
__attribute__((weak, import_module("env"), import_name("o10265"))) i32 o10265(i32 err); __attribute__((weak, import_module("env"), import_name("o10024"))) void o10024(const void*, i32);
__attribute__((weak, import_module("env"), import_name("o10587"))) i32 o10587(); __attribute__((weak, import_module("env"), import_name("o10551"))) i32 o10551(); __attribute__((weak, import_module("env"), import_name("o10158"))) i32 o10158();
__attribute__((weak, import_module("env"), import_name("o10331"))) i32 o10331(i32 = 0); __attribute__((weak, import_module("env"), import_name("o10754"))) i32 o10754(i32); __attribute__((weak, import_module("env"), import_name("o10867"))) i32 o10867(i32);
__attribute__((weak, import_module("env"), import_name("o10637"))) void o10637(i32, i32); __attribute__((weak, import_module("env"), import_name("o10346"))) void o10346(i32, i32);
__attribute__((weak, import_module("env"), import_name("o10688"))) i32 o10688(i32); __attribute__((weak, import_module("env"), import_name("o11090"))) void o11090(i32, i32); __attribute__((weak, import_module("env"), import_name("o11010"))) i32 o11010(i32); __attribute__((weak, import_module("env"), import_name("o10383"))) i32 o10383(i32, i32*, i32*);
__attribute__((weak, import_module("env"), import_name("o10630"))) i32 o10630(); __attribute__((weak, import_module("env"), import_name("o11103"))) void o11103();
__attribute__((weak, import_module("env"), import_name("o10750"))) i32 o10750(i32);
} using namespace e4280_env;
namespace e4280_env {
__attribute__((weak, import_module("env"), import_name("r_E660"))) intptr_t r_E660(...);                      // FUN_6fc2e660, the magic check
__attribute__((weak, import_module("env"), import_name("r_354F0"))) i32 r_354F0(i32 game, i32 unit);            // FUN_6fc554f0
__attribute__((weak, import_module("env"), import_name("r_E2CD0"))) i32 r_E2CD0();                              // FUN_6fd02cd0, the mode gate
__attribute__((weak, import_module("env"), import_name("r_33F80"))) void r_33F80();                             // FUN_6fc53f80, list helper
__attribute__((weak, import_module("env"), import_name("r_E8780"))) i32 r_E8780(i32 unit);                      // FUN_6fd08780
__attribute__((weak, import_module("env"), import_name("r_E8140"))) void r_E8140(); __attribute__((weak, import_module("env"), import_name("r_E8010"))) void r_E8010();             // LAB_6fd08140 / LAB_6fd08010 callbacks
__attribute__((weak, import_module("env"), import_name("r_8EA80"))) intptr_t r_8EA80(...); // FUN_6fcaea80
__attribute__((weak, import_module("env"), import_name("r_6F930"))) intptr_t r_6F930(...); __attribute__((weak, import_module("env"), import_name("r_6F930_"))) i32 r_6F930_(i32 game, i32 rec); // FUN_6fc8f930
__attribute__((weak, import_module("env"), import_name("r_EAE20"))) intptr_t r_EAE20(...); __attribute__((weak, import_module("env"), import_name("r_EAD10"))) i32 r_EAD10();               // FUN_6fd0ae20 / FUN_6fd0ad10
__attribute__((weak, import_module("env"), import_name("r_EEF10"))) intptr_t r_EEF10(...);                     // FUN_6fd0ef10
__attribute__((weak, import_module("env"), import_name("r_EF940"))) intptr_t r_EF940(...); // FUN_6fd0f940
__attribute__((weak, import_module("env"), import_name("r_1320"))) intptr_t r_1320(...);                       // FUN_6fc21320, the skill row
} using namespace e4280_env;
extern i32** sgptDataTables_exref;
static inline i32 sdt(i32 off) { return *(i32*)(*(i32*)sgptDataTables_exref + off); }

// 1.13c D2Game+0xE4280 (ecx = owner record, edx = game)
i32 D2GAME_E4280(i32 self, i32 game)
{
    i32 p = o10920(game);
    i32 u = rd(p, 0x10 + (i32)rb(self, 0x6D) * 4);
    if (o10174(u, 0x13, 0) == 0 && o10174(u, 0x13, 6) == 0 && o10174(u, 0x13, 8) == 0 &&
        o10174(u, 0x13, 7) == 0 && r_E660(0x20333367) == 0)
        o10003(u, 0x13, 0xe);
    return 0;
}

// 1.13c D2Game+0xE4500 (list = stat list of the unit, a = record with byte +0x0C)
i32 D2GAME_E4500(i32 a, i32 id, i32 /*unused*/, i32 list)
{
    if (o10174(list, 0x13, 0)) return 0;
    if (id == 0xfd) return (r_E660(0x20333367) == 0 && rb(a, 0x0C) == 1) ? 1 : 0;
    if (id == 0xff) {
        if (r_E660(0x20333367) != 0) return 1;
        if (!o10174(list, 0x13, 6)) return 0;
        return o10174(list, 0x13, 8) == 0 ? 1 : 0;
    }
    if (id == 0xfc) {
        if (!o10174(list, 0x13, 6)) return 0;
        return o10174(list, 0x13, 7) == 0 ? 1 : 0;
    }
    return 0;
}

// 1.13c D2Game+0xE6420 (ecx = owner record, edx = game-side argument)
i32 D2GAME_E6420(i32 self, i32 arg)
{
    i32 p = o10920(arg);
    i32 u = rd(p, 0x10 + (i32)rb(self, 0x6D) * 4);
    if (o10174(u, 0x12, 0) == 0 && o10174(u, 0x12, 0xf) == 0) r_354F0(arg, self);
    return 0;
}

// 1.13c D2Game+0xE8200 (ebx = unit, eax = owner unit or null)
i32 D2GAME_E8200(i32 unit, i32 owner)
{
    i32 rec = rd(unit, 0x18);
    if (rb(unit, 9) == 0 || rb(rec, 0x0C) == 0 || rb(rec, 1) != 1 || (u8)r_E2CD0() != 0) return 0;
    i32 head = rd(unit, 4);
    i32 list = head ? rd(head, 0x10F4) : 0;
    if (list) {
        for (i32 n = rd(list, 0); n; n = rd(n, 0xF4)) {
            if (rd(n, 0) == 0x12) { r_33F80(); break; }
        }
    }
    wr(unit, 0xC0, (i32)(intptr_t)&r_E8010);
    wb(rec, 1, 0);
    wb(rec, 2, 1);
    wr(rec, 4, owner == 0 ? -1 : rd(owner, 0x0C));
    return 1;
}

// 1.13c D2Game+0xE8F30 (ecx = unit with list at +0xC4, edx = unit with record and callback, edi = unit for E8780)
// MISMATCH with docs/re: every path returns 1 (low byte); the spec's "early outs return 0" is not what the code does.
i32 D2GAME_E8F30(i32 p1, i32 p2, i32 edi)
{
    i32 rec = rd(p2, 0x18);
    wb(rec, 5, 0);
    if (rb(rec, 0x18) == 1 && rb(rec, 3) != 0 && rb(rec, 2) == 0 && rd(p1, 0xC4) != 0) {
        i32 base = rd(rec, 0x0C);
        u32 count = (u32)rd(rec, 8);
        i32 a = 0, b = 0;
        i32 iv = o11010(rd(p1, 0xC4));
        if (iv != 0) {
            o10383(iv, &a, &b);
            for (u32 i = 0; i < count; i++) {
                for (i32 n = rd(rd(base, (i32)(i * 4)), 0x74); n; n = rd(n, 0xE8)) {
                    if (rd(n, 0) == 0) {
                        r_E8780(edi);
                        wr(p2, 0xC0, (i32)(intptr_t)&r_E8140);
                        return 1;
                    }
                }
            }
        }
    }
    return 1;
}

// 1.13c D2Game+0xE9EA0 (eax = unit with data record at +0x18, game at +0x04)
// MISMATCH with docs/re: the id is read from the data record at +0x04 (the spec says +0x0C); the node id is at +0x0C.
void D2GAME_E9EA0(i32 unit)
{
    i32 d = rd(unit, 0x18);
    if (rb(d, 0) != 1) return;
    i32 game = rd(unit, 4);
    if (game == 0) { o10024(nullptr, o10265(0x2d3)); std::_Exit(-1); }
    i32 p = rd(game, 0x1520 + (rd(d, 4) & 0x7f) * 4);
    while (p != 0 && rd(p, 0x0C) != rd(d, 4)) p = rd(p, 0xE4);
    if (p == 0) {
        wb(d, 8, 2); wb(d, 9, 0); wb(d, 10, 0); wb(d, 0xb, 0);
        return;
    }
    o11090(p, rd(d, 8));
    if (rd(d, 8) != 2) {
        wb(d, 8, 2); wb(d, 9, 0); wb(d, 10, 0); wb(d, 0xb, 0);
        i32 c = o10688(rd(p, 4));
        r_8EA80(game, p, 1, (i32)(((u32)rd(c, 0xDC) >> 8) + (u32)rd(game, 0xA8)), 0, 0, 0);
    }
}

// 1.13c D2Game+0xEAB60 (game as the argument)
void D2GAME_EAB60(i32 game)
{
    i32 unit = o10331();
    i32 it = o10754(unit);
    if (it == 0) { o10024(nullptr, o10265(0x8f)); std::_Exit(-1); }
    i32 rec[0x17];
    std::memset(rec, 0, sizeof rec);
    rec[0] = 0x420;
    i32 t = rd(it, 0);
    if (t != 2 && (t < 4 || t > 5) && rd(it, 0x2C) != 0) o10867(rd(it, 0x2C));
    t = rd(it, 0);
    if (t != 2 && (t < 4 || t > 5) && rd(it, 0x2C) != 0) o10750(rd(it, 0x2C));
    i32 placed = r_6F930_(game, (i32)(intptr_t)rec);
    if (placed == 0) {
        if (rd(it, 0x10) == 0) {
            o11090(it, 1);
            i32 c = o10688(0x182);
            r_8EA80(game, it, 1, (i32)(((u32)rd(c, 0xDC) >> 8) + (u32)rd(game, 0xA8)), 0, 0, 0);
        }
        return;
    }
    o10637(placed, rd(it, 0x0C));
    o10346(o10331(it), 0);
}

// 1.13c D2Game+0xEF4B0 (seven arguments packed into a record; the record sink is FUN_6fd0ef10)
void D2GAME_EF4B0(i32 p1, i32 p2, i32 p3, i32 p4, i32 p5, i32 p6, i16 p7)
{
    i32 rec[10] = { p3, p4, 0, p1, p2, 0, p5, p6, -1, 0 };
    *(i16*)(&rec[9]) = p7;
    r_EEF10(rec);
}

// 1.13c D2Game+0xEFB80 (msg = ecx, eax = item-carrying unit)
i32 D2GAME_EFB80(i32 msg, i32 unit, i32 p1, i32 p2, i32 p5)
{
    if (msg != 0x154 || unit == 0) return 0;
    i32 item = rd(unit, 0x2C);
    i32 a, b;
    i32 t = rd(unit, 0);
    if (t == 2 || (t > 3 && t < 6)) a = rd(item, 0x0C);
    else if (item == 0) a = 0;
    else a = o10867(item);
    t = rd(unit, 0);
    if (t == 2 || (t > 3 && t < 6)) b = rd(item, 0x10);
    else if (item == 0) b = 0;
    else b = o10750(item);
    return r_EF940(p1, p2, a, b, 0, 0x154, p5);
}

// D2Game F3CB0 stand-in (eax = unit; superseded by d2game_f3cb0.cpp, which carries the 1.13c body)
i32 D2GAME_F3CB0(i32 unit)
{
    if (unit == 0) return 1;
    i32 row = r_1320(unit);
    if (row != 0) {
        i32 idx = (i32)(u32)(uint16_t)(*(uint16_t*)(intptr_t)(row + 0x10C));
        if (idx != 0 && idx < sdt(0xba0)) { o10630(); o11103(); }
        if (rb(row, 0x10B) < 0x10) return rb(row, 0x10B);
    }
    return 1;
}
