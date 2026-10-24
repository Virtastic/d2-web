// src/core/d2game/d2game_batch_39e30.cpp -- eleven 1.13c D2Game.dll functions (0x39E30..0x3BDA0: the
// mercenary/pet potion + heal helpers and the monster spawn-preset callbacks) written from the
// disassembly of the retail image (the cached Ghidra decompiles drop every register argument and most
// stack arguments of the D2Common thunks, so the argument lists below are read off the call sites and
// the thunk targets' own `ret N`).
//
// Retail records are opaque host objects read at their retail byte offsets. Every callee src/ does not
// own is a seam on D2GameBatchFEnv, in retail argument order, with the register arguments named:
//   getUnit       0xE03A0  SUNIT_GetServerUnit(ecx=pGame, edx=guid, eax=type)
//   getOwner      0xE0A20  (eax=unit, stack pGame) -> flag10 ? getUnit(pGame, unit[0x98], unit[0x94]) : 0
//   unitX/unitY   0x1100 / 0x1130 (ecx=unit)
//   randMod       0x11D0 (ecx=pSeed, esi=range) -> seed roll modulo range
//   mulDiv        0x14D0 (eax * edx / ecx, 0 when ecx == 0)
//   rangedStat    0x39B40 (ecx=bonusStat|-1, eax=statA, stack unit, statB)
//   addToField    0x39260 (eax=unit, esi=record, edi=value)
//   D2Common thunks by ordinal: stdcall, argument counts from the export's `ret N`.
// 1.10f crosswalk: D2MOO SKILLS/SkillItem.cpp potion handlers + MONSTER/MonsterSpawn (prior art).
#include <cstdint>
#include <cstring>
#include "MISSILES/MissMode.h"
#include "MONSTER/MonsterRegion.h"
#include "D2Monsters.h"
#include "D2StatList.h"
#include "D2Inventory.h"

void __fastcall MISSMODE_ResetGuidedFlight(D2UnitStrc* pMissile, D2UnitStrc* pTarget);   // 1.13c D2Game+0x397C0

int32_t __stdcall STATLIST_GetTotalStatValue(D2StatListStrc* pStatList, int nStatId, uint16_t nLayer);   // 1.13c D2Common+0x38BD0

// 1.13c D2Common+0x38B70 (#10973), stdcall (unit, stat, layer) -- ret 0xC (the cached decompile drops the layer; the disassembly
// pushes the word at [esp+0x18] into the key). null unit / stat list -> 0; stat bounds + row + the total (FullStats when the list is
// an Ex list, min-clamped) are STATLIST_GetTotalStatValue, which is the 0x38A80 body keyed (stat << 16) + layer.
static int32_t D2Common_38B70_GetUnitStat(D2UnitStrc* pUnit, int32_t nStatId, uint16_t nLayer)
{
    if (!pUnit || !pUnit->pStatListEx) return 0;
    return STATLIST_GetTotalStatValue(pUnit->pStatListEx, nStatId, nLayer);
}

static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

