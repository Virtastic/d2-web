// Implementation batch 452c0b/c: summon / spawn / skill-use glue in 0x452C0-0x51A90.
// Callees by retail ordinal (oNNNNN) or D2Game RVA (r_NNNN), untyped variadic C functions.
// Hidden register inputs are written /*eax*/ /*ecx*/ /*edx*/ /*esi*/ /*edi*/ /*ebx*/ before the stack args;
// every argument and register was read from the 1.13c disassembly (the decompile misses stack slots).
// Not yet linked or vectored (RE-DONE only).
#include <cstdint>
#include <cstdlib>
using i32 = int32_t; using u32 = uint32_t; using P = intptr_t; using u8 = uint8_t; using i16 = int16_t; using u16 = uint16_t;

extern "C" {
P o10786(...); P o10551(...); P o10887(...); P o10188(...); P o10973(...); P o11013(...); P o10807(...);
P o10392(...); P o10444(...); P o10830(...); P o10819(...); intptr_t o10945(...); P o10913(...); P o10867(...);  // o10945 takes (unit pointer, state, on)
P o10750(...); P o10920(...); P o10076(...); P o10511(...); P o10653(...); P o10554(...); P o10748(...);
P o10243(...); P o10637(...); P o10210(...); P o10885(...); P o10331(...); P o10865(...); P o10689(...);
P o10302(...); P o10418(...); P o10045(...); P o10619(...); P o10660(...); P o10204(...); P o10265(...);
P o10024(...); P o10785(...); P o11073(...); P o10123(...); P o10589(...); P o10509(...); P o10507(...); P o10978(...);
P o11103(...); P o11089(...); P o10704(...); P o10224(...);
P r_1020(...); P r_1100(...); P r_1130(...); P r_1180(...); P r_10D0(...); P r_11D0(...); P r_13B0(...); P r_14D0(...);
P r_1560(...); P r_15D0(...); P r_1850(...); P r_1220(...); P r_1EC0(...); P r_24BF0(...); P r_34660(...); P r_35030(...);
P r_37110(...); P r_37190(...); P r_370E0(...); P r_372C0(...); P r_37680(...);
P r_47450(...); P r_49A80(...); P r_4C220(...); P r_4C9E0(...); P r_4DF20(...); P r_4DF60(...); P r_4DF80(...);
P r_4E180(...); P r_4E230(...); P r_4E2A0(...); P r_4EB50(...); P r_4EC30(...); P r_4F450(...); P r_4F970(...);
P r_500A0(...); P r_509E0(...); P r_50800(...); P r_60390(...);
P r_6F930(...); P r_75E10(...); P r_77240(...); P r_78A90(...); P r_79100(...); P r_7A190(...);
P r_8AD40(...); P r_8B3D0(...); P r_8E6F0(...); P r_8EA80(...); P r_91340(...); P r_92ED0(...); P r_96FA0(...);
P r_97F50(...); P r_9E290(...); P r_9E450(...); P r_9E490(...); P r_9EB80(...); P r_9EE50(...); P r_9F120(...);
P r_9F210(...); P r_9F730(...); P r_9FA30(...); P r_A00B0(...); P r_A0AD0(...); P r_A0C70(...); P r_A0F40(...);
P r_A0FC0(...); P r_A0630(...); P r_A1790(...); P r_A2100(...); P r_A3610(...); P r_AF920(...);
P r_BD330(...); P r_DC530(...); P r_DDDE0(...); P r_DE5A0(...); P r_DED30(...); P r_DFA90(...); P r_DFAE0(...);
P r_E03A0(...); P r_E07A0(...); P r_E0A60(...); P r_E19C0(...); P r_E1D90(...); P r_E20A0(...);
P r_EF510(...); P r_EF570(...); P r_EFE40(...);
}
extern char** sgptDataTables_exref;
extern uint8_t gdwBitMasks_exref[];
extern P DAT_6fd277a8[];               // per-id handler table (0x32 entries)
extern void (*DAT_6fd2e550[])(P, i32, i32); // per-id callback table (real ABI: ecx, edx, stack 1)
extern const char DAT_6fd19ff9[];

static inline P DT() { return (P)*sgptDataTables_exref; }
static inline i32& W(P p, i32 o) { return *(i32*)(p + o); }
static inline u32& U(P p, i32 o) { return *(u32*)(p + o); }
static inline u8& B(P p, i32 o) { return *(u8*)(p + o); }
static inline i16& S(P p, i32 o) { return *(i16*)(p + o); }
static inline P& PP(P p, i32 o) { return *(P*)(p + o); }
static inline i32 sdt(i32 o) { return W(DT(), o); }
// skill-table row (0x23c bytes at +0xb98, count at +0xba0); 0 when out of range
static inline P skrow(i32 id) { return (id < 0 || id >= sdt(0xba0)) ? 0 : PP(DT(), 0xb98) + (P)id * 0x23c; }
// type 2 / 4 / 5 units keep their size pair at +0x2c -> (+0xc, +0x10); others ask the path object
static inline bool isMobile(P u) { i32 t = W(u, 0); return t == 2 || (t > 3 && t < 6); }
static inline i32 unitW(P u) { if (isMobile(u)) return W(PP(u, 0x2c), 0xc); return PP(u, 0x2c) ? (i32)o10867(PP(u, 0x2c)) : 0; }
static inline i32 unitH(P u) { if (isMobile(u)) return W(PP(u, 0x2c), 0x10); return PP(u, 0x2c) ? (i32)o10750(PP(u, 0x2c)) : 0; }
static inline bool skValid(i16 id) { return id >= 0 && id < sdt(0xbd4); }
static inline P missRow(i32 id) { return (id < 0 || id >= sdt(0xbd4)) ? 0 : PP(DT(), 0xbcc) + (P)id * 0x144; }

