// src/core/d2game/d2game_batch_a0830d.cpp -- thirty 1.13c D2Game.dll skill/AI/item helpers whose arguments the Ghidra
// decompile dropped, recovered from the retail disassembly (D2Game.dll 1.13c, base 0x6FC20000): the prologue's
// register reads, the `ret N` stack-argument count and every caller's pushes / register setup.
// Parameter order of every function here, and of every EXT callee call: ecx, edx (fastcall), then the stack
// arguments in source order (first = last pushed), then any other register input in the order eax, ebx, esi, edi.
// Each register input is named in the signature comment. Pointer fields are int-width (at<int>), D2Common ordinals
// and unported D2Game callees are weak env imports (EXT), gdwBitMasks is read by byte offset (bm8 = a byte test,
// bm32 = a dword test), exactly as the retail instructions address it.
#include <cstdint>
#include <cstdlib>
#include <unistd.h>

#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)
// EXTA: an env import other TUs declare with a fixed signature; a namespaced symbol keeps this file's
// variadic calls without a wasm-ld signature clash on the shared C symbol (pattern of d2game_batch_68d70.cpp).
#define EXTA(n) namespace b10_env { __attribute__((weak, import_module("env"), import_name(#n))) intptr_t n(...); } using namespace b10_env
EXT(Ordinal_10024); EXT(Ordinal_10057); EXT(Ordinal_10064); EXT(Ordinal_10101); EXT(Ordinal_10265);
EXT(Ordinal_10306); EXT(Ordinal_10321); EXT(Ordinal_10331); EXTA(Ordinal_10392); EXT(Ordinal_10494);
EXT(Ordinal_10507); EXTA(Ordinal_10511); EXT(Ordinal_10537); EXT(Ordinal_10546); EXTA(Ordinal_10601);
EXT(Ordinal_10708); EXT(Ordinal_10711); EXT(Ordinal_10744); EXT(Ordinal_10785); EXT(Ordinal_10807);
EXT(Ordinal_10828); EXT(Ordinal_10839); EXT(Ordinal_10871); EXTA(Ordinal_10910); EXT(Ordinal_10911);
EXT(Ordinal_10920); EXT(Ordinal_10945); EXT(Ordinal_10973); EXT(Ordinal_10978); EXT(Ordinal_11013);
EXT(Ordinal_11027); EXT(Ordinal_11043); EXT(Ordinal_11103); EXT(Ordinal_11108); EXT(Ordinal_11111);
EXT(Ordinal_11138); EXTA(Ordinal_11139);
EXT(FUN_6fc21420);   // 0x01420 skill row by id (eax = id)
EXT(FUN_6fc95070);   // 0x75070 AI command block init (stack n; esi buffer, edi unit)
EXT(FUN_6fc95e10);   // 0x75E10 (game, buffer, n)
EXT(FUN_6fc98550);   // 0x78550 (ecx game, edx unit, 5 stack) skill-fail notify
EXT(FUN_6fcab710);   // 0x8B710 packet 0x23 (edx unit id, client, bLeft, skill id, owner, al unit type)
EXT(FUN_6fcae6f0);   // 0x8E6F0 (eax event, edi game)
EXTA(FUN_6fcaea80);   // 0x8EA80 event queue add (ecx game, edx unit, 5 stack)
EXT(FUN_6fcbe4f0);   // 0x9E4F0 (ebx item, esi missile, edi unit)
EXT(FUN_6fcbfc00);   // 0x9FC00 (game, id; eax unit)
EXT(FUN_6fcbfd00);   // 0x9FD00 (game, unit, level, 0; eax id)
EXT(FUN_6fcc1880);   // 0xA1880 (game, missile, a1, a2, 0,0,0,1; eax 0, esi unit)
EXT(FUN_6fcc1d10);   // 0xA1D10 (game, id; edi unit)
EXT(FUN_6fcc28f0);   // 0xA28F0 (ecx game, edx missile, unit, a1, a2, 0, 0,0,0,1)
EXT(FUN_6fcd0980);   // 0xB0980 (a, b; ebx unit)
EXT(FUN_6fcff780);   // 0xDF780 game of unit (eax unit)
EXT(FUN_6fd00660);   // 0xE0660 (game, unit, id)
EXT(FUN_6fd007a0);   // 0xE07A0 (ecx game; eax unit)
extern "C" char* sgptDataTables_exref;
extern "C" uint32_t gdwBitMasks_exref[32];

typedef uint8_t u8;
#define P(x) ((void*)(intptr_t)(x))
template <class T> static inline T& at(const void* p, int off) { return *(T*)((u8*)p + off); }
static inline uint32_t bm8(int off) { return *((u8*)gdwBitMasks_exref + off); }
static inline uint32_t bm32(int off) { return *(uint32_t*)((u8*)gdwBitMasks_exref + off); }
static inline u8* tables() { return (u8*)(intptr_t)*(int*)sgptDataTables_exref; }
static inline u8* skillRow(int id) {
    u8* t = tables();
    if (id < 0 || at<int>(t, 0xba0) <= id) return nullptr;
    return (u8*)(intptr_t)(id * 0x23c + at<int>(t, 0xb98));
}
static inline u8* monStatsRow(int id) {
    u8* t = tables();
    if (id < 0 || at<int>(t, 0xa80) <= id) return nullptr;
    return (u8*)(intptr_t)(id * 0x1a8 + at<int>(t, 0xa78));
}
// Retail reads through a null row here (an access violation); we trap at the same point.
static inline u8* mustRow(u8* row) { if (!row) __builtin_trap(); return row; }
// Inline seed advance (unit+0x20 / +0x24 pair, multiplier 0x6AC690C5 plus carry word), as retail writes it out.
static inline void seedStep(void* seed) {
    uint64_t s = (uint64_t)at<uint32_t>(seed, 0) * 0x6ac690c5ull + at<uint32_t>(seed, 4);
    at<uint64_t>(seed, 0) = s;
}
static const char kFile[] = "..\\Source\\D2Game\\SKILLS\\SkillUse.cpp";   // retail __FILE__ string (address only)

// 1.13c D2Game+0xA0830  int (stack game; esi unit), ret 4
int D2GAME_A0830(void* game, int* unit)
{
    intptr_t skill = Ordinal_10511(unit);
    if (!skill) return 0;
    int id = (int)Ordinal_10601(skill, kFile, 0xbff);
    if (!FUN_6fcbfc00(game, id, unit)) {
        if (unit && unit[0] == 0) FUN_6fc98550(game, unit, 0, 5, 0, 0, 1);
        return 0;
    }
    int level = (int)Ordinal_10306(unit, skill, 1);
    int owner, sid, charges;
    if (Ordinal_10537(skill, &owner, &sid, &level, &charges) && charges == 0) return 0;
    u8* row = skillRow(id);
    if (!row) return 0;
    FUN_6fd007a0(game, unit);
    int* tgt = (int*)P(Ordinal_10392(unit[0xb]));
    if (tgt != unit && tgt && !(bm8(8) & at<u8>(row, 7)) && FUN_6fd00660(game, unit, id)) return 0;
    if (!(bm8(0) & at<u8>(row, 5)) && Ordinal_10057(Ordinal_10331(unit))) {
        Ordinal_11103(unit, 0);
        if (unit[0] == 0) FUN_6fc98550(game, unit, 0, 5, 0, 0, 1);
        return 0;
    }
    int r = (int)FUN_6fcbfd00(game, unit, level, 0, id);
    if (unit[0] == 0 && r == 0) FUN_6fc98550(game, unit, 0, 5, 0, 0, 1);
    return r;
}

// 1.13c D2Game+0xA2100  void __fastcall (ecx unit, edx bLeft, stack skillId, ownerGuid), ret 8
// D2MOO D2GAME_AssignSkill_6FD13800.
void D2GAME_A2100(int* unit, int bLeft, int skillId, int ownerGuid)
{
    if (skillId == 5) return;
    if (!bLeft && Ordinal_10494(unit, 0x55)) Ordinal_10945(unit, 0x55, 0);
    intptr_t skill = Ordinal_10708(unit, skillId, ownerGuid);
    if (!skill) return;
    intptr_t oldRight = 0;
    if (bLeft) Ordinal_10546(unit, skillId, ownerGuid);
    else { oldRight = Ordinal_10507(unit); Ordinal_10978(unit, skillId, ownerGuid); }
    int owner = -1;
    Ordinal_10537(skill, &owner, 0, 0, 0);
    if (unit && unit[0] == 0) {
        u8* pd = (u8*)P(Ordinal_10920(unit));
        FUN_6fcab710(unit[0xc], at<int>(pd, 0x9c), bLeft, skillId, owner, 0);
    }
    if (bLeft) return;
    if (oldRight) {
        u8* row = (u8*)P(FUN_6fc21420(Ordinal_10601(oldRight, kFile, 0xdb3)));
        if (row && (bm8(0x14) & at<u8>(row, 4))) {
            int s = at<int16_t>(row, 0x80);
            if (s >= 0 || s < at<int>(tables(), 0xc4)) {   // retail's always-true `||`, kept
                intptr_t sl = Ordinal_10871(unit, s);
                if (sl) { Ordinal_11111(unit, sl); Ordinal_11108(sl); }
                Ordinal_10945(unit, s, 0);
            }
            intptr_t g = FUN_6fcff780(unit);
            if (unit)
                for (u8* ev = (u8*)P(unit[0x37]); ev; ) {
                    u8* next = (u8*)P(at<int>(ev, 0x24));
                    if (at<u8>(ev, 0) == 8 && at<int>(ev, 0x14) == -1) FUN_6fcae6f0(ev, g);
                    ev = next;
                }
        }
    }
    u8* row = mustRow(skillRow(skillId));
    if (bm8(0x14) & at<u8>(row, 4)) FUN_6fcc1d10(FUN_6fcff780(unit), skillId, unit);
}

// 1.13c D2Game+0xA2A40  int __fastcall (ecx game, edx unit, stack a1, a2), ret 8
int D2GAME_A2A40(void* game, int* unit, int a1, int a2)
{
    intptr_t inv = unit[0x18];
    if (!inv) return 0;
    int item = 0, bodyLoc, other;
    if (Ordinal_10101(inv, &item, &bodyLoc, &other)) {
        if (other) item = (int)Ordinal_11139(inv, bodyLoc == 5 ? 4 : 5);
        if (!Ordinal_10711(item) && !Ordinal_10910(item, 0x7d, 0)) return 0;
    }
    if (!item) return 0;
    unit[0x31] |= 0x40;
    int missile = (int)Ordinal_10321(item);
    intptr_t r;
    if (Ordinal_10744(item, 0x26)) r = FUN_6fcc1880(game, missile, a1, a2, 0, 0, 0, 1, 0, unit);
    else r = FUN_6fcc28f0(game, missile, unit, a1, a2, 0, 0, 0, 0, 1);
    FUN_6fcbe4f0(item, r, unit);
    return 1;
}

// 1.13c D2Game+0xA2F90  void (ecx unit, stack game; eax state), ret 4
// The stat list found or made carries state 0x15 regardless of eax; eax only selects the lookup and the extension.
void D2GAME_A2F90(int* unit, void* game, int state)
{
    if (Ordinal_10494(unit, 0x36)) return;
    if (!unit || unit[0] != 1) return;
    u8* md = (u8*)P(unit[5]);
    if (md && (at<u8>(md, 0x16) & 8)) return;
    if (Ordinal_10064(0, unit)) return;
    intptr_t sl = Ordinal_10871(unit, state);
    int frame = at<int>(game, 0xa8);
    if (!sl) {
        int expire = frame + 0x64;
        sl = Ordinal_11013(at<int>(game, 0x1c), 2, expire, unit[0], unit[3]);
        if (sl) {
            FUN_6fcaea80(game, unit, 0xc, expire, 0, 0, 0);
            Ordinal_10785(sl, 0x15);
            Ordinal_10911(sl, 0x6fcbf7a0);   // retail stat-remove callback address, passed through
            Ordinal_10807(unit, sl, 1);
            Ordinal_10945(unit, 0x15, 1);
        }
    } else {
        int expire = frame + state;
        Ordinal_11043(sl, expire);
        FUN_6fcaea80(game, unit, 0xc, expire, 0, 0, 0);
    }
    int buf[8];
    FUN_6fc95070(1, buf, unit);
    FUN_6fc95e10(game, buf, 1);
    unit[0x31] &= ~1;
}