struct D2GameBatchFEnv {
    const void* (*dataTables)();                                   // *sgptDataTables
    void*    (*getUnit)(void* pGame, uint32_t guid, uint32_t type);
    void*    (*getOwner)(void* unit, void* pGame);
    uint32_t (*unitX)(void* unit);
    uint32_t (*unitY)(void* unit);
    int32_t  (*randMod)(void* pSeed, int32_t range);
    int32_t  (*mulDiv)(int32_t a, int32_t b, int32_t c);
    int32_t  (*rangedStat)(int32_t bonusStat, int32_t statA, void* unit, int32_t statB);
    void     (*addToField)(void* unit, void* rec, int32_t value);
    int32_t  (*getStat)(void* unit, int32_t stat, int32_t layer);           // #10973
    void     (*setStat)(void* unit, int32_t stat, int32_t value, int32_t layer); // #10887
    int32_t  (*maxHp)(void* unit);                                           // #10913
    int32_t  (*ord10240)(void* unit);
    int32_t  (*ord11029)(void* unit);
    int32_t  (*ord10567)(void* a, int32_t b, int32_t c, int32_t d);
    int32_t  (*ord10297)(void* a, int32_t b, int32_t c, int32_t d);
    void     (*ord10689)(void* unit, int32_t v, int32_t layer);
    void*    (*ord10331)(void* unit);
    int32_t  (*ord10867)(void* p);
    int32_t  (*ord10750)(void* p);
    void     (*ord10533)(void* a, void* b, void* c, int32_t d, int32_t e);
    int32_t  (*ord10494)(void* unit, int32_t v);
    int32_t  (*ord11104)(void* unit);
    void     (*ord10637)(void* a, int32_t b);
    void     (*ord10346)(void* a, int32_t b);
    int32_t  (*ord10239)(void* p);
    int32_t  (*ord10255)(void* p);
    uint8_t  (*ord10557)(uint32_t classId);
    uint32_t (*ord11018)(void* unit);
    // 0xA0000 (esi=owner; stack pGame, r10240, r11029, word, y1100, x1130) -> record or null
    void*    (*spawnA0000)(void* owner, void* pGame, int32_t a2, int32_t a3, int32_t a4, uint32_t a5, uint32_t a6);
    // 0xA0F40 (ecx=-1, eax=3, edx=rec+0x3C, edi=0; stack pGame, owner, y1100, x1130)
    void*    (*spawnA0F40)(int32_t ecx, int32_t eax, int32_t edx, void* pGame, void* owner, uint32_t a3, uint32_t a4, void* edi);
    // 0x397C0 (esi=unit, stack target)
    void     (*resetGuidedFlight)(void* unit, void* target);
    // 0x3AE10 (ecx, edx, stack a1, a2, a3)
    void     (*setDamageFlags)(void* ecx, void* edx, void* a1, void* a2, void* a3);
    // 0x47CF0 (ecx=r10331(unit), eax=rec+0x4C; stack pGame, y1100, x1130, n)
    void     (*spawnPreset)(void* ecx, int32_t eax, void* pGame, uint32_t a2, uint32_t a3, int32_t n);
    // 0xA24C0 (eax=owner; stack pGame, r10331, val2, val1, arg2, 0, 0x3C, 1)
    void     (*spawnA24C0)(void* owner, void* pGame, void* r, int32_t val2, int32_t val1, void* arg2, int32_t z, int32_t k, int32_t one);
};

// Default env: the seams whose callee is a DEFINED typed function in src/ are direct calls (D2Common ordinal ->
// function list d2common.tsv ordinal column). Seams left null have no usable definition yet (gaps, per function):
//   getUnit/getOwner 0xE03A0/0xE0A20 (no decompile of 0xE0A20 cached), randMod 0x11D0, mulDiv, rangedStat 0x39B40 (arg order),
//   ord10567/10297/10533/10346/10689 (env/probe-ABI definitions),
//   ord10494 (probe), ord10867/10750 (probes), spawnA0000/spawnA0F40/spawnA24C0, setDamageFlags (4-arg typed vs 5-arg seam), addToField.
// UNITS_GetX/YPosition is the typed form of the 0x1100/0x1130 accessor.
static const D2GameBatchFEnv g_batchFEnv = [] {
    D2GameBatchFEnv e{};
    e.dataTables = []() -> const void* { return sgptDataTables; };
    e.unitX = [](void* u) -> uint32_t { return (uint32_t)UNITS_GetXPosition((D2UnitStrc*)u); };
    e.unitY = [](void* u) -> uint32_t { return (uint32_t)UNITS_GetYPosition((D2UnitStrc*)u); };
    e.getStat = [](void* u, int32_t s, int32_t l) -> int32_t { return D2Common_38B70_GetUnitStat((D2UnitStrc*)u, s, (uint16_t)l); };
    e.setStat = [](void* u, int32_t s, int32_t v, int32_t l) { STATLIST_SetUnitStat((D2UnitStrc*)u, s, v, (uint16_t)l); };
    e.maxHp = [](void* u) -> int32_t { return STATLIST_GetMaxLifeFromUnit((D2UnitStrc*)u); };
    e.ord10240 = [](void* u) -> int32_t { return MISSILE_GetSkill((D2UnitStrc*)u); };
    e.ord11029 = [](void* u) -> int32_t { return (int32_t)MISSILE_GetLevel((D2UnitStrc*)u); };
    e.ord10331 = [](void* u) -> void* { return UNITS_GetRoom((D2UnitStrc*)u); };
    e.ord11104 = [](void* u) -> int32_t { return MONSTERS_GetHirelingTypeId((D2UnitStrc*)u); };
    e.ord10637 = [](void* m, int32_t v) { MISSILE_SetTargetX((D2UnitStrc*)m, v); };
    e.ord10239 = [](void* u) -> int32_t { return MONSTERS_IsUndead((D2UnitStrc*)u); };
    e.ord10255 = [](void* u) -> int32_t { return MONSTERS_IsDemon((D2UnitStrc*)u); };
    e.ord10557 = [](uint32_t id) -> uint8_t { return MISSILE_GetElemTypeFromMissileId((int)id); };
    e.ord11018 = [](void* u) -> uint32_t { return (uint32_t)MISSILE_GetTargetX((D2UnitStrc*)u); };
    e.resetGuidedFlight = [](void* m, void* t) { MISSMODE_ResetGuidedFlight((D2UnitStrc*)m, (D2UnitStrc*)t); };
    e.spawnPreset = [](void* room, int32_t cls, void* g, uint32_t x, uint32_t y, int32_t n) {
        D2GAME_SpawnPresetMonster_6FC66560((D2GameStrc*)g, (D2ActiveRoomStrc*)room, cls, (int32_t)x, (int32_t)y, n); };
    return e;
}();