// 1.13c D2Game+0x452C0 (ecx = game, edx = owner; stack: skill, extra)
i32 D2GAME_452C0(P game, P owner, i32 skill, i32 extra)
{
    i32 p3 = skill, v68 = 0;
    if (!skrow(skill)) return 0;
    i32 slots = (i32)r_91340(/*eax*/skill, owner);
    if (slots < 1) return 0;
    if (!r_9FA30(/*ecx*/game, /*eax*/owner, /*ebx*/&v68, &p3)) return 0;
    if (!r_A0630(/*eax*/slots, game, owner)) return 0;
    if (owner) U(owner, 0xc4) |= 0x40;
    u32 roll = (u8)r_1180(/*ecx*/owner + 0x20, /*eax*/0x100);
    i32 rec[0x17] = {};
    rec[0] = 1; rec[1] = (i32)owner; rec[4] = slots; rec[5] = v68; rec[6] = p3; rec[0xb] = skill; rec[0xc] = extra;
    P unit = r_6F930(/*ecx*/game, /*edx*/rec);
    if (!unit) return 0;
    o10637(unit, roll);
    r_7A190(skill, extra, v68, p3, roll);
    return 1;
}


// 1.13c D2Game+0x463F0 (stack: game, skill, lvl; esi = owner)
i32 D2GAME_463F0(P game, i32 skill, i32 lvl, P owner /*esi*/)
{
    P row = skrow(skill);
    if (!row) return 0;
    i32 blk[10] = {};   // blk[0] = 9EE50 out; blk[1..9] = the record handed to A3610
    i32 idx = (i32)r_9EE50(/*eax*/0, /*ecx*/0, /*edx*/&blk[0], owner, 0, skill, lvl);
    if (idx < 0) return 0;
    i32 cls = (int8_t)B(row, 0xbe);
    if (cls < 0 || cls >= sdt(0xbf0)) return 0;
    if (owner) U(owner, 0xc4) |= 0x40;
    blk[2] = (i32)owner; blk[3] = idx; blk[5] = blk[0]; blk[8] = cls;
    blk[9] = (i32)o10786(owner, W(row, 0xc0), skill, lvl);
    P mon = r_A3610(/*eax*/&blk[1], /*esi*/owner, game);
    if (!mon) return 0;
    r_EFE40(/*ecx*/game, /*ebx*/W(owner, 0xd0), 0);
    i32 lv = (i32)o10786(owner, W(row, 0x13c), skill, lvl);
    if (lv < 1) lv = 1;
    r_4E2A0(/*ecx*/lv, /*eax*/owner, game, mon, lvl);
    r_4F970(/*ebx*/lvl, /*edi*/mon, game, owner, mon, skill, lvl, 0);
    return 1;
}


// 1.13c D2Game+0x470F0 (ecx = this, eax = in_EAX, ebx = group idx; stack: a1, a2)
P D2GAME_470F0(P thisv, P eax, i32 ebx, P a1, P a2)
{
    P row = (ebx < 0 || ebx >= sdt(0xa80)) ? 0 : PP(DT(), 0xa78) + (P)ebx * 0x1a8;
    u8 lo = B(row, 0x2f), hi = B(row, 0x30);
    if (!lo || !hi || lo > hi) return 0;
    P pos = o10224(a2, thisv, eax);
    if (!pos) return 0;
    P m = r_EF510(/*ecx*/1, /*edx*/a2, /*eax*/ebx, a1, pos, thisv, eax, -1, 0);
    if (!m) return 0;
    u8 l = lo - 1, h = hi - 1;
    i32 n = (i32)r_11D0(/*ecx*/m + 0x20, /*esi*/(i32)(h - l + 1)) + l;
    for (; n > 0; --n) r_EF570(/*ecx*/1, /*eax*/ebx, /*edx*/m, a1, pos, 3, 0);
    return m;
}


// 1.13c D2Game+0x47700 (eax = in_EAX, ebx = group idx; stack: game, p2, minB, maxB)
P D2GAME_47700(P game, P p2, u32 minB, u32 maxB, P eax, i32 ebx)
{
    u8 lo = (u8)minB, hi = (u8)maxB;
    if (!lo || !hi || lo > hi) return 0;
    i32 out1 = 0, out2 = 0;   // out2 is the game stack slot in retail; game is kept in ebp
    if (!r_47450(/*ebp*/game, /*esi*/eax, /*edi*/p2, /*ebx*/ebx, &out1, &out2, 1)) return 0;
    P m = r_EF510(/*ecx*/1, /*edx*/eax, /*eax*/ebx, game, p2, out1, out2, -1, 0);
    if (!m) return 0;
    u8 l = lo - 1, h = hi - 1;
    if (r_13B0(/*eax*/ebx, /*edx*/0x12) && (i32)r_1850(/*eax*/ebx) == 0x210)
        r_E1D90(/*ecx*/2, /*edx*/0x232, r_1100(/*ecx*/m), r_1130(/*ecx*/m), game, eax, 1, 0, 0);
    i32 n = (i32)r_11D0(/*ecx*/m + 0x20, /*esi*/(i32)(h - l + 1)) + l;
    for (; n > 0; --n) r_EF570(/*ecx*/1, /*eax*/ebx, /*edx*/m, game, p2, 3, 0);
    return m;
}


// 1.13c D2Game+0x47880 (esi = unit, edi = game, ebx = group idx; stack: a1)
void D2GAME_47880(P unit, P edi, i32 ebx, P a1)
{
    if (unit && W(unit, 0) == 1 && PP(unit, 0x14) && (B(PP(unit, 0x14), 0x16) & 4)) {
        uint64_t seed = (uint64_t)U(unit, 0x20) * 0x6ac690c5ULL + U(unit, 0x24);
        *(uint64_t*)(unit + 0x20) = seed;
        for (i32 n = (i32)((u32)seed % 3) + 1; n > 0; --n)
            if (r_EF570(/*ecx*/1, /*eax*/ebx, /*edx*/unit, edi, a1, 4, 0)) r_24BF0(/*ecx*/edi, 0x10);
    }
}