// 1.13c D2Game+0xA3E80  int (ecx unit, stack target; eax point), ret 4 -- AI attack-desirability score, >= 1
int D2GAME_A3E80(int* unit, void* target, int* pt)
{
    int melee = (int)Ordinal_11138(target, unit, 0);
    int near = 0;
    if (pt && FUN_6fcd0980(pt[3], pt[4], target) > 0x55 && FUN_6fcd0980(pt[3], pt[4], unit) < 0x55) near = 100;
    int s24 = (int)Ordinal_10973(unit, 0x24, 0), s27 = (int)Ordinal_10973(unit, 0x27, 0);
    int s29 = (int)Ordinal_10973(unit, 0x29, 0), s25 = (int)Ordinal_10973(unit, 0x25, 0);
    int s2b = melee ? (int)Ordinal_10973(unit, 0x2b, 0) : 0;
    int resist = (s2b + 4 * (s24 + 2 * s25) + s29 + s27) / 15;
    int s16 = (int)Ordinal_10973(unit, 0x16, 0), s31 = (int)Ordinal_10973(unit, 0x31, 0);
    int s33 = (int)Ordinal_10973(unit, 0x33, 0), s35 = (int)Ordinal_10973(unit, 0x35, 0);
    int s37 = (int)Ordinal_10973(unit, 0x37, 0), s3a = (int)Ordinal_10973(unit, 0x3a, 0);
    int dmg = ((s3a >> 8) + s37 + s35 + s33 + s31 + s16) / 2;
    int stateB = Ordinal_10494(unit, 0xb) ? 100 : 0;
    int lowHp = Ordinal_11027(unit) >= 0x14 ? 0 : 100;
    int lLevel = 0, rLevel = 0; u8 lMul = 0, rMul = 0;
    if (intptr_t sk = Ordinal_10828(unit)) {
        lLevel = (int)Ordinal_10306(unit, sk, 1);
        lMul = at<u8>(mustRow(skillRow((int)Ordinal_10601(sk, kFile, 0x225))), 0x18e);
    }
    if (intptr_t sk = Ordinal_10507(unit)) {
        rLevel = (int)Ordinal_10306(unit, sk, 1);
        rMul = at<u8>(mustRow(skillRow((int)Ordinal_10601(sk, kFile, 0x22b))), 0x18e);
    }
    int skillScore = (rMul * rLevel + lMul * lLevel) / 4;
    int range = melee ? 100 : (Ordinal_10839(target, unit, 4) ? 0 : 0x4b);
    if (unit && unit[0] == 1) {
        u8* ms = monStatsRow(unit[1]);
        if (!ms || at<u8>(ms, 0x4e) <= 1) return 0;
    }
    int r = (resist + 5 * (near + range) + 2 * (skillScore + 2 * stateB + dmg) + 3 * lowHp) / 22;
    return r ? r : 1;
}

EXT(Ordinal_10522); EXT(Ordinal_10630); EXT(Ordinal_10786); EXT(Ordinal_11049);
EXT(FUN_6fc21100);   // 0x01100 unit x (ecx unit)
EXT(FUN_6fc21130);   // 0x01130 unit y (ecx unit)
EXT(FUN_6fc21180);   // 0x01180 seeded roll below n (ecx seed, eax n)
EXT(FUN_6fc41290);   // 0x21290 (ecx unit, stack buffer; eax game)
EXT(FUN_6fcbd230);   // 0x9D230 (ecx game, edx unit, a, b)
EXT(FUN_6fcc3b50);   // 0xA3B50 (game, unit)
EXT(FUN_6fcc4610);   // 0xA4610 AI target pick (game, unit, &a, &b, 0, callback)
EXT(FUN_6fcd0840);   // 0xB0840 monster has skill (edx skill; eax unit)
EXTA(FUN_6fd003a0);   // 0xE03A0 unit by type/guid (ecx, edx, eax)
EXT(FUN_6fd120c0);   // 0xF20C0 (stack n; eax game, ebx unit)
EXT(FUN_6fd12120);   // 0xF2120 (game, target, 0, 0; edi skill)
EXT(FUN_6fd121e0);   // 0xF21E0 (game, a, b; eax mode, edi unit)
EXT(FUN_6fd12220);   // 0xF2220 (game, target; eax mode, edi unit)
EXT(FUN_6fd122c0);   // 0xF22C0 AI idle for n frames (stack n; eax game, esi unit)
EXT(FUN_6fd12380);   // 0xF2380 AI skill command (ecx unit, skill, target, x, y; al mode, ebx game)
EXT(FUN_6fd12540);   // 0xF2540 (ecx game, edx unit, target, 2, 0, 0, 1, 0)

// The 8-dword AI command block A4A10/A4FD0 hand to 0x75E10: {0, unit, target, 0, 0, byte 0x15 = 100, 0, 0}.
static void aiSendStop(void* game, void* unit, intptr_t target)
{
    int buf[8] = {0, (int)(intptr_t)unit, (int)target, 0, 0, 0, 0, 0};
    ((u8*)buf)[0x15] = 0x64;
    FUN_6fc95e10(game, buf, 1);
}

// Monster AI-state list (unit->monsterData+0x28 -> +0x20, circular): the node whose +8 is 10, or null.
static u8* aiNode10(int* unit)
{
    if (!unit || unit[0] != 1) return nullptr;
    u8* md = (u8*)P(unit[5]); if (!md) return nullptr;
    u8* ai = (u8*)P(at<int>(md, 0x28)); if (!ai) return nullptr;
    u8* head = (u8*)P(at<int>(ai, 0x20)); if (!head) return nullptr;
    u8* n = (u8*)P(at<int>(head, 0));
    if (at<int>(n, 8) != 10)
        while (n != head) { n = (u8*)P(at<int>(n, 0)); if (at<int>(n, 8) == 10) break; }
    return at<int>(n, 8) == 10 ? n : nullptr;
}

// 1.13c D2Game+0xA4A10  int (stack game, ctx, bConsume; eax unit), ret 0xc -- 1 = charge spent / stopped
int D2GAME_A4A10(void* game, int* ctx, int bConsume, int* unit)
{
    bool ok = false;
    if (unit && unit[0] == 1 && unit[5]) {
        u8* x = (u8*)P(at<int>(P(unit[5]), 0x28));
        if (x && at<int>(x, 0x28)) {
            intptr_t t = FUN_6fd003a0(at<int>(x, 0x28), at<int>(x, 0x2c), at<int>(x, 0x30));
            intptr_t room = t ? Ordinal_10331(t) : 0;
            ok = room && !Ordinal_10057(room);
        }
    }
    if (!ok) { Ordinal_11103(unit, 0); aiSendStop(game, unit, 0); return 1; }
    u8* d = (u8*)P(ctx[0]);
    if (at<int>(d, 0x18) < 0) {
        int sid = at<int16_t>(P(ctx[7]), 0x170);
        u8* row = skillRow(sid);
        if (!row) { FUN_6fd121e0(game, 0, 0, 0, unit); return 1; }
        intptr_t node = Ordinal_10630(unit, sid);
        if (!node) { FUN_6fd121e0(game, 0, 0, 0, unit); return 1; }
        int lvl = (int)Ordinal_10306(unit, node, 1);
        at<int>(P(ctx[0]), 0x18) = (int)Ordinal_10786(unit, at<int>(row, 0x144), sid, lvl);
    }
    int n = at<int>(d, 0x18);
    if (n <= 0) { FUN_6fd121e0(game, 0, 0, 0, unit); return 1; }
    if (bConsume) at<int>(d, 0x18) = n - 1;
    return 0;
}

// 1.13c D2Game+0xA4FD0  void __fastcall (ecx game, edx unit, stack ctx), ret 4 -- summon-minion AI step
static const int kA4FD0Class[5] = {287, 288, 303, 304, 305};   // retail table 0x6FD1B69C
void D2GAME_A4FD0(u8* game, int* unit, int* ctx)
{
    u8* prm = (u8*)P(ctx[7]);
    int target = ctx[2];
    u8* d = (u8*)P(ctx[0]);
    void* seed = &unit[8];
    bool tryStop = true;
    if (at<int>(d, 0x1c)) { seedStep(seed); tryStop = at<uint32_t>(seed, 0) % 100 < 3; }
    if (tryStop && FUN_6fcc3b50(game, unit)) { Ordinal_11103(unit, 0); aiSendStop(game, unit, target); return; }
    if (!at<int>(d, 0x1c)) at<int>(d, 0x1c) = 1;
    int diff = at<u8>(game, 0x6d);
    int delay = at<int16_t>(prm, 0x5c + diff * 2);
    if (delay - at<int>(game, 0xa8) + at<int>(d, 0x1c) > 0) { FUN_6fd122c0(delay, game, unit); return; }
    if (!target) { FUN_6fd122c0(delay, game, unit); return; }
    seedStep(seed);
    if ((int)(at<uint32_t>(seed, 0) % 100) >= at<int16_t>(prm, 0x56 + diff * 2)) { FUN_6fd122c0(delay, game, unit); return; }
    int idx;
    if (at<int>(d, 0x18) <= 0) {
        idx = (int)FUN_6fc21180(seed, 5);
        at<int>(d, 0x14) = idx;
        at<int>(d, 0x18) = at<int16_t>(prm, 0x6e + diff * 2);
    } else { idx = at<int>(d, 0x14); at<int>(d, 0x18) -= 1; }
    int16_t spread = at<int16_t>(prm, 0x68 + diff * 2);
    int xy[2];
    int dx = (int)FUN_6fc21180(seed, spread * 2) - spread;
    xy[0] = dx + (int)FUN_6fc21100(P(target));
    int dy = (int)FUN_6fc21180(seed, spread * 2) - at<int16_t>(prm, 0x68 + at<u8>(game, 0x6d) * 2);
    xy[1] = dy + (int)FUN_6fc21130(P(target));
    if (Ordinal_11049(Ordinal_10331(unit), xy, 2, 0x805, 3)) {
        FUN_6fd12380(unit, kA4FD0Class[idx], 0, xy[0], xy[1], 4, game);
        at<int>(d, 0x1c) = at<int>(game, 0xa8);
    } else FUN_6fd122c0(0xf, game, unit);
}

// 1.13c D2Game+0xA59B0  void __fastcall (ecx game, edx unit, stack ctx), ret 4
void D2GAME_A59B0(void* game, int* unit, int* ctx)
{
    int tx = 0, ty = 0;
    if (u8* n = aiNode10(unit)) { tx = at<int>(n, 0xc); ty = at<int>(n, 0x10); }
    if (Ordinal_10494(unit, 0xc)) Ordinal_10945(unit, 0xc, 0);
    FUN_6fcbd230(game, unit, ctx[0], *(int*)P(ctx[0]));
    if (!aiNode10(unit) && tx && ty) {
        int buf[5] = {0, 0, 10, tx, ty};   // retail leaves the first two dwords uninitialised
        FUN_6fc41290(unit, buf, game);
    }
    FUN_6fd122c0(1, game, unit);
}

// 0x23 entry of the game's AI-event list (game+0x10f4), whose +0x18 record's byte +0x11 gates A5EB0/A62D0.
static bool aiGate23(u8* game)
{
    int* e = (int*)P(at<int>(game, 0x10f4));
    e = e ? (int*)P(*e) : nullptr;
    while (e && *e != 0x23) e = (int*)P(e[0x3d]);
    if (!e) __builtin_trap();   // retail reads through null
    return at<u8>(P(e[6]), 0x11) != 0;
}