// sgptDataTables record accessors: +0xB64/+0xB6C monsters (0x1A4), +0xB98/+0xBA0 (0x23C), +0xA80 count.
static const void* MonRec(const D2GameBatchFEnv* e, int32_t i)
{
    const void* dt = e->dataTables();
    if (i < 0 || i >= (int32_t)rd32(dt, 0xB6C)) return nullptr;
    return (const uint8_t*)rdptr(dt, 0xB64) + (uint32_t)i * 0x1A4;
}
static inline bool Flag10(const void* u) { return (rd32(u, 0xC8) >> 10) & 1; }
static inline bool PetLike(uint32_t t) { return t == 2 || (t > 3 && t < 6); }
static inline void* UnitOwner(const D2GameBatchFEnv* e, void* pGame, const void* u)
{
    return e->getUnit(pGame, rd32(u, 0x98), rd32(u, 0x94));
}
// The 0x10 / 0xC pair 3A1D0 and 3BDA0 read off the owner (pet record at +0x2C).
static void OwnerPair(const D2GameBatchFEnv* e, const void* o, int32_t* v1, int32_t* v2)
{
    void* sub = rdptr(o, 0x2C);
    const uint32_t t = rd32(o, 0);
    if (PetLike(t)) { *v1 = (int32_t)rd32(sub, 0x10); *v2 = (int32_t)rd32(sub, 0xC); return; }
    *v1 = sub ? e->ord10750(sub) : 0;
    *v2 = sub ? e->ord10867(sub) : 0;
}

// 1.13c D2Game+0x39E30
// (ecx=pGame, edx=unit, stack target) -> bool: hit-point heal of target by a rolled owner value.
int32_t D2Game_39E30(void* pGame, void* unit, void* target, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    if (!unit) return 1;
    const void* rec = MonRec(e, (int32_t)rd32(unit, 4));
    if (!rec || !target) return 1;
    const int32_t cls = e->ord10240(unit);
    const int32_t v   = e->ord11029(unit);
    const void* dt = e->dataTables();
    if (cls < 0 || cls >= (int32_t)rd32(dt, 0xBA0)) return 1;
    void* owner = e->getOwner(unit, pGame);
    const int32_t lo = e->ord10567(owner, cls, v, 1);
    owner = e->getOwner(unit, pGame);
    const int32_t hi = e->ord10297(owner, cls, v, 1);
    const int32_t roll = e->randMod((uint8_t*)unit + 0x20, hi - lo) + lo;
    const int32_t hp = e->getStat(target, 6, 0);
    const int32_t mx = e->maxHp(target);
    const int16_t snd = (int16_t)rd16(rec, 0x36);
    if (hp != mx && snd > 0) e->ord10689(target, snd, 0);
    int32_t n = hp + roll;
    if (n >= mx) n = mx;
    e->setStat(target, 6, n, 0);
    return ((const uint8_t*)rec)[0x186] != 0;
}