// 1.13c D2Game+0x487A0 (eax = unit; stack: game, tgt, headSlot, a4)
i32 D2GAME_487A0(P unit /*eax*/, P game, P tgt, P* headSlot, i32 a4)
{
    P head = *headSlot;
    if (B(unit, 0x6c) && r_E03A0(/*ecx*/game, /*edx*/W(unit, 0x64), /*eax*/W(unit, 0x68))) return 0;
    if (!tgt || W(tgt, 0x10) == 0 || W(tgt, 0x10) == 0xc) return 0;
    if ((i32)r_37680() == 1) return 0;
    i32 t = W(tgt, 4);
    if (!r_15D0(/*eax*/t, /*edx*/9)) return 1;
    if (t == 0x92 && (r_60390(/*eax*/game) & 0xff)) return 0;
    for (P n = head; n; n = PP(n, 8))
        if (PP(n, 0) == unit) return 1;
    i32 v = (W(unit, 0) == 0) ? W(o10920(unit), 0x9c) : 0;
    P node = o10045(/*ecx*/PP(game, 0x1c), /*edx*/0xc, "..\\Source\\D2Game\\MONSTER\\MonsterAI.cpp", 0x1c7, 0);
    PP(node, 0) = unit;
    PP(node, 8) = *headSlot;
    *headSlot = node;
    r_BD330(/*eax*/tgt, r_49A80(/*eax*/v), v);
    r_DFAE0(/*eax*/unit, 1, W(tgt, 0xc));
    W(node, 4) = 0;
    P h = o10204(PP(game, 0x1c));
    r_34660(/*eax*/v, unit, tgt, h);
    u8 pkt[8] = {};      // 6-byte packet, then the word 10619 fills
    pkt[0] = 0x27; pkt[1] = (u8)W(tgt, 0); *(i32*)(pkt + 2) = W(tgt, 0xc);
    o10619(h, pkt + 6);
    r_8B3D0(/*eax*/pkt, v);
    o10660(h);
    r_35030(/*eax*/game, v);
    r_8AD40(/*cl*/1, /*edx*/W(tgt, 0xc), /*al*/0x28, v, r_1020(/*eax*/unit, game), a4);
    return 0;
}


// 1.13c D2Game+0x4C2A0 (ecx = game, edx = unit; stack: skill, lvl)
i32 D2GAME_4C2A0(P game, P unit, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!row) return 0;
    i32 s82 = S(row, 0x82), s54 = S(row, 0x54);
    if (s82 < 0 || s82 >= sdt(0xc4) || s54 < 0 || s54 >= sdt(0xbd4)) return 0;
    i32 C[6];            // the record the 4C220 callback reads (retail keeps it in the frame)
    C[0] = skill; C[1] = lvl;
    C[2] = (i32)o10786(unit, W(row, 0x60), skill, lvl);
    C[3] = s82; C[4] = s54;
    C[5] = (i32)o10786(unit, W(row, 0x68), skill, lvl);
    i32 res3 = (i32)o10786(unit, W(row, 0x64), skill, lvl);
    r_A0C70(/*eax*/game, unit, 0, 0, res3, W(row, 0x50), (void*)r_4C220, C, 1);
    return 1;
}


// 1.13c D2Game+0x4C720 (ecx = game, edx = unit; stack: skill, lvl)
i32 D2GAME_4C720(P game, P unit, i32 skill, i32 lvl)
{
    r_E07A0(/*eax*/unit);
    P owner = o10392(W(unit, 0x2c));
    if (owner == unit || !owner) return 0;
    P row = skrow(skill);
    if (!row) return 0;
    i32 rec[0x1c] = {};
    u16 f = (u16)r_DE5A0(/*eax*/owner, /*edi*/unit, game, 0, 0);
    *(u16*)&rec[1] = f;
    if (f & 1) {
        rec[3] = (i32)o10786(unit, W(row, 0x138), skill, lvl);
        if (B(row, 0x1dc)) {
            i32 v = (i32)o10786(unit, W(row, 0x144), skill, lvl);
            rec[0x1a] = v;
            if (v > 0) ((u8*)rec)[0x65] = B(row, 0x1dc);
        }
        r_9F210(/*eax*/skill, /*edi*/unit, rec, lvl);
    }
    r_DDDE0(/*cl*/B(row, 0x1a5), /*eax*/unit, /*edi*/rec, game, owner);
    return 1;
}


// 1.13c D2Game+0x4C870 (ecx = game, edx = unit; stack: skill, lvl)
i32 D2GAME_4C870(P game, P unit, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!row) return 0;
    P X = o10511(unit);
    if (!X) return 0;
    if (!r_9EB80(/*eax*/unit)) {
        r_79100(/*eax*/unit, game);
        r_78A90(game);
        return 0;
    }
    i32 uv = (i32)o10786(unit, W(row, 0x64), skill, lvl);
    r_E07A0(/*eax*/unit, /*ecx*/game);
    P p = o10392(W(unit, 0x2c));
    P q = (p == unit) ? 0 : p;
    i32 Y = 0;           // the local retail hands A0F40 by address; A0FC0 returns it
    P rec;
    if (!q) rec = r_A0F40(/*eax*/3, /*ecx*/-1, /*edx*/uv, /*edi*/&Y, game, unit, 0, 0);
    else { Y = (i32)r_A0FC0(/*eax*/3, /*ecx*/q, /*edx*/uv, /*edi*/q, game, unit, 0, 0); rec = q; }
    i32 v140 = (i32)o10786(unit, W(row, 0x140), skill, lvl);
    i32 v138 = (i32)o10786(unit, W(row, 0x138), skill, lvl);
    i32 c = (v140 > v138) ? v138 : v140;
    if (Y > c) c = Y;
    o10243(X, (c < v138) ? c : v138);
    i32 a;
    if (!rec) { o10554(X, 1); a = -1; } else { o10554(X, W(rec, 0)); a = W(rec, 0xc); }
    o10748(X, a);
    r_A1790(game);
    return 1;
}