// 1.13c D2Game+0xA5EB0  void (ecx unit, stack game; eax ctx), ret 4 -- skill-or-melee AI step
void D2GAME_A5EB0(int* unit, u8* game, int* ctx)
{
    u8* prm = (u8*)P(ctx[7]);
    if (!aiGate23(game)) { FUN_6fd122c0(0x19, game, unit); return; }
    int a = 0, b = 0;
    intptr_t target = FUN_6fcc4610(game, unit, &a, &b, 0, 0x6fcc38c0);   // retail callback address A38C0
    if (!target) { FUN_6fd122c0(0x19, game, unit); return; }
    int melee = (int)Ordinal_11138(unit, target, 0);
    int dist = (int)Ordinal_10522(unit, target);
    void* seed = &unit[8];
    int diff = at<u8>(game, 0x6d);
    int sk = at<int16_t>(prm, 0x170);
    if (sk > 0 && dist < at<int16_t>(prm, 0x56 + diff * 2)
        && FUN_6fc21180(seed, 100) < at<int16_t>(prm, 0x5c + at<u8>(game, 0x6d) * 2)
        && FUN_6fcd0840(sk, unit)) { FUN_6fd12120(game, target, 0, 0, sk); return; }
    if (melee) {
        seedStep(seed);
        if ((int)(at<uint32_t>(seed, 0) % 100) >= at<int16_t>(prm, 0x62 + at<u8>(game, 0x6d) * 2)) {
            FUN_6fd122c0(0x19, game, unit); return;
        }
        FUN_6fd12220(game, target, FUN_6fc21180(seed, 2) ? 4 : 5, unit);
        return;
    }
    FUN_6fd12540(game, unit, target, 2, 0, 0, 1, 0);
    FUN_6fd120c0(0xa, game, unit);
}

EXTA(Ordinal_10080); EXT(Ordinal_11005);
EXT(FUN_6fcd07b0);   // 0xB07B0 (x, y; eax unit)
EXT(FUN_6fd02780);   // 0xE2780 (eax game, ebx unit, esi out xy)
EXT(FUN_6fd06760);   // 0xE6760 (edx unit; eax game)

static inline u8* monAi2c(int* unit)
{
    u8* m = (unit && unit[0] == 1 && unit[5]) ? (u8*)P(at<int>(P(unit[5]), 0x2c)) : nullptr;
    if (!m) __builtin_trap();   // retail writes through null
    return m;
}

// 1.13c D2Game+0xA62D0  void (ecx unit, stack game; eax ctx), ret 4 -- leap/charge-or-melee AI step
void D2GAME_A62D0(int* unit, u8* game, int* ctx)
{
    u8* prm = (u8*)P(ctx[7]);
    if (!aiGate23(game) || Ordinal_10494(unit, 0x36)) { FUN_6fd122c0(0x19, game, unit); return; }
    intptr_t target = ctx[2];
    if (!target) {
        int a = 0, b = 0;
        target = FUN_6fcc4610(game, unit, &a, &b, 0, 0x6fcc38c0);   // retail callback address A38C0
        if (!target) { FUN_6fd122c0(0x19, game, unit); return; }
    }
    int melee = (int)Ordinal_11138(unit, target, 0);
    int dist = (int)Ordinal_10522(unit, target);
    void* seed = &unit[8];
    int sk = at<int16_t>(prm, 0x170);
    if (sk > 0 && FUN_6fcd0840(sk, unit) && dist < at<int16_t>(prm, 0x56 + at<u8>(game, 0x6d) * 2)) {
        intptr_t roll = FUN_6fc21180(seed, 100);
        if (at<int16_t>(prm, 0x5c + at<u8>(game, 0x6d) * 2) > roll) {
            int tx = (int)FUN_6fc21100(P(target)), ty = (int)FUN_6fc21130(P(target));
            int dx = tx - (int)FUN_6fc21100(unit);
            int dy = ty - (int)FUN_6fc21130(unit);
            int d = (int)Ordinal_10080(unit, tx, ty);
            if (!d) d = 1;
            int spd = at<int16_t>(prm, 0x68 + at<u8>(game, 0x6d) * 2);
            int vx = spd * dx / d, vy = spd * dy / d;
            int buf[8];
            FUN_6fc95070((int)at<int8_t>(prm, 0x180), buf, unit);
            Ordinal_11103(unit, Ordinal_10630(unit, at<int16_t>(prm, 0x170)));
            Ordinal_11005(unit[0xb], 1);
            buf[3] = tx + vx; buf[4] = ty + vy; ((u8*)buf)[0x15] = 0x64;
            FUN_6fc95e10(game, buf, 0);
            return;
        }
    }
    if (melee) {
        if (FUN_6fc21180(seed, 100) >= at<int16_t>(prm, 0x62 + at<u8>(game, 0x6d) * 2)) {
            FUN_6fd122c0(0x19, game, unit); return;
        }
        FUN_6fd12220(game, target, FUN_6fc21180(seed, 2) ? 4 : 5, unit);
        return;
    }
    FUN_6fd12540(game, unit, target, 2, 0, 0, 1, 0);
    FUN_6fd120c0(0xa, game, unit);
}

// 1.13c D2Game+0xA6E60  void __fastcall (ecx game, edx unit, stack ctx), ret 4 -- approach-melee-target AI state
void D2GAME_A6E60(void* game, int* unit, int* ctx)
{
    u8* d = (u8*)P(ctx[0]);
    int target = ctx[2], ctx5 = ctx[5];
    int xy[2];
    if (!FUN_6fd02780(game, unit, xy)) { FUN_6fd122c0(0xa, game, unit); return; }
    if (at<int>(d, 0x14) == 0) { at<int>(d, 0x14) = 1; at<int>(d, 0x18) = 0; at<int>(d, 0x1c) = 0; }
    if (!target) { FUN_6fd122c0(0xa, game, unit); return; }
    if (ctx5 >= 0x14) { FUN_6fd122c0(0x28, game, unit); return; }
    int st = at<int>(d, 0x14);
    if (st == 1) {
        FUN_6fd12540(game, unit, 0, 2, xy[0], xy[1], 1, 0);
        at<int>(d, 0x14) = 2;
        return;
    }
    if (st != 2) { FUN_6fd122c0(0x28, game, unit); return; }
    if ((uint32_t)FUN_6fcd07b0(xy[0], xy[1], unit) >= 2 && at<int>(d, 0x18) < 3) {
        at<int>(d, 0x18) += 1;
        at<int>(monAi2c(unit), 0x18) = 7;
        FUN_6fd12540(game, unit, 0, 2, xy[0], xy[1], 1, 0);
        return;
    }
    if (unit) unit[0x31] |= 0x20000;
    FUN_6fd121e0(game, 0, 0, 0, unit);
    FUN_6fd06760(unit, game);
}

EXT(Ordinal_10136); EXT(Ordinal_10159); EXT(Ordinal_10408); EXT(Ordinal_10581); EXT(Ordinal_10885);
EXT(Ordinal_11016);
EXT(FUN_6fc798b0);   // 0x598B0 (eax game)
EXT(FUN_6fc79c50);   // 0x59C50 (eax game)
EXT(FUN_6fcbe810);   // 0x9E810 (unit, level; eax target)
EXT(FUN_6fcd17b0);   // 0xB17B0 unit search (ecx query, edx game, unit, callback; eax mode)
EXT(FUN_6fd020a0);   // 0xE20A0 (ecx game, edx unit)
EXT(FUN_6fd11b10);   // 0xF1B10 (game, unit; al mode)
EXT(FUN_6fd12420);   // 0xF2420 (delay, skill id, skill mode; esi unit)
EXT(FUN_6fd12d80);   // 0xF2D80 (game, unit, 5, 1; eax target)
EXT(FUN_6fd13d20);   // 0xF3D20 (ecx y, edx monster class, x, 0)

// 1.13c D2Game+0xA7410  void __fastcall (ecx game, edx unit, stack ctx), ret 4 -- burrow-under-target AI step
void D2GAME_A7410(u8* game, int* unit, int* ctx)
{
    u8* prm = (u8*)P(ctx[7]);
    int q[2] = {0, 0x19};
    intptr_t target = FUN_6fcd17b0(q, game, unit, 0x6fcc39e0, 1);   // retail callback address A39E0
    if (!target) { FUN_6fd122c0(0x19, game, unit); return; }
    if (Ordinal_10522(unit, target) < at<int16_t>(prm, 0x56 + at<u8>(game, 0x6d) * 2)) {
        FUN_6fc798b0(game);
        Ordinal_10945(unit, 0x92, 1);
        Ordinal_10159(unit[0xb]);
        for (u8* ev = (u8*)P(unit[0x37]); ev; ) {
            u8* next = (u8*)P(at<int>(ev, 0x24));
            if (at<u8>(ev, 0) == 2) FUN_6fcae6f0(ev, game);
            ev = next;
        }
        Ordinal_10885(Ordinal_10331(unit), unit[0], unit[3]);
        Ordinal_10408(unit);
        Ordinal_11016(unit);
        FUN_6fd020a0(game, unit);
        return;
    }
    at<int>(monAi2c(unit), 0x18) = 7;
    FUN_6fd12540(game, unit, target, 2, 0, 0, 1, 0);
}

// 1.13c D2Game+0xA7580  void __fastcall (ecx game, edx unit, stack ctx), ret 4 -- multi-skill caster AI step
void D2GAME_A7580(u8* game, int* unit, int* ctx)
{
    u8* prm = (u8*)P(ctx[7]);
    int target = ctx[2], ctx5 = ctx[5], ctx6 = ctx[6];
    void* seed = &unit[8];
    auto roll100 = [&]() { seedStep(seed); return (int)(at<uint32_t>(seed, 0) % 100); };
    auto pd = [&](int off) { return (int)at<int16_t>(prm, off + at<u8>(game, 0x6d) * 2); };
    FUN_6fc79c50(game);
    if (Ordinal_10494(unit, 0xc)) Ordinal_10945(unit, 0xc, 0);
    int sk0 = at<int16_t>(prm, 0x170);
    if (!target) {
        intptr_t node = Ordinal_10630(unit, sk0);
        u8* md = (unit && unit[0] == 1) ? (u8*)P(unit[5]) : nullptr;
        if (node && md && (at<int>(md, 0x54) == 3 || at<int>(md, 0x54) == 0x13)) {
            intptr_t mode = Ordinal_10136(node);
            intptr_t id = Ordinal_10601(node, kFile, 0x1537);
            FUN_6fd12420(pd(0x5c), id, mode, unit);
        } else FUN_6fd122c0(0x19, game, unit);
        return;
    }
    if (intptr_t node = Ordinal_10630(unit, sk0)) {
        if (ctx6 && roll100() < pd(0x56)) {
            intptr_t mode = Ordinal_10136(node);
            intptr_t id = Ordinal_10601(node, kFile, 0x1544);
            FUN_6fd12420(pd(0x5c), id, mode, unit);
            return;
        }
    }
    if (ctx5 < pd(0x6e) && roll100() < 0x28 && FUN_6fd12d80(game, unit, 5, 1, target)) return;
    int sk1 = at<int16_t>(prm, 0x174);
    if (sk1 > 0) {
        if (intptr_t node = Ordinal_10630(unit, sk1)) {
            if (roll100() < pd(0x62)) {
                int lvl = (int)Ordinal_10306(unit, node, 1);
                if (intptr_t r = FUN_6fcbe810(unit, lvl, target))
                    if (FUN_6fd12380(unit, sk1, r, 0, 0, at<u8>(prm, 0x182), game)) return;
            }
        }
    }
    int sk2 = at<int16_t>(prm, 0x176);
    if (sk2 > 0 && Ordinal_10630(unit, sk2) && FUN_6fc21180(seed, 100) < 0x3c && ctx5 < 0xe) {
        FUN_6fd12380(unit, sk2, target, 0, 0, at<u8>(prm, 0x183), game); return;
    }
    int sk3 = at<int16_t>(prm, 0x172);
    if (sk3 > 0 && roll100() < pd(0x68) && FUN_6fcd0840(sk3, unit)) {
        int q[6] = {0, 0x271, 0x32, 0, 0, 0};
        FUN_6fcd17b0(q, game, unit, 0x6fc35c40, 1);   // retail callback address 15C40
        if (q[4]) { FUN_6fd12380(unit, at<int16_t>(prm, 0x172), q[4], 0, 0, at<u8>(prm, 0x181), game); return; }
        if (at<int16_t>(prm, 0x178) > 0) {
            int cls = at<int>(tables(), 0xa80) > 0x210 ? 0x210 : -1;
            int y = (int)FUN_6fc21130(unit);
            int x = (int)FUN_6fc21100(unit);
            Ordinal_10331(unit);   // retail computes the room here and drops it
            if (FUN_6fd13d20(y, cls, x, 0)) {
                int cls2 = at<int>(tables(), 0xa80) > 0x1c5 ? 0x1c5 : -1;
                intptr_t r = Ordinal_10581(Ordinal_10331(unit), cls2);
                at<int>(P(ctx[0]), 0x3c) = (int)r;
                FUN_6fd12380(unit, at<int16_t>(prm, 0x178), target, 0, 0, at<u8>(prm, 0x184), game);
                return;
            }
        }
        FUN_6fd11b10(game, unit, 6);
        return;
    }
    if (sk2 > 0 && Ordinal_10630(unit, sk2) && ctx5 < 0xe) {
        FUN_6fd12380(unit, sk2, target, 0, 0, at<u8>(prm, 0x183), game); return;
    }
    if (roll100() < 0x3c) FUN_6fd12540(game, unit, target, 2, 0, 0, 1, 0);
    FUN_6fd122c0(5, game, unit);
}