// 1.13c D2Game+0x39F70
// (ecx=pGame, edx=unit, stack target) -> 0 when target is the unit's owner and passes #10494(,0xD)
int32_t D2Game_39F70(void* pGame, void* unit, void* target, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    if (!target || !unit || !Flag10(unit)) return 2;
    void* owner = UnitOwner(e, pGame, unit);
    if (!owner) return 2;
    if (target == owner && e->ord10494(owner, 0xD) != 0) return 0;
    return 2;
}

// 1.13c D2GAME+0x3A1D0
// (ecx=pGame, ebx=unit) void
void D2Game_3A1D0(void* pGame, void* unit, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    void* owner = (unit && Flag10(unit)) ? UnitOwner(e, pGame, unit) : nullptr;
    void* sub = unit ? rdptr(unit, 0x2C) : nullptr;
    if (!owner || !sub) return;
    int32_t v1, v2;
    OwnerPair(e, owner, &v1, &v2);
    e->ord10533(sub, unit, e->ord10331(owner), v2, v1);
}

// 1.13c D2Game+0x3A280
// (esi=record, edi=unit, stack target) -> value; fills the per-type slot of the 0x48-byte record.
int32_t D2Game_3A280(void* rec, void* unit, void* target, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    if (!unit) return 0;
    const uint8_t t = e->ord10557(rd32(unit, 4));
    auto stat = [&](int32_t s) { return e->getStat(unit, s, 0); };
    switch (t) {
    case 0: {
        int32_t v = e->rangedStat(-1, 0x16, unit, 0x15);
        wr32(rec, 8, (uint32_t)v);
        if (target) {
            int32_t b = stat(0x19);
            if (e->ord10255(target)) b += stat(0x79);
            if (e->ord10239(target)) b += stat(0x7A);
            if (b <= -0x5A) b = -0x5A;
            v = (int32_t)rd32(rec, 8);
            wr32(rec, 8, (uint32_t)(v * b / 100 + v));
        }
        if (stat(0x8D)) {
            ((uint8_t*)rec)[5] |= 0x20;
            wr32(rec, 8, rd32(rec, 8) << 1);
        }
        return (int32_t)rd32(rec, 8);
    }
    case 1: { int32_t v = e->rangedStat(0x149, 0x31, unit, 0x30); wr32(rec, 0x10, (uint32_t)v); return v; }
    case 2: { int32_t v = e->rangedStat(0x14A, 0x33, unit, 0x32); wr32(rec, 0x1C, (uint32_t)v); return v; }
    case 3: { int32_t v = e->rangedStat(0x165, 0x35, unit, 0x34); wr32(rec, 0x20, (uint32_t)v); return v; }
    case 4: {
        int32_t v = e->rangedStat(0x14B, 0x37, unit, 0x36);
        wr32(rec, 0x24, (uint32_t)v);
        wr32(rec, 0x30, (uint32_t)stat(0x38));
        return v;
    }
    case 5: {
        int32_t v = e->rangedStat(0x14C, 0x3A, unit, 0x39);
        wr32(rec, 0x28, (uint32_t)v);
        wr32(rec, 0x2C, (uint32_t)stat(0x3B));
        const int32_t d = stat(0x146);
        if (d > 1) wr32(rec, 0x2C, (uint32_t)((int32_t)rd32(rec, 0x2C) / d));
        return (int32_t)rd32(rec, 0x28) * 0x19;
    }
    case 6: wr32(rec, 0x38, (uint32_t)stat(0x3C)); return 0;
    case 7: wr32(rec, 0x3C, (uint32_t)stat(0x3E)); return 0;
    case 8: wr32(rec, 0x40, (uint32_t)stat(0x40)); return 0;
    case 9: wr32(rec, 0x44, (uint32_t)stat(0x42)); return 0;
    case 0xB: {
        int32_t v = e->rangedStat(0x149, 0x13D, unit, 0x13C);
        wr32(rec, 0x14, (uint32_t)v);
        wr32(rec, 0x18, (uint32_t)stat(0x13B));
        return v * 0x19;
    }
    case 0xC: {
        int32_t v = e->rangedStat(0x14B, 0x37, unit, 0x36);
        wr32(rec, 0x24, (uint32_t)v);
        wr32(rec, 0x34, (uint32_t)stat(0x38));
        return v;
    }
    default: return 0;
    }
}