// 1.13c D2Game+0x4CB00 (ecx = game, edx = unit; stack: skill, lvl)
i32 D2GAME_4CB00(P game, P unit, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!row) return 0;
    r_E07A0(/*eax*/unit, /*ecx*/game);
    P owner = o10392(W(unit, 0x2c));
    if (owner == unit || !owner) return 0;
    i32 rec[0x1c] = {};
    i32 type = W(unit, 0);
    i32 h = (i32)o10653(unit, skill, lvl);
    if (type == 1) r_77240(unit, 8);
    u16 f = (u16)r_DE5A0(/*eax*/owner, /*edi*/unit, game, h, 0);
    *(u16*)&rec[1] = f;
    if (f & 1) {
        rec[3] = (i32)o10786(unit, W(row, 0x138), skill, lvl);
        if (B(row, 0x1dc)) {
            i32 v = (i32)o10786(unit, W(row, 0x144), skill, lvl);
            rec[0x1a] = v;
            if (v > 0) ((u8*)rec)[0x65] = B(row, 0x1dc);
        }
        r_9F210(/*eax*/skill, /*edi*/unit, rec, lvl);
    }
    u8 b = B(row, 0x1a5);
    if (!b) b = 0x80;
    r_DDDE0(/*cl*/b, /*eax*/unit, /*edi*/rec, game, owner);
    r_DED30(game, unit, owner);
    return 1;
}


// 1.13c D2Game+0x4CC50 (ecx = game, edx = unit; stack: skill, lvl)
i32 D2GAME_4CC50(P game, P unit, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!row) return 0;
    r_E07A0(/*eax*/unit, /*ecx*/game);
    P owner = o10392(W(unit, 0x2c));
    if (owner == unit || !owner) return 0;
    if (!r_E0A60(/*eax*/owner, /*ecx*/unit, game)) return 0;
    i32 rec[0x1c] = {};
    i32 h = (i32)o10653(unit, skill, lvl);
    u16 f = (u16)r_DE5A0(/*eax*/owner, /*edi*/unit, game, h, 0);
    *(u16*)&rec[1] = f;
    if (f & 1) {
        i32 a = (i32)o10786(unit, W(row, 0x138), skill, lvl);
        rec[2] = (i32)r_DC530(/*eax*/a, /*ecx*/0, /*edx*/unit, 1, 0, 0, 0, (u32)B(row, 0x1a5));
        if (B(row, 0x1dc)) {
            i32 v = (i32)o10786(unit, W(row, 0x144), skill, lvl);
            rec[0x1a] = v;
            if (v > 0) ((u8*)rec)[0x65] = B(row, 0x1dc);
        }
        r_9F210(/*eax*/skill, /*edi*/unit, rec, lvl);
        rec[0] = 1;
        P c = r_372C0(unit);
        if (c && o10865(c)) {
            i32 t = (i32)o10786(unit, W(row, 0x13c), skill, lvl);
            i32 u = (i32)o10786(unit, W(row, 0x140), skill, lvl);
            r_4C9E0(/*eax*/t, /*ecx*/c, /*edx*/unit, game, u);
        }
    }
    r_DDDE0(/*cl*/0x80, /*eax*/unit, /*edi*/rec, game, owner);
    return 1;
}


// 1.13c D2Game+0x4D420 (ecx = game, edx = unit; stack: skill, lvl; ret 8)
i32 D2GAME_4D420(P game, P unit, i32 skill, i32 lvl)
{
    if (unit) U(unit, 0xc4) |= 0x40;
    P row = skrow(skill);
    if (!row) return 0;
    i32 tx, ty;   // out-params of 9FA30: ty via stack arg, tx via ebx
    if (!r_9FA30(/*eax*/unit, /*ebx*/&tx, &ty)) return 1;
    i32 n = (i32)o10786(unit, W(row, 0x138), skill, lvl);
    i32 d[2];     // d[0] = dx (esp+0x10), d[1] = dy (esp+0x14)
    d[0] = tx - unitW(unit);
    d[1] = ty - unitH(unit);
    r_9E490(/*edx*/&d[1], /*esi*/&d[0]);
    r_9E450(/*ebx*/&d[0], /*edi*/&d[1]);   // edi = edx left by 9E490
    i32 x = tx + -((d[0] * n) / 2);
    i32 y = ty + -((d[1] * n) / 2);
    i32 mid = (i16)S(row, 0x48);
    if ((i32)o10076(/*ecx*/unit) != 1 && S(row, 0x4a) >= 0) mid = S(row, 0x4a);
    if (mid < 0 || mid >= sdt(0xb6c)) return 0;
    i32 w = (i32)o10786(unit, W(row, 0x140), skill, lvl);
    if (!w) w = n;
    i32 s[0x17] = {};
    s[1] = (i32)unit; s[2] = (i32)unit; s[4] = mid; s[0xb] = skill; s[0xc] = lvl;
    s[0x11] = (i32)o10786(unit, W(row, 0x13c), skill, lvl);
    i32 head = (n - w) / 2;
    i32 runs[3] = { head, w, n - head - w };
    u32 fl[3] = { 0x10820, 0x820, 0x10820 };
    for (int r = 0; r < 3; ++r) {
        s[0] = fl[r];
        for (i32 k = runs[r]; k > 0; --k) {
            s[7] = x; s[8] = y;
            r_6F930(/*ecx*/game, /*edx*/s);
            x += d[0]; y += d[1];
        }
    }
    return 1;
}



// 1.13c D2Game+0x4D710 (ecx = game, edx = unit; stack: skill, lvl; ret 8)
i32 D2GAME_4D710(P game, P unit, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!row || !unit || W(unit, 0) != 0) return 0;
    // 9EE50 writes its out value through edx = &lvl (the stack arg slot is reused as out-param)
    i32 lvlSlot = lvl;
    i32 r = (i32)r_9EE50(/*eax*/0, /*ecx*/0, /*edx*/&lvlSlot, unit, 0, skill, lvl);
    if (r < 0) return 0;
    u8 cls = B(row, 0xbe);
    if (cls >= sdt(0xbf0)) cls = 0;
    U(unit, 0xc4) |= 0x40;
    i32 t[9] = {};                    // E-0x24.. : t[2] = r, t[1] = unit, t[4] = out, t[7] = class, t[8] = hp
    t[1] = (i32)unit; t[2] = r; t[4] = lvlSlot; t[7] = cls;
    t[8] = (i32)o10786(unit, W(row, 0xc0), skill, lvl);
    P mon = r_A3610(/*eax*/t, game);
    if (!mon) return 0;
    r_4E2A0(/*ecx*/0, /*eax*/unit, game, mon, lvl);
    i32 v = (i32)o10786(unit, W(row, 0x13c), skill, lvl);
    r_4F970(game, unit, mon, skill, lvl, v);   // ebx = lvl, edi = game at the call
    for (char* n = (char*)PP(mon, 0xdc); n;) {
        char c = *n; char* nx = *(char**)(n + 0x24);
        if (c == 2) r_8E6F0(/*eax*/n);
        n = nx;
    }
    r_8EA80(/*edx*/mon, /*ecx*/game, 2, W(PP(game, 0xa8), 0x14) /* address game[0xa8]+0x14 */, 0, 0, 0);
    o10945((void*)mon, 0x5d, 1);
    o10210(mon, unit);
    r_EFE40(/*ecx*/game, 0, /*edi*/mon, /*ebx*/B(unit, 0xd0));
    return 1;
}