EXT(FUN_6fc79230);   // 0x59230 (stack unit; eax game)
EXT(FUN_6fc79290);   // 0x59290 (stack unit; eax game)
EXT(FUN_6fc7a160);   // 0x5A160 (ecx game, stack unit; eax node)
EXT(FUN_6fc7a200);   // 0x5A200 (eax game)
EXT(FUN_6fc7a290);   // 0x5A290 (stack unit, out target; edi game)
EXT(FUN_6fc7b9b0);   // 0x5B9B0 (game, node, unit)
EXT(FUN_6fcaa770);   // 0x8A770 send 6-byte packet (stack client; eax buffer)
EXT(FUN_6fcd1bc0);   // 0xB1BC0 nearest target (ecx game, stack out distance; eax unit, ebx out)
EXT(FUN_6fd0fdb0);   // 0xEFDB0 (stack unit, 0; ebx mode, esi game)
EXT(FUN_6fd130c0);   // 0xF30C0 (stack game, out; esi unit)

// Skill-level range value: skill row +0x150 plus (level - 1) * row +0x154; 0 for a bad id or level < 1 (D2Game+0x1A40).
static inline int skillLevelValue(int id, int lvl)
{
    u8* row = skillRow(id);
    if (!row || lvl <= 0) return 0;
    return (lvl - 1) * at<int>(row, 0x154) + at<int>(row, 0x150);
}

// 1.13c D2Game+0xA7A10  void (ecx game, edx unit, stack ctx -- not read), ret 4
void D2GAME_A7A10(u8* game, int* unit, int* ctx)
{
    (void)ctx;
    u8 pk[8] = {0};                 // retail zeroes only the first dword; the target guid lands in it
    void* seed = unit + 8;
    if (FUN_6fc7a290(unit, pk, game)) {
        int target = at<int>(pk, 0);
        if (!target) { FUN_6fd122c0(0x19, game, unit); return; }
        if (!FUN_6fc79290(unit, game)) {
            int y = (int)FUN_6fc21130(P(target));
            int x = (int)FUN_6fc21100(P(target));
            if ((uint32_t)FUN_6fcd07b0(x, y, unit) >= 4) {
                seedStep(seed);
                if (at<uint32_t>(seed, 0) % 100 < 0x46) { FUN_6fd12540(game, unit, target, 2, 0, 0, 1, 7); return; }
                if ((int)FUN_6fc21180(seed, 100) < 10) { FUN_6fd11b10(game, unit, 4); return; }
                FUN_6fd122c0(10, game, unit); return;
            }
        }
        FUN_6fc79230(unit, game);
        Ordinal_10159(unit[0xb]);
        for (u8* ev = (u8*)(intptr_t)unit[0x37]; ev; ) {
            u8 kind = *ev;
            u8* next = (u8*)(intptr_t)at<int>(ev, 0x24);
            if (kind == 2) FUN_6fcae6f0(ev, game);
            ev = next;
        }
        Ordinal_10885(Ordinal_10331(unit, unit[0], unit[3]));
        Ordinal_10408(unit);
        Ordinal_11016(unit);
        FUN_6fd020a0(game, unit);
        return;
    }
    if (int* node = (int*)(intptr_t)FUN_6fd130c0(game, pk, unit)) {
        if (node[0] == 0) {
            if (FUN_6fc7a160(game, unit, node)) {
                FUN_6fc7b9b0(game, node, unit);
                if (unit[0x34] == 0xb) FUN_6fd0fdb0(unit, 0, 8, game);
            } else {
                FUN_6fc7a200(game);
                if ((int)FUN_6fc21180(seed, 1000) < 100) {
                    pk[0] = 0x8a; pk[1] = 1;
                    at<int>(pk, 2) = unit ? unit[3] : -1;
                    int client = node[0] == 0 ? at<int>(P(Ordinal_10920(node)), 0x9c) : 0;
                    FUN_6fcaa770(client, pk);
                }
            }
        }
    }
    FUN_6fd122c0(0x19, game, unit);
}

// 1.13c D2Game+0xA7C20  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_A7C20(u8* game, int* unit, int* ctx)
{
    if (D2GAME_A4A10(game, ctx, 0, unit)) return;
    u8* prm = (u8*)(intptr_t)ctx[7];
    int sk = at<int16_t>(prm, 0x170);
    intptr_t node = Ordinal_10630(unit, sk);
    if (!node) return;
    int lvl = (int)Ordinal_10306(unit, node, 1);
    if (lvl <= 0) return;
    int dist = 0, out = 0;
    int target = (int)FUN_6fcd1bc0(game, &dist, unit, &out);
    int diff = at<u8>(game, 0x6d);
    if (!target) { FUN_6fd122c0(at<int16_t>(prm, 0x5c + diff * 2), game, unit); return; }
    int* r = (int*)(intptr_t)FUN_6fcbe810(unit, lvl, target);
    int last = at<int>(P(ctx[0]), 0x14);
    int half = skillLevelValue(sk, lvl) / 2;
    if (r && r[3] != last && (int)Ordinal_10522(target, r) < half) {
        if (D2GAME_A4A10(game, ctx, 1, unit)) return;
        at<int>(P(ctx[0]), 0x14) = r[3];
        FUN_6fd12380(unit, sk, r, 0, 0, 9, game);
        return;
    }
    diff = at<u8>(game, 0x6d);
    if (dist < at<int16_t>(prm, 0x68 + diff * 2)) {
        diff = at<u8>(game, 0x6d);
        if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(prm, 0x62 + diff * 2)) {
            if (D2GAME_A4A10(game, ctx, 1, unit)) return;
            FUN_6fd12380(unit, at<int16_t>(prm, 0x172), target, 0, 0, 0xe, game);
            return;
        }
    }
    FUN_6fd122c0(at<int16_t>(prm, 0x5c + diff * 2), game, unit);
}

EXT(Ordinal_10494); EXT(Ordinal_10945); EXT(Ordinal_11103);
EXT(FUN_6fc21320);   // 0x01320 MonStats2 row of a monster class (eax class)
EXT(FUN_6fc57680);   // 0x37680 player busy test (esi unit)
EXTA(FUN_6fcfffd0);   // 0xDFFD0 (stack n, other; esi unit)

// 1.13c D2Game+0xA7DC0  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_A7DC0(u8* game, int* unit, int* ctx)
{
    u8* prm = (u8*)(intptr_t)ctx[7];
    int sk = at<int16_t>(prm, 0x170);
    int range = at<int16_t>(prm, 0x68 + at<u8>(game, 0x6d) * 2);
    if (D2GAME_A4A10(game, ctx, 0, unit)) return;
    intptr_t node = Ordinal_10630(unit, sk);
    if (!node) { Ordinal_11103(unit, 0); aiSendStop(game, unit, 0); return; }
    if (Ordinal_10494(unit, 0xc)) Ordinal_10945(unit, 0xc, 0);
    int dist = 0, out = 0;
    int target = (int)FUN_6fcd1bc0(game, &dist, unit, &out);
    if (target && dist < range) {
        int diff = at<u8>(game, 0x6d);
        if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(prm, 0x56 + diff * 2)) {
            if (D2GAME_A4A10(game, ctx, 1, unit)) return;
            if (unit) {
                for (u8* ev = (u8*)(intptr_t)unit[0x37]; ev; ) {
                    u8 kind = *ev;
                    u8* next = (u8*)(intptr_t)at<int>(ev, 0x24);
                    if (kind == 2) FUN_6fcae6f0(ev, game);
                    ev = next;
                }
            }
            int mode = (u8)Ordinal_10136(node);
            FUN_6fd12380(unit, sk, target, 0, 0, mode, game);
            return;
        }
        FUN_6fd122c0(at<int16_t>(prm, 0x5c + diff * 2), game, unit);
        return;
    }
    FUN_6fd122c0(at<int16_t>(prm, 0x62 + at<u8>(game, 0x6d) * 2), game, unit);
}

// 1.13c D2Game+0xA80C0  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_A80C0(void* game, int* unit, int* ctx)
{
    int* list = (unit && unit[0] == 1 && unit[5]) ? (int*)(intptr_t)at<int>(P(unit[5]), 0x30) : nullptr;
    if (!list) __builtin_trap();   // retail reads [0]
    for (int* e = (int*)(intptr_t)list[0]; e; e = (int*)(intptr_t)e[2])
        if (e[0] != 0) { FUN_6fd122c0(10, game, unit); return; }
    int target0 = 0;
    int* node = (int*)(intptr_t)FUN_6fd130c0(game, &target0, unit);
    if (node && node[0] == 0 && !FUN_6fc57680(node) && FUN_6fc21180(unit + 8, 3) && target0) {
        int* p = (int*)(intptr_t)ctx[0];
        int c = p[6];
        if (c == 0) {
            p[6] = 0x3c;
            if (node[0] == 0) FUN_6fcfffd0(0x12, node, unit);
        } else {
            if (c < 0) p[6] = 0;
            if (p[6] > 0) p[6] = p[6] - 1;
        }
        FUN_6fd122c0(0x14, game, unit);
        return;
    }
    int dist = 0;                    // retail writes it over its own stack-argument slot
    int target = (int)FUN_6fcd1bc0(game, &dist, unit, &target0);
    if (target && dist < 0x19) {
        Ordinal_11103(unit, 0);
        int buf[8] = {4, (int)(intptr_t)unit, target, 0, 0, 0, 0, 0};
        u8 flag = 0x64;
        if (unit) {
            u8* m2 = (u8*)(intptr_t)FUN_6fc21320(unit[1]);
            if (m2 && (at<u8>(m2, 0x104) & bm32(0x10))) flag = 0x65;
        }
        at<u8>(buf, 0x15) = flag;
        FUN_6fc95e10(game, buf, 1);
        return;
    }
    FUN_6fd122c0(0x32, game, unit);
}