// 1.13c D2Game+0x3A790
// (edx=unit, stack target, stack rec) void; adds the monster-row percent shares into the record.
void D2Game_3A790(void* unit, void* target, void* rec, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    if (!unit) return;
    const void* mr = MonRec(e, (int32_t)rd32(unit, 4));
    if (!mr) return;
    const int32_t val = D2Game_3A280(rec, unit, target, e);
    if ((int32_t)rd32(mr, 0x78) > 0 && e->ord10239(target))
        e->addToField(unit, rec, e->mulDiv((int32_t)rd32(mr, 0x78), val, 100));
    if ((int32_t)rd32(mr, 0x7C) > 0 && e->ord10255(target))
        e->addToField(unit, rec, e->mulDiv((int32_t)rd32(mr, 0x7C), val, 100));
}

// 1.13c D2Game+0x3A920
// (ecx=pGame, edx=unit, stack target) -> 1, or 2 + (monster row +0x186 != 0)
int32_t D2Game_3A920(void* pGame, void* unit, void* target, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    if (!unit) return 1;
    const void* mr = MonRec(e, (int32_t)rd32(unit, 4));
    if (!mr || !Flag10(unit)) return 1;
    void* owner = UnitOwner(e, pGame, unit);
    if (!owner || !target) return 1;
    uint8_t local[0x70] = {};
    int32_t v = D2Game_3A280(local, unit, target, e);
    if (v < 0) v = 0;
    const int32_t mx = e->maxHp(owner);
    int32_t n = e->getStat(owner, 6, 0) + v;
    if (n >= mx) n = mx;
    e->setStat(owner, 6, n, 0);
    return ((const uint8_t*)mr)[0x186] != 0 ? 3 : 2;
}

// 1.13c D2Game+0x3AB30
// (ecx=pGame, edx=unit, stack skip) -> 1 (0 when skip is set and the row word is non-negative)
int32_t D2Game_3AB30(void* pGame, void* unit, int32_t skip, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    if (!unit) return 1;
    const void* mr = MonRec(e, (int32_t)rd32(unit, 4));
    if (!mr || (int16_t)rd16(mr, 0x24) < 0) return 1;
    if (skip) return 0;
    const uint32_t x = e->unitY(unit);                             // 0x1130 pushed first
    const uint32_t y = e->unitX(unit);                             // 0x1100
    const int32_t r1 = e->ord11029(unit);
    const int32_t r2 = e->ord10240(unit);
    void* owner = e->getOwner(unit, pGame);
    void* out = e->spawnA0000(owner, pGame, r2, r1, (int16_t)rd16(mr, 0x24), y, x);
    if (out) e->ord10637(out, (int32_t)e->ord11018(unit));
    return 1;
}

// 1.13c D2Game+0x3AFF0
// (ecx, edx passthrough; eax, ebx, esi registers -> 0x3AE10 stack args) void
void D2Game_3AFF0(void* ecx, void* edx, void* eax, void* ebx, void* esi, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    e->setDamageFlags(ecx, edx, eax, ebx, esi);
    if (esi && rd32(esi, 0) == 1 && e->ord11104(esi) == 0) {
        const int32_t add = e->getStat(ebx, 0x78, 0);
        if (add != 0) {
            int32_t n = e->getStat(esi, 0x1F, 0) + add;
            if (n < 0) n = 0;
            e->setStat(esi, 0x1F, n, 0);
        }
    }
}

