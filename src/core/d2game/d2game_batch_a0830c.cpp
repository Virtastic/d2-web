// src/core/d2game/d2game_batch_a0830c.cpp -- eleven 1.13c D2Game.dll skill-handler helpers transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000. Same conventions as
// d2game_batch_a0830.cpp: int-width pointer fields, D2Common ordinals and unported D2Game callees are weak env
// imports (EXT, retail argument order), register inputs the decompiler lost are explicit parameters marked REG?.
// D2Common ordinals used (1.13c, resolved by the link to our D2Common implementations): 10910 stat-with-layer
// getter, 10973 unit stat, 10887 set unit stat, 10689 stat-change notify, 10620 skill-id-of-item, 10551 skill
// on-item hook, 10913/10186 max-hp/max-mana style caps, 10255 is-item-or-player check, 10867/10750 owner-unit prep,
// 10871/10945/11108/11111 state lookup/remove (AD050 only), 10764/10983 path x/y.
// Argument lists re-derived from the 1.13c disassembly (callers' pushes, register setup, ret n); parameter order is
// ecx, edx, stack, then eax, ebx, esi, edi. Event callbacks (table 0x6fd277a8) are
// int __fastcall(game, event, attacker, unit, damage, packed, arg5), ret 0x14: ACA70 ACB10 ACBA0 ACE60 ADBF0 ADDC0.
// Item-use handlers are int __fastcall(game, unit, item, target, x, y, skillId), ret 0x14: ACA20.
// The rows the decompile could not carry live in d2game_batch_a0830d.cpp. No test runner run.
#include <cstdint>
#include <cstdlib>
#include <unistd.h>

#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)
// EXTA: an env import other TUs declare with a fixed signature; a namespaced symbol keeps this file's
// variadic calls without a wasm-ld signature clash on the shared C symbol (pattern of d2game_batch_68d70.cpp).
#define EXTA(n) namespace b10_env { __attribute__((weak, import_module("env"), import_name(#n))) intptr_t n(...); } using namespace b10_env
EXT(Ordinal_10265); EXT(Ordinal_10024); EXT(Ordinal_10867); EXT(Ordinal_10750);
EXTA(Ordinal_10910); EXT(Ordinal_10973); EXT(Ordinal_10913); EXT(Ordinal_10186); EXT(Ordinal_10887);
EXT(Ordinal_10689); EXT(Ordinal_10620); EXT(Ordinal_10551); EXT(Ordinal_10255); EXTA(Ordinal_10764);
EXTA(Ordinal_10983); EXT(Ordinal_10871); EXT(Ordinal_10945); EXT(Ordinal_11108); EXT(Ordinal_11111);
EXTA(Ordinal_11139); EXT(Ordinal_10326); EXT(Ordinal_10744); EXT(Ordinal_10814);
EXT(FUN_6fcae900);   // 0x8E900 skill-event begin
EXTA(FUN_6fcaea80);   // 0x8EA80 event queue add
EXT(FUN_6fcbe970);   // 0x9E970 skill duration/frame calc
EXT(FUN_6fcc19a0);   // 0xA19A0 skill record dispatch
EXT(FUN_6fcc0c70);   // 0xA0C70 skill-use core
EXT(FUN_6fd11730);   // 0xF1730 cast at a point (stack skill, level, y, flag; eax caster, ebx x)
EXT(FUN_6fd117e0);   // 0xF17E0 cast at a unit (ecx caster; stack skill, level, flag; eax target)
EXT(FUN_6fc21420);   // 0x1420 skill row by id (eax)
EXT(FUN_6fc21320);   // 0x1320 monster row by class id (eax)
EXT(FUN_6fc2e5d0);   // 0xE5D0
EXT(FUN_6fc31490);   // 0x11490
EXT(FUN_6fcf60d0);   // 0xD60D0
extern "C" char* sgptDataTables_exref;
extern "C" uint32_t gdwBitMasks_exref[32];