EXTA(Ordinal_10038); EXT(Ordinal_10188); EXT(Ordinal_10285); EXT(Ordinal_10785); EXT(Ordinal_10807);
EXT(Ordinal_11013);
EXT(FUN_6fc9a090);   // 0x7A090 queue overlay event (stack id; esi unit)
EXT(FUN_6fcc4e40);   // 0xA4E40 (stack superunique; esi unit)
EXT(FUN_6fccfc50);   // 0xAFC50 (game, id, 1; eax unit)
static const int kA8430Idx[5] = {61, 62, 63, 64, 65};   // retail table 0x6FD1B688

// Index into tables+0xAE0 (u16[0x42]); -1 outside it (D2Game+0x1830).
static inline int tableAe0(int i) { return (i < 0 || i >= 0x42) ? -1 : at<uint16_t>(tables(), 0xae0 + i * 2); }
// MonsterData +0x28 of a monster, else 0 (D2Game+0x18E0).
static inline int monData28(int* u) { return (u && u[0] == 1 && u[5]) ? at<int>(P(u[5]), 0x28) : 0; }

// 1.13c D2Game+0xA8430  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_A8430(u8* game, int* unit, int* ctx)
{
    u8* prm = (u8*)(intptr_t)ctx[7];
    int* p = (int*)(intptr_t)ctx[0];
    if (!prm) { FUN_6fd122c0(10, game, unit); return; }
    int now = at<int>(game, 0xa8);
    if (now < p[7]) { FUN_6fd122c0(now - p[7], game, unit); return; }   // retail passes the negative difference
    int idx = p[5];
    int q[4] = {0, 0x40, 0, 0x40};
    FUN_6fcd17b0(q, game, unit, 0, 3);
    if (q[2] == 0) {
        if (!(at<u8>(p, 0x18) & 1)) {
            if (idx >= 0 && idx < 5) {
                u8* rec = (u8*)(intptr_t)Ordinal_10038(tableAe0(kA8430Idx[idx]));
                if (!rec) return;
                int id = at<int>(rec, 4);
                if (id < 0) return;
                FUN_6fc9a090(id, unit);
                if (id == 0x3e) FUN_6fc9a090(0x17, unit);
                else if (id == 0x69) FUN_6fc9a090(0x17d, unit);
            }
            FUN_6fd12380(unit, 0x11d, unit, 0, 0, 0xa, game);
            p[6] |= 1;
            p[7] = at<int>(game, 0xa8) + 0xfa;
            FUN_6fcfffd0(0x10, 0, unit);
            return;
        }
        if (idx >= 5) {
            FUN_6fccfc50(game, 0x22f, 1, unit);
            FUN_6fcbd230(game, unit, monData28(unit), 0);
            Ordinal_10945(unit, 0x8e, 1);
            int type = unit ? unit[0] : 6, guid = unit ? unit[3] : -1;
            if (intptr_t list = Ordinal_11013(at<int>(game, 0x1c), 0, 0, type, guid)) {
                Ordinal_10807(unit, list, 1);
                Ordinal_10785(list, 0x8e);
                Ordinal_10188(list, 0x163, 0x22f, 0);
            }
            FUN_6fd122c0(5, game, unit);
            return;
        }
        if (idx < 0) __builtin_trap();   // retail indexes memory ahead of the 0x6FD1B688 table
        FUN_6fcc4e40(Ordinal_10285(2, tableAe0(kA8430Idx[idx])), unit);
        return;
    }
    int* other = (int*)(intptr_t)ctx[2];
    int sk = at<int16_t>(prm, 0x170);
    if (sk > 0 && other) {
        if (u8* row = skillRow(sk)) {
            int nStates = at<int>(tables(), 0xc4);
            int s1 = at<int16_t>(row, 0x80), s2 = at<int16_t>(row, 0x82);
            if (!(s1 >= 0 && s1 < nStates && Ordinal_10494(unit, s1)) &&
                !(s2 >= 0 && s2 < nStates && Ordinal_10494(other, s2)) &&
                (int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(prm, 0x56 + at<u8>(game, 0x6d) * 2)) {
                FUN_6fd12380(unit, at<int16_t>(prm, 0x170), other, 0, 0, at<u8>(prm, 0x180), game);
                return;
            }
        }
    }
    FUN_6fd122c0(10, game, unit);
}

EXT(Ordinal_10750); EXT(Ordinal_10826); EXT(Ordinal_10851); EXT(Ordinal_10867);
EXT(FUN_6fc57440);   // 0x37440 player's linked unit guid (eax unit)
EXT(FUN_6fccf840);   // 0xAF840 (ecx unit; esi out[5], edi game)
EXT(FUN_6fcd0a20);   // 0xB0A20 (stack other; ebx unit)
EXT(FUN_6fd13e30);   // 0xF3E30 skill usable on (ecx unit, stack game, skill, 0; eax 0, esi other)

// Unit path x / y the way retail inlines them: static units read path +0xC / +0x10, dynamic ones go through
// Ord_10867 / Ord_10750; no path gives 0.
static inline bool staticPath(int* u) { return u[0] == 2 || (u[0] > 3 && u[0] <= 5); }
static inline int unitPX(int* u) { return staticPath(u) ? at<int>(P(u[0xb]), 0xc) : (u[0xb] ? (int)Ordinal_10867(u[0xb]) : 0); }
static inline int unitPY(int* u) { return staticPath(u) ? at<int>(P(u[0xb]), 0x10) : (u[0xb] ? (int)Ordinal_10750(u[0xb]) : 0); }

// Skill weapon/aura class test shared by both hands of A8E80: ids 0x3B / 0x38 always, 0x33 above level 3,
// 0x1B above level 7.
static inline bool a8e80SkillFlag(int* u, intptr_t sk, int line)
{
    bool f = false;
    int id = (int)Ordinal_10601(sk, kFile, line);
    if (id == 0x3b || id == 0x38) f = true;
    if (id == 0x33) { if ((int)Ordinal_10306(u, sk, 1) > 3) f = true; }
    else if (id == 0x1b) { if ((int)Ordinal_10306(u, sk, 1) > 7) f = true; }
    return f;
}

// 1.13c D2Game+0xA8E80  int (stack unit, target, a3, a4, pt; eax ctx), ret 0x14
// Picks one of 17 hireling actions: a weight table chosen by melee range / line of sight, adjusted, then rolled.
int D2GAME_A8E80(int* unit, int* target, uint32_t a3, int a4, int* pt, int* ctx)
{
    if (ctx[5]) return ctx[5];
    if (!target) {
        int y = unitPY(unit), x = unitPX(unit);
        if (Ordinal_10851(Ordinal_10331(unit), x, y, 2, 0x40)) return 0x10;
        seedStep(unit + 8);
        return at<uint32_t>(unit + 8, 0) % 1000 >= 1 ? 0xb : 4;
    }
    u8* game = (u8*)(intptr_t)target[0x20];
    if (!game) game = (u8*)(intptr_t)FUN_6fcff780(unit);
    int* found = nullptr;
    bool chaos = false;
    if (target[0] == 0) {
        int guid = (int)FUN_6fc57440(target);
        if (!game) __builtin_trap();   // retail asserts
        int* u = (int*)(intptr_t)at<int>(game, 0x1520 + (guid & 0x7f) * 4);
        while (u && u[3] != guid) u = (int*)(intptr_t)u[0x39];
        found = u;
        if (u && Ordinal_10826(Ordinal_10331(u)) == 0x6c && (uint32_t)Ordinal_10080(u, pt[3], pt[4]) < 0x55) chaos = true;
    }
    bool skFlag = false;
    if (intptr_t sk = Ordinal_10828(target)) skFlag |= a8e80SkillFlag(target, sk, 0x27f);
    if (intptr_t sk = Ordinal_10507(target)) skFlag |= a8e80SkillFlag(target, sk, 0x286);
    bool far1 = false, far2 = false;
    if (pt) {
        int d = (int)FUN_6fcd0980(pt[3], pt[4], target);
        if (d > 0x55) far1 = true;
        if (d > 0x69) far2 = true;
    }
    int melee = (int)Ordinal_11138(unit, target, 0);
    bool noLos = !Ordinal_10839(unit, target, 4);
    int fireRes = (int)Ordinal_10973(target, 0x27, 0);
    int lightRes = (int)Ordinal_10973(target, 0x29, 0);
    int W[17];
    bool done = false;
    if (melee) {
        static const int T1[17] = {0, 0, 40, 70, 0, 40, 24, 40, 15, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < 17; i++) W[i] = T1[i];
        if ((int)Ordinal_11027(target) < 0x14) W[2] += 10;
        if (Ordinal_10494(target, 0xb)) { W[7] = 0; W[8] = 0; }
        if (fireRes > lightRes) W[6] -= 10;
        if (fireRes < lightRes) W[6] += 10;
        if (noLos) { W[5] = 0; W[6] = 0; }
        if (chaos) W[15] = 10;
        done = true;
    }
    if (!noLos) {
        if (!done) {
            static const int T2[17] = {0, 0, 0, 0, 5, 0, 25, 0, 25, 40, 0, 0, 25, 0, 0, 0, 0};
            for (int i = 0; i < 17; i++) W[i] = T2[i];
            if (a4 < 2) { W[8] -= 5; W[6] = 0; W[1] = 25; W[9] = 0; }
            if (skFlag && !far1) W[13] = 15;
            else if (far1) {
                W[13] = 25; W[1] = 0; W[12] = 15;
                if (W[9] == 0) W[9] = 0x14;
                if (a4 < 2) W[9] -= 5;
            }
            if (far2) W[14] = 0x3c;
            if (chaos) W[15] = 0x14;
        }
    } else if (!done) {
        static const int T3[17] = {0, 0, 0, 0, 0, 25, 25, 0, 15, 20, 10, 0, 20, 0, 0, 0, 0};
        for (int i = 0; i < 17; i++) W[i] = T3[i];
        if ((uint32_t)FUN_6fcd0a20(target, unit) > 0x19) { W[10] += 10; W[8] -= 5; W[5] = 0; }
        if (fireRes > lightRes) { W[6] -= 10; W[8] -= 10; }
        if (fireRes < lightRes) { W[6] += 10; W[8] += 10; }
        if (a4 < 2) W[6] -= 10;
        if (a4 > 3) W[6] += 5;
        if (a3 > 0x3c) W[9] += 10;
        Ordinal_10331(unit);
        int out[5];
        FUN_6fccf840(unit, out, game);
        if (a4 < 2 && out[3] < 2) W[9] = 0;
        if (skFlag && !far1) { W[6] += 10; W[10] = 30; W[13] = 15; }
        else if (far1) {
            W[1] = 0; W[12] = 10; W[10] = 0; W[13] = 15;
            if (W[9] == 0) W[9] = 10;
        }
        if (far2) { W[9] = 0x14; W[14] = 0x3c; }
        if (chaos) W[15] = 0xf;
    }
    if (FUN_6fd13e30(target, game, 0xc7, 0, 0, unit)) W[9] = 0;
    if (W[15] && found && !FUN_6fd13e30(found, game, 0xc7, 0, 0, unit)) W[15] = 0;
    int sum = 0;
    for (int i = 0; i < 17; i++) sum += W[i];
    uint32_t r = 0;
    if (sum > 0) {
        seedStep(unit + 8);
        uint32_t lo = at<uint32_t>(unit + 8, 0);
        r = (sum & (sum - 1)) ? lo % (uint32_t)sum : lo & (uint32_t)(sum - 1);
    }
    int acc = 0;
    for (int i = 0; i < 17; i++) { acc += W[i]; if ((int)r < acc) return i; }
    return 0xb;
}

EXT(FUN_6fc3cae0);   // 0x1CAE0 AI approach (stack game, target, a, b, c, d; eax unit), ret 0x18
EXT(FUN_6fcd1c90);   // 0xB1C90 nearest unit (stack unit, state, &out1, &out2; eax game), ret 0x10
EXT(FUN_6fcd0e90);   // 0xB0E90 (ecx unit; eax other)

// 1.13c D2Game+0xA9E80  void (ecx game, edx unit, stack ctx), ret 4
// ctx[0] is the AI state (+0x1C remembered target guid), ctx[7] the per-difficulty parameter row. The two
// out-param slots of B1C90 are the frame slots of ctx and of the state pointer, so both start from those values.
void D2GAME_A9E80(u8* game, int* unit, int* ctx)
{
    int* st = (int*)P(ctx[0]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    if (!target) {
        int g = st[7];
        if (g) {
            if (!game) __builtin_trap();   // retail asserts (line 0x2d3)
            for (int* u = (int*)P(at<int>(game, 0x1120 + (g & 0x7f) * 4)); u; u = (int*)P(u[0x39]))
                if (u[3] == g) { FUN_6fc3cae0(game, u, 3, 0, 0, 0, unit); return; }
        }
        FUN_6fd122c0(10, game, unit);
        return;
    }
    st[7] = target[3];
    int dist = (int)FUN_6fcd0a20(target, unit);
    int mode = target[4];
    u8* row = (u8*)P(ctx[7]);
    if (dist > 0x32) { FUN_6fc3cae0(game, target, 3, 0, 0, 0, unit); return; }
    int pct = 100;
    int16_t c = at<int16_t>(row, 0x32);
    if (c > 0) { pct = at<int16_t>(row, 0x34) * 100 / c - 100; if (pct >= 100) pct = 100; }
    if (dist > 0x1c) { FUN_6fc3cae0(game, target, 0, 0, 100, 0, unit); return; }
    if (dist > 0x12) {
        if ((mode == 2 || mode == 6) && FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit)) return;
        if (mode == 3 && FUN_6fc3cae0(game, target, 0, 0, 100, 0, unit)) return;
    }
    int slotA = (int)(intptr_t)ctx, slotB = (int)(intptr_t)st;
    int r1 = (int)FUN_6fcd1c90(target, st, &slotB, &slotA, game);
    int r2 = (int)FUN_6fcd1c90(unit, st, &slotA, &slotB, game);
    bool picked = false;
    if (r2 && slotA <= 0x1c) {
        slotA = r2;
        if (FUN_6fcd0e90(unit, r2)) picked = true;
    }
    if (!picked) {
        slotA = 0;
        if (r1 && (int)FUN_6fcd0a20(P(r1), unit) < 0x1c && FUN_6fcd0e90(unit, r1)) slotA = r1;
    }
    int diff = at<u8>(game, 0x6d);
    if (slotB) {
        if (slotA) {
            if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x62 + diff * 2)) {
                FUN_6fd12120(game, slotA, 0, 0, (int)at<int16_t>(row, 0x170));
                return;
            }
            FUN_6fd12220(game, slotA, 4, unit);
            FUN_6fd120c0((int)at<int16_t>(row, 0x56 + diff * 2), game, unit);
            return;
        }
    } else if (slotA) {
        if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x5c + diff * 2)) {
            if (pct < -0x7e || pct > 0x7e) __builtin_trap();   // retail asserts (line 0x110)
            u8* ai = (unit[0] == 1 && unit[5]) ? (u8*)P(at<int>(P(unit[5]), 0x2c)) : nullptr;
            if (pct) at<int>(ai, 0x1c) = pct;
            at<u8>(ai, 0x20) = 0x28;
        }
        FUN_6fd12540(game, unit, slotA, 2, 0, 0, 1, 0);
        return;
    }
    if (dist > 0x10) FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit);
    else FUN_6fd122c0(0xf, game, unit);
}

