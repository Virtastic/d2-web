// Mapping batch bcb90: D2Game 1.13c functions in 0xBCB90-0xF5068 that had a spec but no body in src/.
// Written from the cached 1.13c decompiles in the style of d2game_batch_e4280b.cpp: RE-DONE only, not linked
// into the engine. Retail callees are declared by RVA (r_HEX, D2Game base 0x6FC20000) or ordinal (oNNNNN),
// variadic and returning the widest value. Register and frame slots the decompile shows as unaff_* / in_EAX
// are explicit parameters. Retail pointers are 32-bit fields read through rp().
#include <cstdint>
#include <cstring>

using i32 = int32_t; using u32 = uint32_t; using i16 = int16_t; using u16 = uint16_t; using u8 = uint8_t;
using P = intptr_t;
using R = intptr_t;

template <class T> static inline T ld(P p) { T v; std::memcpy(&v, (const void*)p, sizeof v); return v; }
template <class T> static inline void st(P p, T v) { std::memcpy((void*)p, &v, sizeof v); }
static inline i32 rd(P p, i32 off) { return ld<i32>(p + off); }
static inline P rp(P p, i32 off) { return (P)ld<u32>(p + off); }
static inline u16 rw(P p, i32 off) { return ld<u16>(p + off); }
static inline u8 rb(P p, i32 off) { return ld<u8>(p + off); }
static inline void wr(P p, i32 off, i32 v) { st<i32>(p + off, v); }
static inline void wp(P p, i32 off, P v) { st<u32>(p + off, (u32)v); }
static inline void wb(P p, i32 off, u8 v) { st<u8>(p + off, v); }
#define AD(x) ((P)&(x))

extern u8 DAT_6fd19ff9[];        // error-format string
extern u8 sgptDataTables_exref[];  // holds the data-table base pointer (u32)
static inline P sdtBase() { return (P)ld<u32>((P)sgptDataTables_exref); }
[[noreturn]] void d2_exit(int);

R o10024(...); R o10158(...); R o10174(...); R o10003(...); R o10265(...); R o10331(...); R o10383(...);
R o10427(...); R o10551(...); R o10587(...); R o10630(...); R o10688(...); R o10750(...); R o10826(...);
R o10867(...); R o10920(...); R o11010(...); R o11090(...); R o11103(...);
R r_1320(...); R r_33F80(...); R r_354A0(...); R r_35EB0(...); R r_8A3E0(...); R r_8EA80(...); R r_8F5C0(...);
R r_BCED0(...); R r_DFCC0(...); R r_E1C00(...); R r_E2CD0(...); R r_E660(...); R r_E8780(...); R r_EAD10(...);
R r_EAE20(...); R r_EEF10(...); R r_EF940(...); R r_354F0(...); R r_C8950(...);

[[noreturn]] static void fatal(u32 msg) { o10024(AD(DAT_6fd19ff9[0]), o10265(msg)); d2_exit(-1); }

// Walk the game's quest record list (control block at game+0x10F4, first record at +0, next at record+0xF4)
// for the first record whose number (+0x00) is `type`; 0 when none.
static P findQuest(P game, i32 type)
{
    P ctl = rp(game, 0x10F4);
    if (ctl == 0) return 0;
    for (P q = rp(ctl, 0); q != 0; q = rp(q, 0xF4)) if (rd(q, 0) == type) return q;
    return 0;
}

// 1.13c D2Game+0xBD330 (in_EAX = item record, param_1 = unit, param_2 = client/player)
void D2GAME_BD330(P in_EAX, P param_1, P param_2)
{
    if (in_EAX == 0) return;
    i32 id = rd(in_EAX, 4);
    if (id != 0x96 && id != 0xC6 && id != 0x203 && id != 0xFC) return;
    u8 notice[8] = { 0x4F };
    r_8A3E0(AD(notice), 1);
    if (param_2 == 0) fatal(0x996);
    P rec = (P)r_8F5C0(rp(param_2, 0x1A8));
    if (rec == 0) return;
    if (rb(rec, 0x21) == 0) r_BCED0(rp(param_2, 0x1A8));
    P slots = rp(rec, 0x10);
    P data = (P)o10427(rp(param_1, 0x70), id, 0, 0);
    if (data == 0) fatal(0x1F6);
    i32 n = (i32)rw(data, 0x116) - (i32)rw(data, 0x114) + 1;
    if (n == -1) fatal(0x5B);
    u8 notice7[8] = { 7 };   // the decompile leaves the notice buffer to the frame; only the code 7 is visible
    for (P e = slots + 8; n > 0; --n, e += 16)
        if (rd(e, 4) != 0 && rd(e, 0) == 0) r_8A3E0(AD(notice7), 7);
}