// 1.13c D2Game+0x4E180 (eax = owner unit, edi = source unit or null; no stack args, plain ret)
void D2GAME_4E180(P owner /*eax*/, P src /*edi*/)
{
    i32 v = (i32)o10973(owner, 0x15d, 0);
    if (!v) return;
    i32 a = 0, t = 6, c = -1;
    if (src) { a = W(src, 8); t = W(src, 0); c = W(src, 0xc); }
    P sl = o11013(a, 0, 0, t, c);
    if (!sl) return;
    o10807(src, sl, 1);
    if ((i32)o10973(src, 0x8e, 0) <= 0) o10188(sl, 0x27, v, 0);
    if ((i32)o10973(src, 0x90, 0) <= 0) o10188(sl, 0x29, v, 0);
    if ((i32)o10973(src, 0x94, 0) <= 0) o10188(sl, 0x2b, v, 0);
    o10188(sl, 0x2d, v, 0);
}


// 1.13c D2Game+0x4E2A0 (ecx = level, eax = owner unit; stack: game, mon, bonus; ret 0xc)
i32 D2GAME_4E2A0(i32 lvl /*ecx*/, P owner /*eax*/, P info /*game*/, P unit /*mon*/, i32 bonus)
{
    bool apply = true;
    if (lvl < 1) {
        i32 cur = (i32)o10973(owner, 0xc, 0);
        lvl = (cur * 3 + ((cur * 3 >> 31) & 3)) / 4 + bonus; // (x + (x>>31 & 3)) >> 2
        if (lvl < 2) lvl = 1;
        if (cur <= lvl) lvl = cur;
        if (lvl == 0) apply = false;
    }
    if (apply) o10887(unit, 0xc, lvl, 0);
    i32 cnt = sdt(0xb74), m = cnt - 1;
    if (lvl < m) m = lvl;
    if (m < 0 || m >= cnt) return 0;
    P r = PP(DT(), 0xb70) + (P)m * 0x78;
    if (!r) return 0;
    u32 a = B(info, 0x6d);
    if (a != 0 && a > 1) a = 2;
    i32 b = (B(info, 0x6a) == 0 && W(info, 0x74) == 0) ? 0 : 1;
    o10551(unit, 0x1f, W(r, (a + b * 3) * 4), 0);
    o10551(unit, 0x13, W(r, a * 4 + (b + 2) * 0xc), 0);
    return 1;
}

// 1.13c D2Game+0x4EA30 (ecx = value, eax = unit, esi = game; no stack args, plain ret; returns value)
i32 D2GAME_4EA30(i32 value /*ecx*/, P unit /*eax*/, P game /*esi*/)
{
    if (!unit) return value;
    P ud = o10920(unit);
    if (!ud || sdt(0xbf0) <= 3 || !PP(ud, 0x44)) return value;
    P pn = *(P*)PP(ud, 0x44) + 0x24;
    P node = pn ? PP(pn, 0) : 0; // *(**(ud+0x44) + 0x24)
    if (!node) return value;
    while (B(node, 0) & 1) { node = PP(node, 0x14); if (!node) return value; }
    u32 id = U(node, 4);
    if (!game) { o10024(DAT_6fd19ff9, (i32)o10265(0x2d3)); std::_Exit(-1); }
    P u = PP(game, 0x1320 + (id & 0x7f) * 4);
    if (!u) return value;
    while (U(u, 0xc) != id) { u = PP(u, 0xe4); if (!u) return value; }
    if (W(u, 0) != 1 || W(u, 4) != 0x122) return value;
    if (value) {
        P s = r_1220(/*eax*/(P)0x122);
        if (!s || S(s, 0x170) < 0) return value;
        i32 prod = (i32)r_1EC0(/*eax*/(i32)S(s, 0x170)) * value;
        i32 q = prod / 100;
        return value - (i32)r_4DF20(/*eax*/q, /*ebx*/u);
    }
    o10887(u, 6, o10913(u), 0);
    return value;
}


// 1.13c D2Game+0x4ECD0 (esi = unit; stack: a1, a2; ret 8; returns bool)
bool D2GAME_4ECD0(P unit /*esi*/, P a1, P a2)
{
    if (!unit) return false;
    if (W(unit, 0) == 1) {
        if (!r_1560(/*eax*/W(unit, 4), /*edx*/2)) return false;
        P p = PP(unit, 0x14);
        if (p && (B(p, 0x16) & 0x20)) return false;
    }
    u32 fl = U(unit, 0xc4);
    if (!(fl >> 2 & 1) || !(fl >> 3 & 1) || !(fl >> 1 & 1)) return false;
    if (r_DFA90(/*eax*/unit)) return false;
    return r_E0A60(/*eax*/unit, /*ecx*/a2, a1) != 0;
}