EXT(FUN_6fcc41b0);   // 0xA41B0 (dl kind, stack game, target; eax mode, esi unit), ret 8
EXT(FUN_6fcfeee0);   // 0xDEEE0 (ecx game, edx unit, stack unit, 1), ret 8
EXT(FUN_6fd11c30);   // 0xF1C30 (stack game, n; eax target, edi unit), ret 8
// 0x01A90 inlined: skills row +0x158 + (lvl - 1) * row +0x15C; 0 for a bad id or lvl <= 0.
static inline int skillLevelValue2(int id, int lvl)
{
    if (id < 0 || id >= at<int>(tables(), 0xba0) || lvl <= 0) return 0;
    u8* r = (u8*)P(at<int>(tables(), 0xb98)) + id * 0x23c;
    if (!r) return 0;
    return at<int>(r, 0x158) + (lvl - 1) * at<int>(r, 0x15c);
}

// 1.13c D2Game+0xAA1D0  void (ecx game, edx unit, stack ctx), ret 4
// st = ctx[0]: +0x14 charge counter (-1 = unset), +0x18 next-use frame, +0x1C toggle. ctx[7] parameter row.
void D2GAME_AA1D0(u8* game, int* unit, int* ctx)
{
    int* st = (int*)P(ctx[0]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    u8* row = target ? (u8*)P(ctx[7]) : nullptr;
    if (!row) { FUN_6fd122c0(10, game, unit); return; }
    if (st[5] == -1) {
        st[5] = 3;
        int16_t sk = at<int16_t>(row, 0x170);
        if (sk >= 0) {
            int lvl = (int)Ordinal_10306(target, Ordinal_10630(target, (int)sk), 1);
            st[5] = skillLevelValue2(at<int16_t>(row, 0x170), lvl);
        }
    } else if (st[5] == 0) {
        FUN_6fcfeee0(game, unit, unit, 1);
        return;
    }
    int dist = (int)FUN_6fcd0a20(target, unit);
    if (dist > 0x32) { FUN_6fc3cae0(game, target, 3, 0, 0, 0, unit); return; }
    int pct = 100;
    int16_t c = at<int16_t>(row, 0x32);
    if (c > 0) { pct = at<int16_t>(row, 0x34) * 100 / c - 100; if (pct >= 100) pct = 100; }
    if (dist > 0x1c) { FUN_6fc3cae0(game, target, 0, 0, pct, 0, unit); return; }
    int o1 = 0, o2 = 0;
    if (int r = (int)FUN_6fcd1c90(target, ctx[0], &o1, &o2, game)) Ordinal_10522(unit, r);
    int c6 = ctx[6], c5 = ctx[5], c2 = ctx[2];
    int d = at<u8>(game, 0x6d);
    int avg = (at<int16_t>(row, 0x5c + d * 2) + at<int16_t>(row, 0x56 + d * 2)) / 2;
    if (c2 && st[6] < at<int>(game, 0xa8)) {
        if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x68 + d * 2) && c5 < at<int16_t>(row, 0x6e + d * 2)) {
            if (c6) {
                FUN_6fd12220(game, c2, 4, unit);
                int dd = at<u8>(game, 0x6d);
                st[6] = at<int>(game, 0xa8) + at<int16_t>(row, 0x62 + dd * 2) * 10;
                st[5] -= 1;
            } else {
                FUN_6fd12540(game, unit, c2, 2, 0, 0, 1, 0);
            }
            return;
        }
    }
    d = at<u8>(game, 0x6d);
    if (dist <= at<int16_t>(row, 0x56 + d * 2) && dist >= at<int16_t>(row, 0x5c + d * 2)) {
        if (FUN_6fcc41b0(4, game, target, st[7] ? 5 : 6, unit)) return;
        st[7] = st[7] == 0;
        if (FUN_6fcc41b0(4, game, target, st[7] ? 5 : 6, unit)) return;
    }
    if (!FUN_6fd11c30(game, avg, target, unit)) FUN_6fc3cae0(game, target, 1, 0, 0, 0, unit);
}

EXT(FUN_6fd12d80);   // 0xF2D80 (stack game, unit, mode, 1; eax target), ret 0x10
EXT(FUN_6fc3e140);   // 0x1E140 (stack game, unit, target, flag, 0, 6; esi other), ret 0x18
EXT(FUN_6fd019c0);   // 0xE19C0 (ecx x, edx room, stack game, unit, 0, 0; eax y), ret 0x10

// 1.13c D2Game+0xAA4D0  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_AA4D0(u8* game, int* unit, int* ctx)
{
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    if (!target) { FUN_6fd122c0(10, game, unit); return; }
    u8* row = (u8*)P(ctx[7]);
    int o1 = (int)(intptr_t)ctx, o2 = 0;   // o1 is the frame slot of ctx; o2 an uninitialised local
    int r = (int)FUN_6fcd1c90(unit, ctx[0], &o1, &o2, game);
    if (r && o1 > 0x18) r = 0;
    if (!r) r = 0;
    if (o2 && r) {
        seedStep(unit + 8);
        if ((int)(at<uint32_t>(unit + 8, 0) % 100) < at<int16_t>(row, 0x56 + at<u8>(game, 0x6d) * 2)
            && FUN_6fd12d80(game, unit, 6, 1, r)) return;
    }
    seedStep(unit + 8);
    if ((int)(at<uint32_t>(unit + 8, 0) % 100) < at<int16_t>(row, 0x5c + at<u8>(game, 0x6d) * 2)) { r = 0; o2 = 0; }
    int dist = (int)Ordinal_10522(unit, target);
    if (dist > at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2)) {
        int y = unitPY(target), x = unitPX(target);
        if (FUN_6fd019c0(x, Ordinal_10331(target), game, unit, 0, 0, y)) { FUN_6fd122c0(0x19, game, unit); return; }
    }
    int mode = target[4];
    if (dist > at<int16_t>(row, 0x68 + at<u8>(game, 0x6d) * 2)) {
        if ((mode == 2 || mode == 6) && FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit)) return;
        if (mode == 3 && FUN_6fc3cae0(game, target, 0, 0, 0x3c, 0, unit)) return;
    }
    if (FUN_6fc3e140(game, unit, r, o2, 0, 6, target)) return;
    FUN_6fd122c0(0x19, game, unit);
}

EXT(FUN_6fc2c1f0);   // 0x0C1F0 nearest enemy (stack game, range; esi unit), ret 8
EXT(FUN_6fd11b10);   // 0xF1B10 (stack game, unit; al n), ret 8

static inline u8* monAi(int* u) { return (u[0] == 1 && u[5]) ? (u8*)P(at<int>(P(u[5]), 0x2c)) : nullptr; }

