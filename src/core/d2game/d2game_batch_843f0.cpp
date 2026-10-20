// Batch of five D2Game monster-AI handlers (1.13c D2Game+0x843F0..0x877B0), decompile-derived.
// Env-seam style: callees are function pointers on Env; register args that the decompile lost
// (unaff_*) are explicit params. Struct offsets are the retail 32-bit ones, read through rd<>().
#include <cstdint>
#include <cstring>

namespace d2game_batch_843f0 {

using P = uintptr_t;
struct Pkt { uint32_t type; P unit; uint32_t a, b, c, d, e, f; };  // GUESS: layout of the 0x18-byte AI packet

struct Env {
    P gdwBitMasks;                                    // dword table: [4]=+0x10 [3]=+0xC [2]=+8 [5]=+0x14
    void (*SetMode)(P unit, int);                     // Ordinal_11103 / 10035 (first use)
    int  (*IsBoss)(P unit);                           // Ordinal_10830
    int  (*GetStat)(P unit);                          // Ordinal_11027
    P    (*GetClientData)(P unit);                    // FUN_F21320
    P    (*GetTarget)(P game, P aiTick);              // F2280-ish seed/target lookup, FUN_F2280 (GUESS)
    uint32_t (*Roll100)(uint64_t* seed);              // FUN_F1180
    void (*SendPkt)(P game, Pkt*);                    // FUN_FC95E10
    void (*Wait)(int ticks);                          // FUN_FD122C0
    void (*Wait2)(int ticks);                         // FUN_FD120C0
    void (*Attack)(P game, P unit, P target, int, int, int, int); // FUN_FD12540
    void (*Approach)(P target, P unit, P game);       // FUN_FD13000 / FD12F30 (a4 extra)
    void (*Flee)(P game, P target);                   // FUN_FD12220
    void (*Circle)(P unit);                           // FUN_FD12380
    int  (*Walk)(P game, P unit, int);                // FUN_FD12D80 / FD12C10
    void (*Strafe)(P unit, P game, P target, int);    // FUN_FD126B0
    void (*Special)(P target, P unit, P game, uint8_t); // FUN_FD12F30
    int  (*FindTarget)(void* res, P game, P unit, P cb); // FUN_FCD17B0
    int  (*TargetDist)(P game, uint32_t* res);        // FUN_FCD1BC0
    int  (*TryCast)(P unit);                          // FUN_FC9C6E0
    P    (*GetMissileSkill)();                        // FUN_F21280
    P    (*Missile)(int, int, P game, P skill, int, P, int, int); // FUN_FD0F650
    void (*GetXYZ)(P unit, int*, int*, int*, P*, int*, int, int); // Ordinal_10751
    void (*GetDir)(P unit);                           // Ordinal_10331
    void (*SetStat)(P unit, int stat, int val);       // Ordinal_10945
    void (*SetState)(P unit, int st, int on);         // Ordinal_10689
};

template <class T> static T rd(P b, uint32_t off) { T v; memcpy(&v, (void*)(b + off), sizeof v); return v; }
template <class T> static void wr(P b, uint32_t off, T v) { memcpy((void*)(b + off), &v, sizeof v); }

static int16_t row(P tbl, uint32_t base, P game) { return rd<int16_t>(tbl, base + rd<uint8_t>(game, 0x6d) * 2); }
static uint32_t rnd(P u)  // unit+0x20 LCG, returns low dword
{
    uint64_t s = rd<uint64_t>(u, 0x20);
    s = (uint32_t)s * 0x6ac690c5ull + (uint32_t)(s >> 32);
    wr<uint64_t>(u, 0x20, s);
    return (uint32_t)s;
}
static P mdata(P u) { return (u && rd<int32_t>(u, 0) == 1 && rd<P>(u, 0x14)) ? rd<P>(rd<P>(u, 0x14), 0x2c) : 0; }
static void setAnim(P u, int dir, int mode)
{ P m = mdata(u); if (dir) wr<int32_t>(m, 0x18, dir); wr<uint8_t>(m, 0x20, (uint8_t)mode); }
static int coin(P u) { int d = (rnd(u) & 0xff) < 0x80 ? 5 : 6; return d == 1 ? 7 : d; }
static void sendFlags(Env& e, P game, P unit, uint32_t type, uint32_t maskIdx, uint32_t byteOff, bool low)
{
    Pkt p{}; p.type = type; p.unit = unit;
    uint8_t v = 100;
    if (unit) { P c = e.GetClientData(unit); if (c && (rd<uint8_t>(c, byteOff) & rd<uint32_t>(e.gdwBitMasks, maskIdx * 4))) v = 0x65; }
    (void)low;
    p.c = (p.c & ~0xff00u) | ((uint32_t)v << 8);
    e.SendPkt(game, &p);
}

// 1.13c D2Game+0x843F0  (ebx=unaff_EBX flag, esi=counter struct; both lost register args)
void sub_843F0(Env& e, P game, P unit, P aiTick, int unaffEBX, P ctr)
{
    P tbl = rd<P>(aiTick, 0x1c);
    P target = rd<P>(aiTick, 0x18);
    e.SetMode(unit, 0);
    if (rd<int32_t>(ctr, 0x14) < row(tbl, 0x56, game)) {
        if (!e.IsBoss(unit) && (int)(rnd(unit) % 100) < row(tbl, 0x62, game)) {
            struct { P t; int32_t best; } r{ target, 0 };
            e.FindTarget(&r, game, unit, 0);
            if (r.best < row(tbl, 0x5c, game) && e.TryCast(unit)) { wr<int32_t>(ctr, 0x14, rd<int32_t>(ctr, 0x14) + 1); return; }
        }
    }
    if (unaffEBX) {
        if ((int)(rnd(unit) % 100) < row(tbl, 0x68, game)) { e.SetMode(unit, 0); sendFlags(e, game, unit, 0, 4, 0x104, false); return; }
        e.Wait(row(tbl, 0x7a, game));
        return;
    }
    if (rd<int32_t>(ctr, 0x14) < row(tbl, 0x56, game) && row(tbl, 0x6e, game) <= (int)(rnd(unit) % 100)) {
        if ((int)(rnd(unit) % 100) < row(tbl, 0x74, game)) { setAnim(unit, coin(unit), 4); e.Attack(game, unit, target, 2, 0, 0, 1); return; }
        e.Wait(0xf);
        return;
    }
    e.Attack(game, unit, target, 2, 0, 0, 1);
}

// 1.13c D2Game+0x84800
void sub_84800(Env& e, P game, P unit, P aiTick)
{
    P target = rd<P>(aiTick, 8), tbl = rd<P>(aiTick, 0x1c);
    if (rd<int32_t>(aiTick, 0x18)) {
        if ((int)(rnd(unit) % 100) < row(tbl, 0x5c, game)) { e.SetMode(unit, 0); sendFlags(e, game, unit, 0, 4, 0x104, false); return; }
        e.Wait(row(tbl, 0x68, game));
        return;
    }
    if ((int)(rnd(unit) % 100) < row(tbl, 0x56, game)) { wr<int32_t>(mdata(unit), 0x1c, 0x4b); e.Approach(target, unit, game); return; }
    struct { P t; int32_t dist; } r{ 0, 0x7fffffff };
    e.FindTarget(&r, game, unit, 0x6fcbca00);
    int a = row(tbl, 0x62, game);
    if (r.t && a * a < r.dist) { wr<int32_t>(mdata(unit), 0x1c, 0x4b); e.Attack(game, unit, r.t, 2, 0, 0, 1); return; }
    if (rnd(unit) % 100 < 0x19) { setAnim(unit, coin(unit), 3); e.Attack(game, unit, target, 2, 0, 0, 1); return; }
    e.Wait(row(tbl, 0x68, game));
}

// 1.13c D2Game+0x864A0
void sub_864A0(Env& e, P game, P unit, P aiTick)
{
    int32_t w18 = rd<int32_t>(aiTick, 0x18);
    uint32_t dist = rd<uint32_t>(aiTick, 0x14);
    P tbl = rd<P>(aiTick, 0x1c);
    uint32_t q[3] = { dist, rd<uint32_t>(aiTick, 8), (uint32_t)w18 };
    int tgt = e.TargetDist(game, q);  // GUESS: result is target ptr/id; q[0] updated dist
    dist = q[0];
    if (!tgt) {
        if (rnd(unit) % 100 < 0x32) { setAnim(unit, coin(unit), 3); e.Attack(game, unit, q[1], 2, 0, 0, 1); return; }
        e.Wait(row(tbl, 0x62, game));
        return;
    }
    if (w18 == 0 && unit && rd<int32_t>(unit, 0) == 1 && rd<P>(unit, 0x14)) {
        int m = rd<int32_t>(rd<P>(unit, 0x14), 0x54);
        if (m == 3 || m == 0x13) { e.SetMode(unit, 0); sendFlags(e, game, unit, 4, 4, 0x104, false); return; }
    }
    if ((int)dist < 6 && (int)(rnd(unit) % 100) < row(tbl, 0x68, game)) {
        wr<int32_t>(mdata(unit), 0x1c, 100);
        if (e.Walk(game, 0, 0xc)) return;
    }
    int16_t s80 = row(tbl, 0x80, game);
    if (s80 > 0 && s80 < (int)dist && (int)e.Roll100((uint64_t*)(unit + 0x20)) < row(tbl, 0x56, game)) {
        wr<int32_t>(mdata(unit), 0x1c, 10);
        e.Special(tgt, unit, game, rd<uint8_t>(tbl, 0x80 + rd<uint8_t>(game, 0x6d) * 2));
        return;
    }
    if (row(tbl, 0x6e, game) < (int)dist) {
        wr<int32_t>(mdata(unit), 0x1c, 100);
        uint8_t c = rd<uint8_t>(tbl, 0x6e + rd<uint8_t>(game, 0x6d) * 2);
        e.Attack(game, unit, tgt, 0xf, 0, 0, c ? c : 1);
        return;
    }
    if ((int)(rnd(unit) % 100) >= row(tbl, 0x5c, game)) { e.Wait(row(tbl, 0x62, game)); return; }
    if (rd<int16_t>(tbl, 0x172) >= 0 && (int)e.Roll100((uint64_t*)(unit + 0x20)) < row(tbl, 0x74, game)) { e.Circle(unit); return; }
    if (rd<int16_t>(tbl, 0x174) >= 0 && (int)e.Roll100((uint64_t*)(unit + 0x20)) < row(tbl, 0x7a, game)) { e.Circle(unit); return; }
    if (rd<int16_t>(tbl, 0x170) >= 0) { e.Circle(unit); return; }
    e.Flee(game, tgt);
}

// 1.13c D2Game+0x874A0  (ebx=distance bucket, retaddr slot=target; lost register args)
void sub_874A0(Env& e, P game, P unit, P aiTick, int unaffEBX, P target)
{
    int32_t w18 = rd<int32_t>(aiTick, 0x18);
    P tbl = rd<P>(aiTick, 0x1c);
    if (!w18 && unit && rd<int32_t>(unit, 0) == 1 && rd<P>(unit, 0x14)) {
        int m = rd<int32_t>(rd<P>(unit, 0x14), 0x54);
        if (m == 3 || m == 0x13) { e.SetMode(unit, 0); sendFlags(e, game, unit, 5, 5, 0x104, false); return; }
    }
    int st = e.GetStat(unit);
    uint32_t q[3] = { (uint32_t)unaffEBX, 0, 0 };
    if (row(tbl, 0x56, game) <= st) {
        if (w18) { e.SetMode(unit, 0); sendFlags(e, game, unit, 0, 4, 0x104, false); return; }
        if (unaffEBX < 0xf && e.TargetDist(game, q) && (int)e.Roll100((uint64_t*)(unit + 0x20)) < row(tbl, 0x62, game)) { e.Flee(game, target); return; }
        e.Attack(game, unit, target, 2, 0, 0, 1);
        return;
    }
    if (unaffEBX < 3) {
        wr<int32_t>(mdata(unit), 0x1c, 0x32);
        if (!e.Walk(game, unit, 5)) e.Flee(game, target);
        return;
    }
    if (unaffEBX > 0xf) { e.Attack(game, unit, target, 2, 0, 0, 6); return; }
    if (e.TargetDist(game, q) && (int)e.Roll100((uint64_t*)(unit + 0x20)) < row(tbl, 0x68, game)) { e.Flee(game, target); return; }
    if (row(tbl, 0x5c, game) <= (int)e.Roll100((uint64_t*)(unit + 0x20))) { e.Wait(10); return; }
    e.Strafe(unit, game, target, 3);
}

// 1.13c D2Game+0x877B0  (param_3 is the AI-tick struct; param_2 the unit; param_1 the game/monster)
void sub_877B0(Env& e, P game, P unit, P aiTick)
{
    P cnt = rd<P>(aiTick, 0x14);   // GUESS: decompile treats this as a scalar (int)param_3[5]
    P tbl = rd<P>(aiTick, 0x1c);
    P st = rd<P>(aiTick, 0);
    int kind = unit ? rd<int32_t>(unit, 0x10) : 0;
    if (rd<int32_t>(st, 0x14) < 1) {
        wr<int32_t>(st, 0x14, row(tbl, 0x5c, game) + rd<int32_t>(game, 0xa8));
        wr<int32_t>(st, 0x18, row(tbl, 0x56, game));
    }
    if (kind == 1 || kind == 10) {
        if (kind == 1 && (int)cnt > 5) { e.Wait(5); return; }
        e.SetMode(unit, 0);
        Pkt p{}; p.type = kind == 1 ? 10 : 0xb; p.unit = unit;
        uint8_t v = 100;
        P c = unit ? e.GetClientData(unit) : 0;
        if (c && (rd<uint8_t>(c, 0x105) & rd<uint32_t>(e.gdwBitMasks, kind == 1 ? 8 : 12))) v = 0x65;
        p.c = (p.c & ~0xff00u) | ((uint32_t)v << 8);
        p.a = p.b = 0;
        e.SendPkt(game, &p);
        e.Wait2(0x14);
        return;
    }
    if (kind == 0xb && rd<int32_t>(st, 0x18) > 0) {
        if (rd<int32_t>(st, 0x14) < rd<int32_t>(game, 0xa8)) {
            wr<int32_t>(st, 0x14, row(tbl, 0x5c, game) + rd<int32_t>(game, 0xa8));
            int x = 0, y = 0, z = 0, w = 0; P pp = 0;
            e.GetXYZ(unit, &x, &y, &z, &pp, &w, 0, 0);
            e.GetDir(unit);
            P sk = e.GetMissileSkill();
            P m = sk ? e.Missile(x, w, game, sk, y, pp, 2, 0x42) : 0;  // GUESS arg order
            if (m) {
                wr<int32_t>(st, 0x18, rd<int32_t>(st, 0x18) - 1);
                wr<uint32_t>(m, 0xc4, rd<uint32_t>(m, 0xc4) | 0x4020000);
                if (unit && rd<int32_t>(unit, 4) == 0x2c7) { e.SetStat(m, 0xb8, 1); e.SetState(m, 0xca, 0); }
            }
        }
        e.Wait2(row(tbl, 0x5c, game));
        return;
    }
    e.SetMode(unit, 0);
    Pkt p{}; p.unit = unit; p.c = 0x6400;
    e.SendPkt(game, &p);
}

}  // namespace d2game_batch_843f0