// 1.13c D2Game+0x4F970 (no register inputs; stack: game, unit, mon, skill, lvl, v; ret 0x18)
i32 D2GAME_4F970(P game, P unit, P mon, i32 skill, i32 lvl, i32 v)
{
    if (skill == 0) return 0;
    P row = skrow(skill);
    if (!row) return 0;
    i32 monId = mon ? W(mon, 0xc) : -1;
    const i16* ids = (const i16*)(row + 0x98);
    const u32* calcs = (const u32*)(row + 0xa4);
    for (int i = 0; i < 5; ++i) {
        i32 id = ids[i];
        if (id < 0 || id >= sdt(0xbd4)) continue;
        P mr = missRow(id);
        if (!mr) continue;
        o10551(mon, id, o10786(unit, calcs[i], skill, lvl), 0);
        if (S(mr, 0x48) > 0 && !r_370E0(/*eax*/mon, /*edx*/2)) {
            r_9E290(/*ecx*/monId, /*eax*/(i32)S(mr, 0x4c), /*edx*/2, /*esi*/mon, game, (i32)S(mr, 0x48), id << 16, 0);
            if (S(mr, 0x4a) > 0)
                r_9E290(/*ecx*/monId, /*eax*/(i32)S(mr, 0x4e), /*edx*/2, /*esi*/mon, game, (i32)S(mr, 0x4a), id << 16, 0);
        }
    }
    P slot = 0;
    const i16* ids2 = (const i16*)(row + 0x54);
    const u32* calcs2 = (const u32*)(row + 0x68);
    for (int i = 0; i < 6; ++i) {
        i32 id = ids2[i];
        if (id < 0 || id >= sdt(0xbd4)) continue;
        P mr = missRow(id);
        if (!mr) continue;
        i32 val = (i32)o10786(unit, calcs2[i], skill, lvl);
        if (!val) continue;
        if (!slot) {
            slot = o11013(W(game, 0x1c), 0, 0, mon ? W(mon, 0) : 6, monId);
            if (!slot) return 0;
            o10807(mon, slot, 1);
        }
        o10188(slot, id, val, 0);
        i16 si = S(mr, 0x48);
        if (si <= 0) continue;
        bool found = false;
        if (mon) {
            for (P n = PP(mon, 0x90); n; n = PP(n, 0x1c))
                if (W(n, 4) == 2 && W(n, 8) == W(mon, 0xc) && W(n, 0xc) == id) { found = true; break; }
            if (found) continue;
        }
        i32 k = S(mr, 0x4c);
        if (k >= 0 && k < 0x32 && DAT_6fd277a8[k])
            r_37190(/*esi*/mon, game, (i32)si, id << 16, 0, DAT_6fd277a8[k], 2, monId);
        i16 dx = S(mr, 0x4a);
        if (dx > 0) r_9E290(/*ecx*/monId, /*eax*/(i32)S(mr, 0x4e), /*edx*/2, /*esi*/mon, game, (i32)dx, id << 16, 0);
    }
    i16 s80 = S(row, 0x80);
    if (s80 > 0 && s80 <= sdt(0xc4)) {
        o10945((void*)mon, s80, 1);
        if (slot) o10785(slot, (i32)S(row, 0x80));
    }
    i32 calc = (i32)o10786(unit, W(row, 0x138), skill, lvl);
    i32 hp = (i32)o10913(mon);
    i32 add;
    if (hp > 0x100000) {
        if ((hp & ~0xf) < 0x640) add = (i32)((int64_t)hp * calc / 100);
        else add = (hp / 100) * calc;
    } else if (calc > 0x10000) {
        if ((calc & ~0xf) < 0x640) add = (i32)((int64_t)hp * calc / 100);
        else add = (calc / 100) * hp;
    } else add = (hp * calc) / 100;
    hp += add;
    o10887(mon, 7, hp, 0);
    o10887(mon, 6, hp, 0);
    const i16* gs = (const i16*)(row + 0xc4);
    const u32* gc = (const u32*)(row + 0xd0);
    for (int i = 0; i < 5; ++i) {
        i32 g = gs[i];
        if (g <= 0 || g >= sdt(0xba0)) continue;
        i32 n = (i32)o10786(unit, gc[i], skill, lvl);
        if (n <= 0) continue;
        P gr = skrow(g);
        if (!gr) continue;
        o10302(mon, g, n, 0, "..\\Source\\D2Game\\SKILLS\\Skills.cpp", 0x112a);
        o10418(mon);
        if (mon && W(mon, 0) == 0) r_96FA0(W(mon, 0x80), mon);
        if (gdwBitMasks_exref[0x14] & B(gr, 4)) r_A2100(/*ecx*/mon, /*edx*/0, (i32)gs[i], -1);
    }
    const i16* ps = (const i16*)(row + 0x84);
    if (ps[0] >= 0) {
        r_37110(/*edi*/mon, /*ebx*/game, 1, (i32)S(row, 0x80));
        for (int i = 0; i < 3; ++i) {
            if (ps[i] < 0) break;
            i32 k = ps[i + 3];
            if (k >= 0 && k < 0x32 && DAT_6fd277a8[k])
                r_37190(/*esi*/mon, game, (i32)ps[i], skill, lvl, DAT_6fd277a8[k], 1, (i32)S(row, 0x80));
        }
    }
    i16 sc = S(row, 0xe4);
    if (sc > 0 && sc < 0x2b && mon && W(mon, 0) == 1 && PP(mon, 0x14)) {
        P pd = PP(mon, 0x14);
        P tbl = pd + 0x1c;
        P cl = PP(game, 0xf0 + W(pd, 0x58) * 4);
        if (cl && !(B(pd, 0x16) & 8)) W(cl, 0x2c8) += 1;
        S(pd, 0x16) |= 8;
        int j = 0;
        while (j < 9 && B(tbl, j)) ++j;
        if (j < 9) {
            B(tbl, j) = (u8)sc;
            if (DAT_6fd2e550[sc]) DAT_6fd2e550[sc](mon, sc, 1);   // callee: ecx = mon, edx = id, stack 1
        }
    }
    i16 sk2 = S(row, 0xe6);
    if (sk2 > 0 && sk2 < sdt(0xbc0)) o10689(mon, (i32)sk2, 0);
    i32 out = v;
    if (v == 0) {
        i32 cur = (i32)o10973(unit, 0xc, 0);
        i32 cap = lvl * 3;
        if (cap <= 1) cap = 1;
        out = cap < cur ? cap : cur;
    }
    r_92ED0(/*eax*/lvl, /*ecx*/mon, game, unit, out, 0);
    return 1;
}