// 1.13c D2Game+0xC6D20 (in_EAX = game, param_1 = player)
void D2GAME_C6D20(P in_EAX, P param_1)
{
    P q = findQuest(in_EAX, 8);
    if (q == 0) return;
    if (rb(q, 9) != 1) return;
    if (!(rb(q, 0x0C) < 3 || rb(q, 0x0B) < 2)) return;
    if (o10826(o10331(param_1)) != 0x31) return;
    u8 b = rb(q, 0x0C);
    wr(q, 0xA8, 0);
    if (b < 3) r_35EB0();
    if (rb(q, 0x0B) < 2) {
        wb(q, 0x14, 0);
        P sub = rp(q, 0x18);
        bool z = rb(sub, 9) == 0;
        if (z) wb(sub, 9, 1);
        r_354A0(AD(q), 0, 2, z);
    } else if (b >= 3) {
        return;
    }
    r_DFCC0(0);
}

// 1.13c D2Game+0xC8950 (unaff_EBX = argument block: [0] = game, [1] = player)
void D2GAME_C8950(P unaff_EBX)
{
    P room = (P)o10331(rd(unaff_EBX, 4));
    for (P u = rp(room, 0x74); u != 0; u = rp(u, 0xE8))
        if (rd(u, 0) == 5) r_E1C00(rd(unaff_EBX, 0));
}

// 1.13c D2Game+0xC8C80 (in_EAX = argument block: [0] = game, [1] = player)
u32 D2GAME_C8C80(P in_EAX)
{
    P q = findQuest(rp(in_EAX, 0), 0xD);
    if (q != 0 && rb(q, 9) == 1 && rb(rp(q, 0x18), 0x0B) == 0) return 0;
    if (rd(in_EAX, 4) != 0 && rd(rp(in_EAX, 4), 0x10) == 2) r_C8950(in_EAX);
    return 0;
}

// 1.13c D2Game+0xE4280 (param_1 = record with slot byte at +0x6D, param_2 = owner)
u32 D2GAME_E4280(P param_1, P param_2)
{
    P tbl = (P)o10920(param_2);
    P list = rp(tbl, 0x10 + rb(param_1, 0x6D) * 4);
    if (o10174(list, 0x13, 0) == 0 && o10174(list, 0x13, 6) == 0 && o10174(list, 0x13, 8) == 0 &&
        o10174(list, 0x13, 7) == 0 && r_E660(0x20333367) == 0)
        o10003(list, 0x13, 0xE);
    return 0;
}

// 1.13c D2Game+0xE4500 (param_1 = record, param_2 = id, param_4 = stat list)
u32 D2GAME_E4500(P param_1, i32 param_2, P param_4)
{
    if (o10174(param_4, 0x13, 0) != 0) return 0;
    if (param_2 == 0xFD) return (r_E660(0x20333367) == 0 && rb(param_1, 0x0C) == 1) ? 1 : 0;
    if (param_2 == 0xFF) {
        if (r_E660(0x20333367) != 0) return 1;
        if (o10174(param_4, 0x13, 6) == 0) return 0;
        return o10174(param_4, 0x13, 8) == 0 ? 1 : 0;
    }
    if (param_2 == 0xFC) {
        if (o10174(param_4, 0x13, 6) == 0) return 0;
        return o10174(param_4, 0x13, 7) == 0 ? 1 : 0;
    }
    return 0;
}

// 1.13c D2Game+0xE6420 (param_1 = record with slot byte at +0x6D, param_2 = owner)
u32 D2GAME_E6420(P param_1, P param_2)
{
    P tbl = (P)o10920(param_2);
    P list = rp(tbl, 0x10 + rb(param_1, 0x6D) * 4);
    if (o10174(list, 0x12, 0) == 0 && o10174(list, 0x12, 0xF) == 0) r_354F0(param_2, param_1);
    return 0;
}

// 1.13c D2Game+0xE8200 (in_EAX = owner unit or null, unaff_EBX = unit); installs mode callback LAB_6fd08010 at +0xC0
u32 D2GAME_E8200(P in_EAX, P unaff_EBX, P labCallback)
{
    P rec = rp(unaff_EBX, 0x18);
    if (rb(unaff_EBX, 9) == 0 || rb(rec, 0x0C) == 0 || rb(rec, 1) != 1 || (u8)r_E2CD0() != 0) return 0;
    P ctl = rp(rp(unaff_EBX, 4), 0x10F4);
    if (ctl != 0)
        for (P q = rp(ctl, 0); q != 0; q = rp(q, 0xF4))
            if (rd(q, 0) == 0x12) { r_33F80(); break; }
    wp(unaff_EBX, 0xC0, labCallback);
    wb(rec, 1, 0);
    wb(rec, 2, 1);
    wr(rec, 4, in_EAX == 0 ? -1 : rd(in_EAX, 0x0C));
    return 1;
}