typedef uint8_t u8;
template <class T> static inline T& at(const void* p, int off) { return *(T*)((u8*)p + off); }
// The decompile indexes gdwBitMasks_exref as a byte array (gdwBitMasks_exref[0x14] cast to byte): byte offset into
// the little-endian u32 table, so [0x14] is dword 5 (0x20), [0x1c] is dword 7 (0x80).
static inline uint32_t bm8(int off) { return *((u8*)gdwBitMasks_exref + off); }
static inline uint32_t bm32(int off) { return *(uint32_t*)((u8*)gdwBitMasks_exref + off); }
static inline u8* tables() { return (u8*)(intptr_t)*(int*)sgptDataTables_exref; }
static inline u8* skillRow(int id) {
    u8* t = tables();
    if (id < 0 || at<int>(t, 0xba0) <= id) return nullptr;
    return (u8*)(intptr_t)(id * 0x23c + at<int>(t, 0xb98));
}
static inline void seedStep(void* seed) {
    uint64_t s = (uint64_t)at<uint32_t>(seed, 0) * 0x6ac690c5ull + at<uint32_t>(seed, 4);
    at<uint64_t>(seed, 0) = s;
}
// Shared head of ADB30/AD050: unit present, id in range, row flag 0x20 set, row+0x80 a valid index below +0xc4.
static inline u8* eventSkillRow(void* unit, int id) {
    if (!unit || id < 0) return nullptr;
    u8* t = tables();
    if (id >= at<int>(t, 0xba0)) return nullptr;
    u8* row = skillRow(id);
    if (!row || !(bm8(0x14) & at<u8>(row, 4))) return nullptr;
    int s = at<int16_t>(row, 0x80);
    if (s < 0 || s >= at<int>(t, 0xc4)) return nullptr;
    return row;
}
static void fatal(int code, int line) {
    intptr_t c = Ordinal_10265(code); (void)line;
    Ordinal_10024("D2Game.dll", c);          // retail string DAT_6fd19ff9 is not in our image
    _exit(-1);
}

// 1.13c D2Game+0xA1000  (stack a1, a2, a3; esi = unit), ret 0xc
// Builds a 5-dword search context {result 0, best 0x7fffffff, unit x, unit y, a3} and hands it to the A0C70 room
// walk with callback 0x6fcbedb0 (a retail address, passed through unchanged); returns the context's result.
int D2GAME_A1000(int a1, int a2, int a3, int* unit)
{
    int ctx[5];
    ctx[0] = 0;
    ctx[1] = 0x7fffffff;
    int k = *unit;
    bool direct = k == 2 || (k > 3 && k <= 5);
    int path = unit[0xb];
    ctx[2] = direct ? at<int>((void*)(intptr_t)path, 0xc) : (path ? (int)Ordinal_10867(path) : 0);
    k = *unit;
    direct = k == 2 || (k > 3 && k <= 5);
    path = unit[0xb];
    ctx[3] = direct ? at<int>((void*)(intptr_t)path, 0x10) : (path ? (int)Ordinal_10750(path) : 0);
    ctx[4] = a3;
    FUN_6fcc0c70(unit, 0, 0, a2, 0x8783, 0x6fcbedb0, ctx, 0, a1 /*eax*/);
    return ctx[0];
}

// 1.13c D2Game+0xA1420  (stack game, itemCode; ebx = unit), ret 8
// Gives a player a fresh item of itemCode in whichever hand slot (body loc 4 or 5) is free.
int D2GAME_A1420(void* game, int itemCode, int* unit)
{
    if (!unit || *unit != 0) return 0;
    int inv = unit[0x18];
    int a = (int)Ordinal_11139(inv, 4);
    int b = (int)Ordinal_11139(inv, 5);
    uint8_t code;
    if (a == 0) {
        if (b == 0) { code = 4; goto go; }
        if (!Ordinal_10326(b) && !Ordinal_10744(b, 0x2d)) { code = 4; goto go; }
    }
    if (b == 0 && a != 0) {
        if (!Ordinal_10326(a) && !Ordinal_10744(a, 0x2d)) { code = 5; goto go; }
    }
    return 0;
go: {
    int s[0x21] = {0};                                  // item-create request
    s[0] = (int)(intptr_t)unit;
    s[2] = (int)(intptr_t)game;
    at<uint16_t>(s, 0x2a) = at<uint16_t>(game, 0x78);
    s[5] = (int)Ordinal_10814(itemCode);
    if (s[5] == -1) fatal(0xf4c, 0);
    s[7] = 0; s[8] = 0; s[9] = 0;
    s[6] = 4;
    at<uint16_t>(s, 0x28) = 1;
    s[0xc] = 2;
    s[3] = (int)FUN_6fc2e5d0(0, unit /*eax*/);
    int* item = (int*)FUN_6fc31490(game, s, 0);
    if (!item) fatal(0xf55, 0);
    int out = 0;                                        // retail passes an uninitialised local as the out slot
    FUN_6fcf60d0(unit, game, code, 0, &out, item[3] /*eax guid*/);
    return 1;
  }
}