// 1.13c D2Game+0x50250 (ecx = unit, edx = effect record; no stack args, plain ret)
i32 D2GAME_50250(P unit /*ecx*/, i32* e /*edx*/)
{
    if (e[2]) {
        if (!unit || W(unit, 0) != 1 || o10830(unit) == 1) return 0;
        i32 k = e[0x13] == 0x17 ? 0xa : e[0x13] == 0x1b ? 0xc : (e[0x13] != 0x38 ? 0xb : 0);
        if (!r_9F120(/*eax*/unit, k)) return 0;
    }
    i32 first = 0;
    if (e[7] >= 0) {
        first = (i32)r_4E230(/*edi*/e[7], /*ebx*/e[0xd]);
        if (!first) return 0;
    }
    if (!D2GAME_4ECD0(unit, (P)e[0], (P)e[1])) return 0;
    i32 t[9] = {};
    t[0] = e[1]; t[1] = (i32)unit; t[2] = e[4]; t[3] = e[5]; t[4] = e[6]; t[5] = e[7];
    t[6] = first; t[7] = e[0x13];
    t[8] = e[2] ? (i32)(P)0x6fc70130 /* RVA 0x50130 callback */ : 0;
    P sl = r_A00B0(/*eax*/t);
    if (!sl) return 0;
    if (e[2]) r_500A0(/*eax*/e[4], /*edx*/unit, e[0]);
    for (int i = 0; i < 5; ++i) {
        i32 id = e[8 + i];
        if (id < 0 || id >= sdt(0xbd4)) continue;
        i32 v = (i32)r_4E230(/*edi*/id, /*ebx*/e[14 + i]);
        if (v) o10188(sl, id, v, 0);
    }
    if (e[3]) o10819(unit, "..\\Source\\D2Game\\SKILLS\\SkillNec.cpp", 0x246);   // string at 0x6fd1c1dc
    if (e[0x14] >= 0 && e[0x17] > 0) {
        r_37110(/*edi*/unit, /*ebx*/e[0], 1, e[0x13]);
        for (int i = 0; i < 3; ++i) {
            i32 k = e[0x17 + i];
            if (k >= 0 && k < 0x32 && DAT_6fd277a8[k])
                r_37190(/*esi*/unit, e[0], e[0x14 + i], e[4], e[5], DAT_6fd277a8[k], 1, e[0x13]);
        }
    }
    return 1;
}


// Shared slot loop of 505D0/51A90 (inline in both): i = 1..5 over the s16 ids at row+0x56; an unknown id
// stores -1 into rec[7+i] and ends the loop, a missing table row ends it without a store, a hit with
// (row[5] & gdwBitMasks[4]) sets rec[3]; rec[13+i] = 10786(unit, row dword i-1 at +0x6c, skill, lvl).
static void fillSlots(P unit, P row, i32 skill, i32 lvl, i32* rec)
{
    rec[3] = 0;
    for (int i = 1; i < 6; ++i) {
        i16 id = S(row, 0x56 + 2 * (i - 1));
        if (id < 0 || id >= sdt(0xbd4)) { rec[7 + i] = -1; return; }
        P p = r_10D0(/*eax*/(i32)id, /*ecx*/DT());
        if (!p) return;
        if (gdwBitMasks_exref[4] & B(p, 5)) rec[3] = 1;
        rec[7 + i] = id;
        rec[13 + i] = (i32)o10786(unit, W(row, 0x6c + 4 * (i - 1)), skill, lvl);
    }
}

// 1.13c D2Game+0x505D0 (ecx = game, edx = unit; stack: skill, lvl; ret 8)
i32 D2GAME_505D0(P game /*ecx*/, P unit /*edx*/, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!row || S(row, 0x54) < -1 || S(row, 0x54) >= sdt(0xbd4) || S(row, 0x82) < 0 || S(row, 0x82) >= sdt(0xc4)) return 0;
    if (unit) U(unit, 0xc4) |= 0x40;
    i32 g = (i32)r_4DF80(/*eax*/(i32)S(row, 0x82));
    i32 v1 = (i32)o10786(unit, W(row, 0x64), skill, lvl);
    i32 v2 = (i32)o10786(unit, W(row, 0x60), skill, lvl);
    if (g) v2 = (i32)r_4DF60(/*eax*/(i32)B(game, 0x6d), /*esi*/v2);
    i32 rec[0x1a] = {};
    rec[0] = (i32)game; rec[1] = (i32)unit; rec[2] = g; rec[4] = skill; rec[5] = lvl; rec[6] = v2;
    fillSlots(unit, row, skill, lvl, rec);
    rec[19] = S(row, 0x82); rec[20] = S(row, 0x84); rec[21] = S(row, 0x86); rec[22] = S(row, 0x88);
    rec[23] = S(row, 0x8a); rec[24] = S(row, 0x8c); rec[25] = S(row, 0x8e);
    return (i32)r_A0AD0(/*ecx*/game, W(row, 0x50), unit, v1, (void*)D2GAME_50250, rec);
}


// 1.13c D2Game+0x50800 (eax = unit, ecx = game; plain ret)
void D2GAME_50800(P game /*ecx*/, P unit /*eax*/)
{
    i32 h = unitH(unit), w = unitW(unit);
    o11073(o10331(unit, w, h, o10123(unit, 0x8000)));
    i32 l[10] = {};
    l[0] = unitW(unit);
    l[1] = unitH(unit);
    P t = o10509(o10331(unit, &l[2], o10589(unit, 0x3c01, 1)));
    if (!t) return;
    U(unit, 0xc4) |= 0x402000e;
    r_AF920(/*eax*/game, /*ebx*/unit);
    for (int k = 2; k < 10; k++) l[k] = 0;
    o11103(unit, 0);
    l[2] = 1; l[3] = (i32)(intptr_t)unit; l[7] = 0x6400;
    r_75E10(game, &l[2], 1);
    r_9F730(/*ebx*/unit);
    for (char* n = (char*)PP(unit, 0xdc); n;) {
        char c = *n; char* nx = *(char**)(n + 0x24);
        if (c == 8) r_8E6F0(/*eax*/n);
        n = nx;
    }
    if (o10507(unit)) o10978(unit, 0, -1);
    r_E19C0(/*eax*/l[1], /*ecx*/l[0], /*edx*/t, game, unit, 0, 0);
}