// 1.13c D2Game+0xAA710  void (stack game, ctx; eax unit), ret 8
// st = ctx[0]: +0x14 retreat flag, +0x18 retreat-from guid, +0x1C remembered target guid. ctx[7] parameter row.
// Both B1C90 out-param pairs alias frame slots: the first gets the game slot, the second the state slot.
void D2GAME_AA710(u8* game, int* ctx, int* unit)
{
    int* st = (int*)P(ctx[0]);
    u8* row = (u8*)P(ctx[7]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    if (!target) {
        int g = st[7];
        if (g) {
            if (!game) __builtin_trap();   // retail asserts (line 0x2d3)
            int* found = (int*)P(at<int>(game, 0x1120 + (g & 0x7f) * 4));
            while (found && found[3] != g) found = (int*)P(found[0x39]);
            if (at<int16_t>(row, 0x172) >= 0 && found) {
                int y = (int)FUN_6fc21130(found), x = (int)FUN_6fc21100(found);
                FUN_6fd12380(unit, (int)at<int16_t>(row, 0x172), 0, x, y, (int)at<u8>(row, 0x181), game);
                FUN_6fcaea80(game, unit, 0, at<int>(game, 0xa8) + 6, 0, 0, 0);
                FUN_6fd120c0(8, game, unit);
                return;
            }
        }
        FUN_6fd122c0(10, game, unit);
        return;
    }
    st[7] = target[3];
    if (Ordinal_10057(Ordinal_10331(unit))) {
        if (FUN_6fc3e140(game, unit, 0, 0, 0, 6, target)) return;
        FUN_6fd122c0(0x21, game, unit);
        return;
    }
    int thr = at<int16_t>(row, 0x68 + at<u8>(game, 0x6d) * 2);
    int o1 = (int)(intptr_t)game, o2 = 0, o3 = 0, stSlot = (int)(intptr_t)st;
    int r = (int)FUN_6fcd1c90(unit, st, &o1, &o2, game);
    int pick = (r && o1 <= thr) ? r : 0;
    int r2 = (int)FUN_6fcd1c90(target, st, &o3, &stSlot, game);
    if (!(pick && FUN_6fcd0e90(unit, pick))) {
        pick = 0;
        if (r2 && o3 < thr && FUN_6fcd0e90(unit, r2)) pick = r2;
    }
    int dist = (int)Ordinal_10522(target, unit);
    if (pick && dist > thr && (int)Ordinal_10522(target, pick) > thr) pick = 0;
    int pct = 100;
    int16_t c = at<int16_t>(row, 0x32);
    if (c > 0) { pct = at<int16_t>(row, 0x34) * 100 / c - 100; if (pct >= 100) pct = 100; }
    if (at<int16_t>(row, 0x172) >= 0 && dist > 0x32) {
        int y = (int)FUN_6fc21130(target), x = (int)FUN_6fc21100(target);
        FUN_6fd12380(unit, (int)at<int16_t>(row, 0x172), 0, x, y, (int)at<u8>(row, 0x181), game);
        FUN_6fcaea80(game, unit, 0, at<int>(game, 0xa8) + 2, 0, 0, 0);
        FUN_6fd120c0(10, game, unit);
        return;
    }
    if (dist > at<int16_t>(row, 0x6e + at<u8>(game, 0x6d) * 2)) { FUN_6fc3cae0(game, target, 0, 1, 100, 0, unit); return; }
    if (dist > thr) {
        int mode = target[4];
        if (mode == 3) { FUN_6fc3cae0(game, target, 0, 1, 100, 0, unit); return; }
        if (mode == 2 || mode == 6) { FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit); return; }
    }
    if (FUN_6fc3e140(game, unit, pick, o2, 1, 6, target)) return;
    intptr_t cursed = Ordinal_10494(unit, 0x8a);
    bool clearFlag = false;
    if (at<int16_t>(row, 0x170) >= 0 && !cursed) {
        bool tryFlee = (int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2)
                       || !pick || at<int>(P(ctx[0]), 0x14);
        if (tryFlee) {
            int* n = (int*)P(FUN_6fc2c1f0(game, 10, unit));
            if (n && (int)Ordinal_10522(n, unit) < thr / 2) {
                if (Ordinal_11138(unit, n, 0)) {
                    at<int>(P(ctx[0]), 0x14) = 0;
                    FUN_6fd12380(unit, (int)at<int16_t>(row, 0x170), n, 0, 0, (int)at<u8>(row, 0x180), game);
                    return;
                }
                if (!o2) {
                    FUN_6fd12540(game, unit, n, 0xf, 0, 0, 1, 0);
                    at<int>(P(ctx[0]), 0x14) = 1;
                    at<int>(P(ctx[0]), 0x18) = n[3];
                    return;
                }
            }
        } else clearFlag = true;
    } else clearFlag = true;
    if (clearFlag) at<int>(P(ctx[0]), 0x14) = 0;
    if (o2) {
        FUN_6fd12220(game, pick, 4, unit);
        FUN_6fd120c0((int)at<int16_t>(row, 0x56 + at<u8>(game, 0x6d) * 2), game, unit);
        return;
    }
    if (pick) {
        if (pct < -0x7e || pct > 0x7e) __builtin_trap();   // retail asserts (line 0x110)
        if (pct) at<int>(monAi(unit), 0x1c) = pct;
        FUN_6fd12540(game, unit, pick, 0xf, 0, 0, 1, 0);
        return;
    }
    if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x5c + at<u8>(game, 0x6d) * 2)) FUN_6fd11b10(game, unit, 10);
    else FUN_6fd122c0(0xf, game, unit);
}

EXT(Ordinal_10186); EXT(Ordinal_10913);
EXT(FUN_6fcffa90);   // 0xDFA90 unit is dead (eax unit)

// 1.13c D2Game+0xAB070  void (ecx game, edx unit, stack ctx), ret 4
// Healer/mana-giver minion: ctx[2] a buddy unit, ctx[6] its flag, ctx[7] the parameter row, ctx[0] the state.
void D2GAME_AB070(u8* game, int* unit, int* ctx)
{
    u8* row = (u8*)P(ctx[7]);
    int* st = (int*)P(ctx[0]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    if (!target) return;
    int c2 = ctx[2], c6 = ctx[6];
    int melee = 0, flag = 1, distN = 0;
    int* n = nullptr;
    if ((int)Ordinal_10522(unit, target) >= at<int16_t>(row, 0x6e + at<u8>(game, 0x6d) * 2)
        && FUN_6fc3cae0(game, target, 3, 0, 0, 6, unit)) return;
    if (c2 && FUN_6fcffa90(P(c2))) { c2 = 0; c6 = 0; }
    if (at<int16_t>(row, 0x170) > 0) {
        int sk = at<int16_t>(row, 0x170);
        u8* skr = skillRow(sk);
        intptr_t node = Ordinal_10630(unit, sk);
        if (skr && node) {
            int lvl = (int)Ordinal_10306(unit, node, 1);
            int v = (int)Ordinal_10786(unit, at<int>(skr, 0x64), (int)at<int16_t>(row, 0x170), lvl);
            if (v <= 5) v = 5; else if (v >= 0x32) v = 0x32;
            n = (int*)P(FUN_6fc2c1f0(game, v, unit));
            if (n) distN = (int)Ordinal_10522(unit, n);
        }
    }
    if (distN >= at<int16_t>(row, 0x5c + at<u8>(game, 0x6d) * 2)) n = nullptr;
    else if (n) melee = (int)Ordinal_11138(unit, n, 0);
    if (FUN_6fc3e140(game, unit, n, melee, 0, 6, target)) return;
    int v = 0, m = 0;
    bool test = true;
    if (unit[1] == 0x1aa) { v = (int)Ordinal_10973(target, 6, 0); m = (int)Ordinal_10913(target); }
    else if (unit[1] == 0x1ab) { v = (int)Ordinal_10973(target, 8, 0); m = (int)Ordinal_10186(target); }
    else test = false;
    if (test && v >= m) flag = 0;
    if (n && melee && flag && at<int16_t>(row, 0x56 + at<u8>(game, 0x6d) * 2) + st[6] < at<int>(game, 0xa8)) {
        FUN_6fd12380(unit, (int)at<int16_t>(row, 0x170), n, 0, 0, 8, game);
        st[6] = at<int>(game, 0xa8);
        return;
    }
    if (c6 && c2 && (int)FUN_6fc21180(unit + 8, 100) < 0x19) {
        FUN_6fd12d80(game, unit, (int)at<u8>(row, 0x68 + at<u8>(game, 0x6d) * 2), 0, c2);
        return;
    }
    if (n) { FUN_6fd12540(game, unit, n, 2, 0, 0, 1, 7); return; }
    FUN_6fd122c0((int)at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2), game, unit);
}

// 1.13c D2Game+0xAB360  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_AB360(u8* game, int* unit, int* ctx)
{
    u8* row = (u8*)P(ctx[7]);
    int* st = (int*)P(ctx[0]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    int flag = 0, dist = 0;
    if (!target) { FUN_6fd122c0(0x19, game, unit); return; }
    if ((int)Ordinal_10522(unit, target) >= at<int16_t>(row, 0x6e + at<u8>(game, 0x6d) * 2)
        && FUN_6fc3cae0(game, target, 3, 0, 0, 6, unit)) return;
    if (Ordinal_10057(Ordinal_10331(unit))) {
        if (FUN_6fc3e140(game, unit, 0, 0, 0, 6, target)) return;
        FUN_6fd122c0((int)at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2), game, unit);
        return;
    }
    int* n = (int*)P(FUN_6fcd1bc0(game, &dist, unit, &flag));
    if (dist >= at<int16_t>(row, 0x5c + at<u8>(game, 0x6d) * 2)) n = nullptr;
    if (FUN_6fc3e140(game, unit, n, flag, 0, 6, target)) return;
    if (n) {
        if (Ordinal_10494(n, 2) || Ordinal_10973(n, 0x2d, 0) == 100) {
            FUN_6fd12d80(game, unit, (int)at<u8>(row, 0x68 + at<u8>(game, 0x6d) * 2), 0, n);
            return;
        }
        if (!flag) { FUN_6fd12540(game, unit, n, 2, 0, 0, 1, 7); return; }
        int16_t sk = at<int16_t>(row, 0x170);
        if (sk >= 0 && at<int16_t>(row, 0x56 + at<u8>(game, 0x6d) * 2) + st[6] < at<int>(game, 0xa8)) {
            FUN_6fd12380(unit, (int)sk, n, 0, 0, (int)at<u8>(row, 0x180), game);
            st[6] = at<int>(game, 0xa8);
            return;
        }
    }
    FUN_6fd122c0((int)at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2), game, unit);
}

// ---- AC410: weapon-pair skill swap AI (AiThink4.cpp) ----
EXT(Ordinal_10302); EXTA(Ordinal_10002);
EXT(FUN_6fcc3ae0);   // 0xA3AE0 (ecx node, stack ctx; eax unit), ret 4
EXT(FUN_6fcc4890);   // 0xA4890 (ecx target, edx unit, stack game, skill, ctx; eax skillId), ret 0xc
static const char kFileAi4[] = "..\\Source\\D2Game\\Ai\\AiThink4.cpp";   // retail __FILE__ string (address only)

static void ac410Cast(u8* game, int* unit, int* ctx, intptr_t skill, int target, int line)
{
    // F2380 al mode = Ord_10136(skill); the 10601 id stays pushed as the skill argument
    intptr_t id = Ordinal_10601(skill, kFileAi4, line);
    FUN_6fd12380(unit, id, target, 0, 0, Ordinal_10136(skill), game);
}