// 1.13c D2Game+0xACA20  item-use handler (game, unit, item, target, x, y, skillId), ret 0x14
int D2GAME_ACA20(void* game, int* unit, int* item, int* target, int x, int y, int skillId)
{
    (void)game; (void)target; (void)x; (void)y; (void)skillId;
    int id = (int)Ordinal_10620(item);
    if (id >= 0 && id < at<int>(tables(), 0xbd4)) {
        int v = (int)Ordinal_10973(item, 0x47, 0);
        if (v) { Ordinal_10551(unit, id, v, 0); return 1; }
    }
    return 0;
}

// Life (6/0x97) or mana (8/0x98) leech onto the attacker, capped by its max.
static int leech(int* attacker, uint32_t packed, int stat, int notify, intptr_t (*cap)(...))
{
    int amt = (int)Ordinal_10910(attacker, packed >> 16, packed);
    if (!amt) return 0;
    uint32_t mx = (uint32_t)cap(attacker);
    int cur = (int)Ordinal_10973(attacker, stat, 0);
    if ((int)mx <= cur) return 0;
    uint32_t v = (uint32_t)(cur + amt * 0x100);
    v &= ((int)v < 1) - 1;                              // clamp negatives to 0
    if ((int)mx <= (int)v) v = mx;
    Ordinal_10887(attacker, stat, v, 0);
    Ordinal_10689(attacker, notify, 0);
    return 1;
}

// 1.13c D2Game+0xACA70  event callback (event 0x18), ret 0x14
int D2GAME_ACA70(void* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)game; (void)event; (void)damage; (void)arg5;
    if (!attacker || !unit) return 0;
    if (!Ordinal_10255(unit)) return 0;
    return leech(attacker, packed, 6, 0x97, Ordinal_10913);
}

// 1.13c D2Game+0xACB10  event callback (event 0x28), ret 0x14
int D2GAME_ACB10(void* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)game; (void)event; (void)damage; (void)arg5;
    if (!attacker || !unit) return 0;
    return leech(attacker, packed, 6, 0x97, Ordinal_10913);
}

// 1.13c D2Game+0xACBA0  event callback (event 0x17), ret 0x14: mana twin of ACB10, attacker must be a player.
int D2GAME_ACBA0(void* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)game; (void)event; (void)damage; (void)arg5;
    if (!attacker || !unit || *attacker != 0) return 0;
    return leech(attacker, packed, 8, 0x98, Ordinal_10186);
}

// 1.13c D2Game+0xACE60  event callback (event 0x07), ret 0x14
// Rolls the attacker's seed; on (lo & 0x7f) < threshold sets damage flag 8 (0x20 / 0x80 / 0x40 by monster row).
int D2GAME_ACE60(void* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)game; (void)event; (void)arg5;
    if (!attacker || !unit || !damage) return 0;
    int amt = (int)Ordinal_10910(attacker, packed >> 16, packed);
    if (amt <= 0) return 0;
    uint32_t th = 0x40;
    if (*unit == 1) {
        u8* r = (u8*)FUN_6fc21320(unit[1] /*eax*/);
        if (r) {
            if (bm32(0xc) & at<u8>(r, 5)) th = 0x20;
            else if (bm32(8) & at<u8>(r, 5)) th = 0x80;
        }
    }
    void* seed = (u8*)attacker + 0x20;
    seedStep(seed);
    if ((at<uint32_t>(seed, 0) & 0x7f) < th) { at<u8>(damage, 4) |= 8; return 1; }
    return 0;
}