// 1.13c D2Game+0x51340 (ecx = game, edx = unit, stack skill, lvl; ret 8)
i32 D2GAME_51340(P game /*ecx*/, P unit /*edx*/, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!skill) return 0;
    i32 cls = B(row, 0xbe);
    if (cls >= sdt(0xbf0)) cls = 0;
    r_E07A0(/*eax*/unit, /*ecx*/game);
    P owner = o10392(PP(unit, 0x2c));
    if (owner == unit || !owner) return 0;
    if (!(r_4EC30(/*esi*/owner) & 0xff)) return 0;
    r_4F450(/*esi*/unit, /*ebx*/game);
    r_50800(/*ecx*/game, /*eax*/owner);
    i32 a = (B(game, 0x6a) == 0 && W(game, 0x74) == 0) ? 0 : 1;
    i32 z[14] = {};
    i32 base = W(owner, 4);
    // 6-arg call: the 3-arg stat getter plus the two args pushed under 11089
    P lv = o10973(owner, 0xc, 0);
    o11089(base, a, B(game, 0x6d), lv, 1, z);
    i32 n = z[1] - z[0] + 1;
    i32 hp = ((i32)(intptr_t)r_11D0(/*ecx*/owner + 0x20, /*esi*/n) + z[0]) << 8;
    o10887(owner, 7, hp, 0);
    o10887(owner, 6, hp, 0);
    i32 maxLv = (i32)(intptr_t)o10973(unit, 0xc, 0);
    u32 cur = (u32)(intptr_t)o10973(owner, 0xc, 0);
    if (cur != 0 && maxLv < (i32)cur) {
        i32 v = (i32)(intptr_t)r_14D0(/*eax*/maxLv, /*ecx*/cur, /*edx*/hp);
        if (v < 1) v = 1;
        o10887(owner, 7, v, 0);
        o10887(owner, 6, v, 0);
        o10887(owner, 0xc, maxLv, 0);
    }
    D2GAME_4F970(game, unit, owner, skill, lvl, 0);
    r_509E0(/*eax*/unit, game, owner, skill, lvl);
    i32 c = (i32)(intptr_t)o10786(unit, W(row, 0xc0), skill, lvl);
    r_97F50(/*eax*/cls, game, unit, owner, c);
    r_EFE40(/*ecx*/game, /*ebx*/PP(unit, 0xd0), /*edi*/owner, 0);
    o10704(owner);
    return 1;
}


// 1.13c D2Game+0x518B0 (ecx = game, edx = unit, stack skill, lvl; ret 8)
i32 D2GAME_518B0(P game /*ecx*/, P unit /*edx*/, i32 skill, i32 lvl)
{
    r_E07A0(/*eax*/unit, /*ecx*/game);
    P owner = o10392(PP(unit, 0x2c));
    if (owner == unit || !owner) return 0;
    if (!o10444(owner, 0)) return 0;
    r_4F450(/*esi*/unit, /*ebx*/game);
    P row = skrow(skill);
    if (!row) return 0;
    i32 outv = 0;   // edx = out pointer into the caller frame (one dword read back below)
    i32 idx = (i32)r_9EE50(/*eax*/0, /*ecx*/0, /*edx*/&outv, unit, 0, skill, lvl);
    if (idx < 0) return 0;
    i32 cls = (int8_t)B(row, 0xbe);
    if (cls < 0 || cls >= sdt(0xbf0)) return 0;
    i32 a = (i32)r_1100(/*ecx*/owner);
    i32 b = (i32)r_1130(/*ecx*/owner);
    o10885(o10331(owner, W(owner, 0), W(owner, 0xc)));
    r_E20A0(/*ecx*/game, /*edx*/owner);
    i32 t[9] = {};
    t[0] = 1; t[1] = (i32)unit; t[2] = idx; t[3] = 0; t[4] = outv; t[5] = a; t[6] = b; t[7] = cls;
    t[8] = (i32)(intptr_t)o10786(unit, W(row, 0xc0), skill, lvl);
    P mon = r_A3610(/*eax*/t, game);
    if (!mon) return 0;
    r_4E2A0(/*ecx*/0, /*eax*/unit, game, mon, lvl);
    r_4EB50(/*eax*/mon, /*ecx*/unit, skill, lvl);
    D2GAME_4F970(game, unit, mon, skill, lvl, 0);
    D2GAME_4E180(unit, mon);
    r_EFE40(/*ecx*/game, /*ebx*/PP(unit, 0xd0), 0);
    return 1;
}


// 1.13c D2Game+0x51A90 (ecx = game, edx = unit, stack skill, lvl; ret 8)
i32 D2GAME_51A90(P game /*ecx*/, P unit /*edx*/, i32 skill, i32 lvl)
{
    P row = skrow(skill);
    if (!row || S(row, 0x54) < -1 || S(row, 0x54) >= sdt(0xbd4) || S(row, 0x82) < 0 || S(row, 0x82) >= sdt(0xc4)) return 0;
    if (unit) U(unit, 0xc4) |= 0x40;
    i32 v1 = (i32)(intptr_t)o10786(unit, W(row, 0x64), skill, lvl);
    i32 v2 = (i32)(intptr_t)o10786(unit, W(row, 0x60), skill, lvl);
    i32 sc = (i32)(intptr_t)r_4DF60(/*eax*/(i32)B(game, 0x6d), /*esi*/v2);
    i32 rec[0x1a] = {};
    rec[0] = (i32)game; rec[1] = (i32)unit; rec[4] = skill; rec[5] = lvl; rec[6] = sc;
    fillSlots(unit, row, skill, lvl, rec);
    rec[19] = S(row, 0x82);
    // callback is D2Game+0x50B70 (not in this batch)
    return (i32)(intptr_t)r_A0AD0(/*ecx*/game, W(row, 0x50), unit, v1, (void*)(intptr_t)0x50B70 /*RVA*/, rec);
}