// 1.13c D2Game+0x3B070
// (eax=unit, stack pGame) -> 0 when the spawn ran, else 1
int32_t D2Game_3B070(void* unit, void* pGame, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    if (!unit) return 1;
    const void* mr = MonRec(e, (int32_t)rd32(unit, 4));
    if (!mr) return 1;
    const int32_t z = (int32_t)rd32(mr, 0x3C);
    if (e->ord11018(unit) & 4) return 1;
    if (!Flag10(unit)) return 1;
    void* owner = UnitOwner(e, pGame, unit);
    if (!owner) return 1;
    const uint32_t y = e->unitY(unit);                             // 0x1130
    const uint32_t x = e->unitX(unit);                             // 0x1100
    void* m = e->spawnA0F40(-1, 3, z, pGame, owner, x, y, nullptr);
    e->resetGuidedFlight(unit, m);
    return 0;
}

// 1.13c D2Game+0x3BD10
// (ecx=pGame, edx=unit) -> 1
int32_t D2Game_3BD10(void* pGame, void* unit, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    const void* mr = unit ? MonRec(e, (int32_t)rd32(unit, 4)) : nullptr;
    if (mr) {
        const int32_t a = (int32_t)rd32(mr, 0x4C);
        if (a >= 0 && a < (int32_t)rd32(e->dataTables(), 0xA80)) {
            int32_t n = (int32_t)rd32(mr, 0x50);
            if (n < 0 || n >= 0x10) n = 1;
            const uint32_t y = e->unitY(unit);                     // 0x1130
            const uint32_t x = e->unitX(unit);                     // 0x1100
            e->spawnPreset(e->ord10331(unit), a, pGame, x, y, n);
        }
    }
    return 1;
}

// 1.13c D2Game+0x3BDA0
// (esi=unit, stack pGame, stack arg2) void
void D2Game_3BDA0(void* unit, void* pGame, void* arg2, const D2GameBatchFEnv* e = &g_batchFEnv)
{
    e->ord10637(unit, 1);
    void* owner = (unit && Flag10(unit)) ? UnitOwner(e, pGame, unit) : nullptr;
    int32_t v1, v2;
    OwnerPair(e, unit, &v1, &v2);
    e->spawnA24C0(owner, pGame, e->ord10331(unit), v2, v1, arg2, 0, 0x3C, 1);
    e->ord10346(e->ord10331(unit), 1);
}

// 1.13c D2Game+0x42AD0
// (ecx=pGame, edx=unit, two unused stack args) -> bool: the unit has a D2Common room (#10331) and its
// target coordinates (0x9FA30) land in a game room (0x1280, eax=room, ebx=x, edi=y) that 0xA808 accepts
// (returns 0). Seams are in retail argument order.
struct D2Game42AD0Env {
    void*    (*ord10331)(void* unit);                                  // 0xA478
    int32_t  (*targetXY)(void* unit, void* pGame, uint32_t* x, uint32_t* y); // 0x9FA30
    void*    (*roomAt)(void* room, uint32_t x, uint32_t y);            // 0x1280
    int32_t  (*roomBlocked)(void* room);                               // 0xA808
};
int32_t D2Game_42AD0(void* pGame, void* unit, const D2Game42AD0Env* e)
{
    void* room = e->ord10331(unit);
    if (!room) return 0;
    uint32_t x, y;
    if (!e->targetXY(unit, pGame, &x, &y)) return 0;
    void* r = e->roomAt(room, x, y);
    if (!r) return 0;
    return e->roomBlocked(r) == 0;
}