// 1.13c D2Game+0xAC410  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_AC410(u8* game, int* unit, int* ctx)
{
    u8* row = (u8*)P(ctx[7]);
    int ctx2 = ctx[2], ctx5 = ctx[5], ctx6 = ctx[6];
    int* target = nullptr;
    if (unit) {
        for (u8* ev = (u8*)P(unit[0x37]); ev; ) {
            u8* next = (u8*)P(at<int>(ev, 0x24));
            if (at<u8>(ev, 0) == 2) FUN_6fcae6f0(ev, game);
            ev = next;
        }
        if (unit[0] == 1 && unit[5]) {
            int* s = (int*)P(at<int>(P(unit[5]), 0x28));
            if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
        }
    }
    if (!target) { FUN_6fd122c0(100, game, unit); return; }
    int c = at<int16_t>(row, 0x84);
    c = c <= 1 ? 1 : (c < 0x100 ? c : 0x100);
    int* st = (int*)P(ctx[0]);
    st[6] += -1 - at<int16_t>(row, 0x68 + at<u8>(game, 0x6d) * 2);
    if (st[6] < 0 || st[6] > (c << 6)) st[6] = 0;
    int dist = (int)Ordinal_10522(unit, target);
    if (ctx5 > at<int16_t>(row, 0x56 + at<u8>(game, 0x6d) * 2) || dist > at<int16_t>(row, 0x5c + at<u8>(game, 0x6d) * 2))
        ctx2 = 0;
    if (FUN_6fc3e140(game, unit, ctx2, ctx6, 0, 6, target)) return;
    intptr_t skill = 0;
    if (ctx2) {
        intptr_t a = Ordinal_10507(target), b = Ordinal_10828(target);
        if (!a || !b) { FUN_6fd122c0(0x19, game, unit); return; }
        intptr_t ra = Ordinal_10601(a, kFileAi4, 0xb53), rb = Ordinal_10601(b, kFileAi4, 0xb54);
        intptr_t la = FUN_6fcc3ae0(a, ctx, target), lb = FUN_6fcc3ae0(b, ctx, target);
        Ordinal_10302(unit, ra, la, 0, kFileAi4, 0xb59);
        Ordinal_10302(unit, rb, lb, 0, kFileAi4, 0xb5a);
        intptr_t sa = Ordinal_10708(unit, ra, -1), sb = Ordinal_10708(unit, rb, -1);
        if (!sa || !sb) { FUN_6fd122c0(0x19, game, unit); return; }
        skill = FUN_6fc21180(&unit[8], 2) ? sb : sa;
        intptr_t sid = Ordinal_10601(skill, kFileAi4, 0xb66);
        int lvl = st[7] > 1 ? st[7] : 1;
        int pct = at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2) - 2 * lvl;
        pct = pct <= 5 ? 5 : (pct < 100 ? pct : 100);
        if (ctx6 && FUN_6fc21180(&unit[8], 100) < pct) { skill = Ordinal_10708(unit, 0, -1); sid = 0; }
        if (!FUN_6fcc4890(target, unit, game, skill, ctx, sid)) {
            skill = (skill == sb) ? sa : sb;
            if (!FUN_6fcc4890(target, unit, game, skill, ctx, Ordinal_10601(skill, kFileAi4, 0xb79))) {
                skill = Ordinal_10708(unit, 0, -1);
                if (!skill) {
                    Ordinal_10302(unit, 0, 1, 0, kFileAi4, 0xb81);
                    skill = Ordinal_10708(unit, 0, -1);
                }
            }
        }
    }
    if (!skill) { FUN_6fd122c0(0x19, game, unit); return; }
    int id = (int)Ordinal_10601(skill, kFileAi4, 0xb8f);
    if (Ordinal_10002(unit, skill) == 1) {
        if (!ctx6) { FUN_6fd12540(game, unit, ctx2, 0xf, 0, 0, 1, 0); return; }
        ac410Cast(game, unit, ctx, skill, ctx2, 0xb96);
    } else {
        ac410Cast(game, unit, ctx, skill, ctx2, 0xba8);
    }
    u8* srow = (u8*)P(FUN_6fc21420(id));
    int lvl = (int)Ordinal_10306(unit, skill, 1);
    if (lvl <= 1) lvl = 1;
    at<int>(P(ctx[0]), 0x14) = (int)Ordinal_10786(unit, at<int>(srow, 0x190), id, lvl) / 3 + at<int>(game, 0xa8) + 0x12;
}

// ---- AD580 / AD6C0: event callbacks (table 0x6fd277a8, events 0x14 / 0x12) ----
EXT(FUN_6fcfe0c0);   // 0xDE0C0 apply damage (stack game, attacker, 1, &dmg; eax unit), ret 0x10
EXT(FUN_6fcfcf20);   // 0xDCF20 damage follow-up (stack game, attacker, &dmg; eax unit), ret 0xc

// 1.13c D2Game+0xAD580  int __fastcall (ecx game, edx event, stack attacker, unit, damage, packed, arg5), ret 0x14
int D2GAME_AD580(u8* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)damage; (void)arg5;
    if (!attacker || !unit) return 0;
    int lvl = (int)Ordinal_10910(attacker, packed >> 16, packed);
    if (lvl <= 0) return 0;
    int a = (int)Ordinal_10973(attacker, 0xc, 0);
    int b = (int)Ordinal_10973(unit, 0xc, 0);
    int c = lvl - 1 < 0 ? 0 : lvl - 1;
    if (event == 6) a -= 6;
    int v = (c * 4 - b + a + 10) * 5;
    if (event == 6) v /= 3;
    v = v <= 0 ? 0 : (v < 100 ? v : 100);
    seedStep(&attacker[8]);
    int r = (int)(at<uint32_t>(&attacker[8], 0) % 100u);
    if (r >= v) return 0;
    int d = (v - r) * 2 + 25;
    d = d <= 25 ? 25 : (d < 0xfa ? d : 0xfa);
    int dmg[0x1c] = {};
    dmg[0x34 / 4] = d;
    FUN_6fcfe0c0(game, attacker, 1, dmg, unit);
    return 1;
}

// 1.13c D2Game+0xAD6C0  int __fastcall (ecx game, edx event, stack attacker, unit, damage, packed, arg5), ret 0x14
int D2GAME_AD6C0(u8* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)event; (void)damage; (void)arg5;
    if (!attacker || !unit || !((unit[0x31] >> 2) & 1)) return 0;
    int lvl = (int)Ordinal_10910(attacker, packed >> 16, packed);
    if (lvl <= 0) return 0;
    int a = (int)Ordinal_10973(unit, 0xc, 0);
    int b = (int)Ordinal_10973(attacker, 0xc, 0);
    int d = b > a ? (b - a) * 10 + 0x19 : 0x19;
    int dmg[0x1c] = {};
    at<uint16_t>(dmg, 4) |= 0x4021;
    dmg[0x24 / 4] = lvl << 8;
    dmg[0x30 / 4] = d;
    dmg[0x60 / 4] = 0x3d;
    FUN_6fcfe0c0(game, attacker, 1, dmg, unit);
    FUN_6fcfcf20(game, attacker, dmg, unit);
    return 1;
}

// ---- AE6D0: item-use potion by percentage (1.10f SKILLITEM_pSpell04_ApplyPotionStats lineage) ----
EXT(Ordinal_10695); EXT(Ordinal_10428); EXT(Ordinal_10959); EXT(Ordinal_10551);
EXT(Ordinal_10680); EXT(Ordinal_10887);
EXT(FUN_6fc214d0);   // 0x014D0 (ecx div, edx a; eax b) -> a*b/div
EXT(FUN_6fc6ea30);   // 0x4EA30 share potion life (ecx value; eax unit, esi game)
EXT(FUN_6fc9a260);   // 0x7A260 queue life update (stack pct; esi unit), ret 4

// 1.13c D2Game+0xAE6D0  int __fastcall (ecx game, edx unit, stack item, target, x, y, skillId), ret 0x14
int D2GAME_AE6D0(u8* game, int* unit, int* item, int* target, int x, int y, int skillId)
{
    (void)target; (void)x; (void)y; (void)skillId;
    u8* irow = (u8*)P(Ordinal_10695(item ? item[1] : -1));
    if (!irow) return 0;
    int len = (int)Ordinal_10428(unit, item, at<int>(irow, 0xb0));
    int state = at<int16_t>(irow, 0x98);
    intptr_t list = 0;
    int remaining = 0;
    if (len > 0) {
        if (state <= 0 || state >= at<int>(tables(), 0xc4)) return 0;
        list = Ordinal_10871(unit, state);
        if (list) remaining = (int)Ordinal_10959(list) - at<int>(game, 0xa8);
    }
    int result = 0, hp = 0;
    for (int i = 0; i < 3; ++i) {
        int stat = at<int16_t>(irow, 0x9e + i * 2);
        if (stat < 0 || stat >= at<int>(tables(), 0xbd4)) continue;
        u8* isc = (u8*)P(at<int>(tables(), 0xbcc)) + stat * 0x144;
        if (!isc || at<uint16_t>(isc, 0x32) >= at<int>(tables(), 0xbd4)) continue;
        int value = (int)Ordinal_10428(unit, item, at<int>(irow, 0xa4 + i * 4));
        if (value <= 0) continue;
        int maxv = (int)Ordinal_10973(unit, at<uint16_t>(isc, 0x32), 0);
        int add = (int)FUN_6fc214d0(100, maxv, value);
        if (value == 100) add = maxv;
        if (stat == 6) hp = 1;
        if ((stat == 6 || stat == 0x4a) && unit && unit[0] == 0) add = (int)FUN_6fc6ea30(add, unit, game);
        if (len <= 0) {
            int old = (int)Ordinal_10973(unit, stat, 0);
            if (old + add > maxv) add = maxv - old;
            Ordinal_10551(unit, stat, add, 0);
            result = 1;
            continue;
        }
        int expire = at<int>(game, 0xa8) + remaining + len;
        if (!list) {
            list = Ordinal_11013(at<int>(game, 0x1c), 2, expire, unit ? unit[0] : 6, unit ? unit[3] : -1);
            if (!list) continue;
            Ordinal_10785(list, state);
            Ordinal_10945(unit, state, 1);
            if (!unit || unit[0] != 0) {
                if (unit)
                    for (u8* ev = (u8*)P(unit[0x37]); ev; ) {
                        u8* next = (u8*)P(at<int>(ev, 0x24));
                        if (at<u8>(ev, 0) == 3) FUN_6fcae6f0(ev, game);
                        ev = next;
                    }
                FUN_6fcaea80(game, unit, 3, at<int>(game, 0xa8) + 1, 0, 0, 0);
            }
            Ordinal_10911(list, 0x6fcbf7a0);   // retail remove callback address BF7A0
            Ordinal_10807(unit, list, 1);
        }
        Ordinal_11043(list, expire);
        int v = (int)Ordinal_10680(list, stat, 0) * remaining + add;
        if (i == 0) FUN_6fcaea80(game, unit, 0xc, expire, 0, 0, 0);
        if (remaining + len > 0) Ordinal_10188(list, stat, v / (remaining + len), 0);
        result = 1;
    }
    if (hp && unit && unit[0] == 0) {
        int cur = (int)Ordinal_10973(unit, 6, 0) >> 8;
        int mx = (int)Ordinal_10913(unit) >> 8;
        u8 pct = (mx > 0 && cur < mx) ? (u8)((cur << 7) / mx) : 0x80;
        FUN_6fc9a260(pct, unit);
        Ordinal_10887(unit, 0x160, pct, 0);
    }
    return result;
}

// ---- ADFA0: use a book or scroll for a skill (skill-do table 0x6fd274a8, entry 0x71) ----
EXT(Ordinal_10460); EXT(Ordinal_11147); EXT(Ordinal_11028); EXT(Ordinal_11065);
EXT(Ordinal_10698); EXT(Ordinal_10464);
EXT(FUN_6fcf9bf0);   // 0xD9BF0 use stored item (ecx unit, edx game, stack x, y, &out; eax guid), ret 0xc
EXT(FUN_6fcf8ba0);   // 0xD8BA0 use belt item (ecx guid, stack game, unit, x, y, 0; eax &out), ret 0x14

// 1.13c D2Game+0xADFA0  int __fastcall (ecx game, edx unit, stack skillId, skillLevel), ret 8
// 1.13c D2Game+0xADFB4  ADFA0's body past the empty-inventory return (not an entry point)
int D2GAME_ADFA0(u8* game, int* unit, int skillId, int skillLevel)
{
    (void)skillLevel;
    if (!unit[0x18]) return 0;
    for (intptr_t node = Ordinal_10460(unit[0x18]); node; node = Ordinal_10464(node)) {
        int* it = (int*)P(Ordinal_11147(node));
        u8* rec;
        if (Ordinal_10744(it, 0x12)) {
            rec = (u8*)P(Ordinal_11065((uint16_t)Ordinal_11028(it, 0)));
            if (!rec || at<int>(rec, 0xc) != skillId || Ordinal_10973(it, 0x46, 0) <= 0) continue;
        } else if (Ordinal_10744(it, 0x16)) {
            rec = (u8*)P(Ordinal_11065((uint16_t)Ordinal_11028(it, 0)));
            if (!rec || at<int>(rec, 8) != skillId) continue;
        } else continue;
        int page = (int)Ordinal_10698(node);
        int guid = it ? it[3] : -1;
        // retail passes &skillId (the dead argument slot) as the out pointer
        if (page == 1) {
            FUN_6fcf9bf0(unit, game, FUN_6fc21100(it), FUN_6fc21130(it), &skillId, guid);
            return 1;
        }
        if (page == 2) {
            FUN_6fcf8ba0(guid, game, unit, FUN_6fc21100(it), FUN_6fc21130(it), 0, &skillId);
            return 1;
        }
        return 0;
    }
    return 0;
}