// 1.13c D2Game+0xE8F30 (param_1 = unit, param_2 = unit holding the 0x18 record, unaff_EDI = argument for E8780)
u32 D2GAME_E8F30(P param_1, P param_2, P unaff_EDI, P labCallback)
{
    P rec = rp(param_2, 0x18);
    wb(rec, 5, 0);
    if (rb(rec, 0x18) == 1 && rb(rec, 3) != 0 && rb(rec, 2) == 0 && rd(param_1, 0xC4) != 0) {
        P table = rp(rec, 0x0C);
        u32 count = (u32)rd(rec, 8);
        if (o11010(rd(param_1, 0xC4)) != 0) {
            i32 out1 = 0, out2 = 0;
            o10383(o11010(rd(param_1, 0xC4)), AD(out1), AD(out2));
            for (u32 i = 0; i < count; ++i)
                for (P n = rp(rp(table, (i32)(i * 4)), 0x74); n != 0; n = rp(n, 0xE8))
                    if (rd(n, 0) == 0) {
                        r_E8780(unaff_EDI);
                        wp(param_2, 0xC0, labCallback);   // LAB_6fd08140
                        return 1;
                    }
        }
    }
    return 1;
}

// 1.13c D2Game+0xE9EA0 (in_EAX = update record: [4] = game, [0x18] = state block)
void D2GAME_E9EA0(P in_EAX)
{
    P blk = rp(in_EAX, 0x18);
    if (rb(blk, 0) != 1) return;
    P game = rp(in_EAX, 4);
    if (game == 0) fatal(0x2D3);
    P n = rp(game, 0x1520 + ((u32)rd(blk, 4) & 0x7F) * 4);
    while (n != 0 && rd(n, 0x0C) != rd(blk, 4)) n = rp(n, 0xE4);
    if (n == 0) { wb(blk, 8, 2); wb(blk, 9, 0); wb(blk, 10, 0); wb(blk, 11, 0); return; }
    o11090(n, rd(blk, 8));
    if (rd(blk, 8) != 2) {
        wb(blk, 8, 2); wb(blk, 9, 0); wb(blk, 10, 0); wb(blk, 11, 0);
        P d = (P)o10688(rd(n, 4));
        r_8EA80(game, n, 1, (rd(d, 0xDC) >> 8) + rd(game, 0xA8), 0, 0, 0);
    }
}

// 1.13c D2Game+0xEAF80 (in_EAX = player unit; the Ordinal_10551/10587 arguments are not recovered)
void D2GAME_EAF80(P in_EAX)
{
    if (in_EAX == 0 || rd(in_EAX, 0) != 0) return;
    i32 idx = rd(in_EAX, 4);
    if (idx < 0 || idx >= rd(sdtBase(), 0xBC8)) return;
    P ent = (P)(idx * 0xC4 + rd(sdtBase(), 0xBC4));
    if (ent == 0) return;
    if (rb(ent, 0x30) != (u32)o10587()) { o10551(); o10551(); o10158(); }
    o10587();
    r_EAE20();
    if (rb(ent, 0x31) != (u32)o10587()) { o10551(); o10551(); o10158(); }
    o10587();
    r_EAD10();
}

// 1.13c D2Game+0xEF4B0 (ecx = param_1, edx = param_2, stack = param_3..param_7)
void D2GAME_EF4B0(u32 param_1, u32 param_2, i32 param_3, u32 param_4, u32 param_5, u32 param_6, u16 param_7)
{
    struct { i32 a; u32 b; u32 c; u32 d; u32 e; u32 f; u32 g; u32 h; u32 i; u16 j; } rec;
    rec.a = param_3; rec.b = param_4; rec.c = 0; rec.d = param_1; rec.e = param_2;
    rec.f = 0; rec.g = param_5; rec.h = param_6; rec.i = 0xFFFFFFFFu; rec.j = param_7;
    r_EEF10(AD(rec));
}

// 1.13c D2Game+0xEFB80 (this = message id, in_EAX = unit)
u32 D2GAME_EFB80(P thisMsg, P in_EAX, P param_1, u32 param_2, u16 param_5)
{
    if (thisMsg != 0x154 || in_EAX == 0) return 0;
    i32 t = rd(in_EAX, 0);
    P item = rp(in_EAX, 0x2C);
    bool direct = (t == 2) || (t > 3 && t < 6);
    i32 a = direct ? rd(item, 0x0C) : (item == 0 ? 0 : (i32)o10867(item));
    i32 b = direct ? rd(item, 0x10) : (item == 0 ? 0 : (i32)o10750(item));
    return (u32)r_EF940(param_1, param_2, a, b, 0, 0x154, param_5);
}

// 1.13c D2Game+0xF3CB0 (in_EAX = unit; the Ordinal_10630/11103 arguments are not recovered)
u8 D2GAME_F3CB0(P in_EAX)
{
    if (in_EAX == 0) return 1;
    P row = (P)r_1320();
    if (row != 0) {
        u16 s = rw(row, 0x10C);
        if (s != 0 && (i32)s < rd(sdtBase(), 0xBA0)) { o10630(); o11103(); }
        if (rb(row, 0x10B) < 0x10) return rb(row, 0x10B);
    }
    return 1;
}