// function probe seam (src/core/fn/d2game_3a280.cpp): the five callees replaced by recorders. cfg dwords: type, ranged, demon, undead, stat0..3
// (stat returns consumed in call order, last repeats); lg is the hook log (count, then 10 words per call: rva, eax, ecx, edx, ebx, esi, s0..s3).
static const uint32_t* g_p3a_cfg; static uint32_t* g_p3a_lg; static uint32_t g_p3a_n;
static void p3a_log(uint32_t rva, uint32_t ecx, uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2)
{
    uint32_t* w = g_p3a_lg + 1 + g_p3a_lg[0]++ * 10;
    w[0] = rva; w[1] = eax; w[2] = ecx; w[3] = w[4] = w[5] = 0; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = 0;
}
int32_t D2Game_3A280_probe(void* rec, void* unit, void* target, const uint32_t* cfg, uint32_t* lg)
{
    g_p3a_cfg = cfg; g_p3a_lg = lg; g_p3a_n = 0; lg[0] = 0;
    D2GameBatchFEnv e{};
    e.ord10557 = [](uint32_t c) -> uint8_t { p3a_log(0xB25E, 0, 0, c, 0, 0); return (uint8_t)g_p3a_cfg[0]; };
    e.ord10255 = [](void* p) -> int32_t { p3a_log(0xA814, 0, 0, (uint32_t)(uintptr_t)p, 0, 0); return (int32_t)g_p3a_cfg[2]; };
    e.ord10239 = [](void* p) -> int32_t { p3a_log(0xA508, 0, 0, (uint32_t)(uintptr_t)p, 0, 0); return (int32_t)g_p3a_cfg[3]; };
    e.getStat = [](void* u, int32_t s, int32_t l) -> int32_t {
        p3a_log(0xA42A, 0, 0, (uint32_t)(uintptr_t)u, (uint32_t)s, (uint32_t)l);
        return (int32_t)g_p3a_cfg[4 + (g_p3a_n < 3 ? g_p3a_n++ : 3)];
    };
    e.rangedStat = [](int32_t b, int32_t a, void* u, int32_t s) -> int32_t {
        p3a_log(0x39B40, (uint32_t)b, (uint32_t)a, (uint32_t)(uintptr_t)u, (uint32_t)s, 0);
        return (int32_t)g_p3a_cfg[1];
    };
    return D2Game_3A280(rec, unit, target, &e);
}

// function probe seam (src/core/fn/d2game_3bda0.cpp): the seven callees replaced by recorders. cfg dwords (unit+0x100): owner, v1 (#10750), v2 (#10867), room (#10331).
// lg is the hook log: count, then 10 words per call (rva, eax, ecx, edx, ebx, esi, s0..s3).
static const uint32_t* g_p3b_cfg; static uint32_t* g_p3b_lg; static void* g_p3b_game;
static void p3b_log(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3)
{
    uint32_t* w = g_p3b_lg + 1 + g_p3b_lg[0]++ * 10;
    w[0] = rva; w[1] = eax; w[2] = ecx; w[3] = edx; w[4] = w[5] = 0; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = s3;
}
static inline uint32_t p32(const void* p) { return (uint32_t)(uintptr_t)p; }
void D2Game_3BDA0_probe(void* unit, void* pGame, void* arg2, uint32_t* lg)
{
    g_p3b_cfg = (const uint32_t*)((uint8_t*)unit + 0x100); g_p3b_lg = lg; g_p3b_game = pGame; lg[0] = 0;
    D2GameBatchFEnv e{};
    e.ord10637 = [](void* u, int32_t v) { p3b_log(0xA50E, 0, 0, 0, p32(u), (uint32_t)v, 0, 0); };
    e.getUnit = [](void* g, uint32_t guid, uint32_t type) -> void* {
        p3b_log(0xE03A0, type, p32(g), guid, 0, 0, 0, 0); return (void*)(uintptr_t)g_p3b_cfg[0]; };
    e.ord10750 = [](void* p) -> int32_t { p3b_log(0xA3B8, 0, 0, 0, p32(p), 0, 0, 0); return (int32_t)g_p3b_cfg[1]; };
    e.ord10867 = [](void* p) -> int32_t { p3b_log(0xA454, 0, 0, 0, p32(p), 0, 0, 0); return (int32_t)g_p3b_cfg[2]; };
    e.ord10331 = [](void* u) -> void* { p3b_log(0xA478, 0, 0, 0, p32(u), 0, 0, 0); return (void*)(uintptr_t)g_p3b_cfg[3]; };
    e.spawnA24C0 = [](void* o, void* g, void* r, int32_t v2, int32_t v1, void*, int32_t, int32_t, int32_t) {
        p3b_log(0xA24C0, p32(o), 0, 0, p32(g), p32(r), (uint32_t)v2, (uint32_t)v1); };
    e.ord10346 = [](void* r, int32_t v) { p3b_log(0xA51A, 0, 0, 0, p32(r), (uint32_t)v, 0, 0); };
    D2Game_3BDA0(unit, pGame, arg2, &e);
}