// 1.13c D2Game+0xADB30  (stack game, a2, skillId, a4; edi = unit), ret 0x10
void D2GAME_ADB30(void* game, int a2, int skillId, int a4, int* unit)
{
    u8* row = eventSkillRow(unit, skillId);
    if (!row) return;
    FUN_6fcae900(game, a2, 9, unit /*eax*/);
    uint32_t d = (uint32_t)FUN_6fcbe970(skillId, row, game, unit, a4 /*eax*/);
    FUN_6fcaea80(game, unit, 9, d, 0, a2, skillId);
    if (bm8(0x1c) & at<u8>(row, 5)) FUN_6fcc19a0(game, skillId, a4, 1, 1, 0, unit /*eax*/);
}

// 1.13c D2Game+0xAD050  (stack game, a2; eax = skillId, edi = unit), ret 8
// The state ordinals take the skill row's state (row+0x80), not the skill id.
void D2GAME_AD050(void* game, int a2, int skillId, int* unit)
{
    u8* row = eventSkillRow(unit, skillId);
    if (!row) return;
    int state = at<int16_t>(row, 0x80);
    Ordinal_10945(unit, state, 0);
    intptr_t list = Ordinal_10871(unit, state);
    if (list) { Ordinal_11111(unit, list); Ordinal_11108(list); }
    FUN_6fcae900(game, a2, 9, unit /*eax*/);
}

// 1.13c D2Game+0xADBF0  event callback (event 0x30), ret 0x14
// Chance roll on the attacker's seed; casts the packed skill/level at the unit (F17E0) or, with no unit, at the
// attacker's own path position (F1730).
int D2GAME_ADBF0(void* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)game; (void)event; (void)damage; (void)arg5;
    if (!attacker) return 0;
    int chance = (int)Ordinal_10910(attacker, packed >> 16, packed);
    if (chance <= 0) return 0;
    u8* t = tables();
    int lo = (int)(packed & 0xffff);
    int level = at<int>(t, 0xc70) & lo;
    int id = lo >> (at<uint8_t>(t, 0xc6c) & 0x1f);
    seedStep((u8*)attacker + 0x20);
    if ((int)(at<uint32_t>(attacker, 0x20) % 100) >= chance) return 0;
    if (id < 0 || !skillRow(id)) return 0;
    if (unit) { FUN_6fd117e0(attacker, id, level, 0, unit /*eax*/); return 1; }
    int path = attacker[0xb];
    intptr_t x = Ordinal_10764(path);
    intptr_t y = Ordinal_10983(attacker[0xb]);
    FUN_6fd11730(id, level, y, 0, attacker /*eax*/, x /*ebx*/);
    return 1;
}

// 1.13c D2Game+0xADDC0  event callback (event 0x20), ret 0x14
// As ADBF0, gated on damage flag 0x20 when damage is given. Skills with row bit 0x20 at +7 are cast by the unit on
// itself (flag 0); others from the attacker at the unit or its own path position (flag 1).
int D2GAME_ADDC0(void* game, int event, int* attacker, int* unit, int* damage, uint32_t packed, int arg5)
{
    (void)game; (void)event; (void)arg5;
    if (!attacker) return 0;
    int chance = (int)Ordinal_10910(attacker, packed >> 16, packed);
    if (chance <= 0) return 0;
    u8* t = tables();
    int lo = (int)(packed & 0xffff);
    int level = at<int>(t, 0xc70) & lo;
    int id = lo >> (at<uint8_t>(t, 0xc6c) & 0x1f);
    if (damage && !(at<u8>(damage, 0) & 0x20)) return 0;
    seedStep((u8*)attacker + 0x20);
    if ((int)(at<uint32_t>(attacker, 0x20) % 100) >= chance) return 0;
    u8* row = (u8*)FUN_6fc21420(id /*eax*/);
    if (!row) return 0;
    if (bm8(0x14) & at<u8>(row, 7)) {
        if (unit) FUN_6fd117e0(unit, id, level, 0, unit /*eax*/);
        return 1;
    }
    if (unit) { FUN_6fd117e0(attacker, id, level, 1, unit /*eax*/); return 1; }
    intptr_t x = Ordinal_10764(attacker[0xb]);
    intptr_t y = Ordinal_10983(attacker[0xb]);
    FUN_6fd11730(id, level, y, 1, attacker /*eax*/, x /*ebx*/);
    return 1;
}