// function probe seam (src/core/fn/d2game_3aff0.cpp): the four callees replaced by recorders; lg as for 3A280. cfg: hireling id, then 2 stat returns in call order.
static const uint32_t* g_p3f_cfg; static uint32_t g_p3f_n;
int32_t D2Game_3AFF0_probe(void* eax, void* ebx, void* esi, void* ecx, void* edx, const uint32_t* cfg, uint32_t* lg)
{
    g_p3f_cfg = cfg; g_p3f_n = 0; g_p3a_lg = lg; lg[0] = 0;
    D2GameBatchFEnv e{};
    e.setDamageFlags = [](void* c, void* d, void* a, void* b, void* s) {
        const uint32_t A = (uint32_t)(uintptr_t)a, B = (uint32_t)(uintptr_t)b, S = (uint32_t)(uintptr_t)s;
        p3a_log(0x3AE10, (uint32_t)(uintptr_t)c, A, A, B, S);   // stack words = eax, ebx, esi
        uint32_t* w = g_p3a_lg + 1 + (g_p3a_lg[0] - 1) * 10; w[3] = (uint32_t)(uintptr_t)d; w[4] = B; w[5] = S;
    };
    e.ord11104 = [](void* u) -> int32_t { p3a_log(0xA7F6, 0, 0, (uint32_t)(uintptr_t)u, 0, 0); return (int32_t)g_p3f_cfg[0]; };
    e.getStat = [](void* u, int32_t s, int32_t l) -> int32_t { p3a_log(0xA42A, 0, 0, (uint32_t)(uintptr_t)u, (uint32_t)s, (uint32_t)l); return (int32_t)g_p3f_cfg[1 + (g_p3f_n < 1 ? g_p3f_n++ : 1)]; };
    e.setStat = [](void* u, int32_t s, int32_t v, int32_t l) { p3a_log(0xA472, 0, 0, (uint32_t)(uintptr_t)u, (uint32_t)s, (uint32_t)v); (void)l; };
    D2Game_3AFF0(ecx, edx, eax, ebx, esi, &e);
    return 0;
}

// function probe seam (src/core/fn/d2game_3a1d0.cpp): the five callees replaced by recorders (p3b_log shape). cfg dwords (unit+0x40): owner, room.
// #10750/#10867 return the descriptor's scripted values read back off the owner's record (+0x10 / +0xC), as the vector stores them there.
void D2Game_3A1D0_probe(void* unit, void* pGame, uint32_t* lg)
{
    g_p3b_cfg = (const uint32_t*)((uint8_t*)unit + 0x40); g_p3b_lg = lg; lg[0] = 0;
    D2GameBatchFEnv e{};
    e.getUnit = [](void* g, uint32_t guid, uint32_t type) -> void* {
        p3b_log(0xE03A0, type, p32(g), guid, 0, 0, 0, 0); return (void*)(uintptr_t)g_p3b_cfg[0]; };
    e.ord10750 = [](void* p) -> int32_t { p3b_log(0xA3B8, 0, 0, 0, p32(p), 0, 0, 0); return (int32_t)rd32(p, 0x10); };
    e.ord10867 = [](void* p) -> int32_t { p3b_log(0xA454, 0, 0, 0, p32(p), 0, 0, 0); return (int32_t)rd32(p, 0xC); };
    e.ord10331 = [](void* u) -> void* { p3b_log(0xA478, 0, 0, 0, p32(u), 0, 0, 0); return (void*)(uintptr_t)g_p3b_cfg[1]; };
    e.ord10533 = [](void* s, void* u, void* r, int32_t v2, int32_t) { p3b_log(0xA6CA, 0, 0, 0, p32(s), p32(u), p32(r), (uint32_t)v2); };
    D2Game_3A1D0(pGame, unit, &e);
}
