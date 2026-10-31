// src/core/d2game/d2game_batch_97340.cpp -- seven 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Stage A batch lane: implementation and
// marker only, no probe shim and no vector yet.
//
// Conventions. Register-only inputs (Ghidra unaff_* / in_EAX / extraout_ECX) are explicit parameters named
// after the register. Stack and fastcall arguments keep the decompile's order. Callees src/ does not own
// are seams on D2GameBatch97340Env in the retail argument order the decompile shows at that call site.
// 0x97340 was checked against the retail disassembly (its stdcall ret 0xC, the game in EAX, the helper
// register and stack pushes); 0xBFFA0's call site pushes unit, x, y with the game in EAX.
//
// Prior art: none for these seven (1.13c-only pet, summon and skill-dispatch code); the 1.10f D2MOO
// PLAYER/PlayerPets.cpp sub_6FC7E310 is the shape of 0x97340, read as a shape only.
//
// MISMATCH notes are at each function. They are unverified transcriptions, not evidence.
#include <cstdint>
#include <cstddef>
#include <cstring>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// Host seams for every callee and data block the seven bodies reach that src/ does not own.
struct D2GameBatch97340Env {
    // *sgptDataTables_exref and gdwBitMasks (dword array; byte reads at +0x14 / +0x18 are the decompile's
    // gdwBitMasks[0x14] / [0x18] byte-indexed reads).
    void*           sgptDataTables;
    const uint8_t*  gdwBitMasks;

    // 0x97340 PetsReevaluateForPlayer.
    void*    (*ordinal10920)(void* unit);                                        // UNITS_GetPlayerData (0xA4C0)
    uint32_t (*ordinal10331)(void* unit);                                        // 0xA478
    void*    (*ordinal10331Ptr)(void* unit);                                     // 10331 as a pointer (0x9CDE0)
    void     (*sub972C0)(void* entry, void* game, uint32_t ecx, uint32_t x, uint32_t y);
    void     (*sub964B0)(void* entry, void* game);
    void     (*sub96C00)(void* game, void* unit, uint32_t limit, void* entry);

    // 0x98E90 SummonSpawnForOwner, 0x99620 SkilPalCompanionSpawn, 0x99830 SkillSummonUnitForOwner.
    void*    (*ordinal10511)();                                                  // skill gate / summon handle
    uint32_t (*sub_E07A0)(void* game);                                           // FUN_6fd007a0; returns the ECX it leaves
    void*    (*ordinal10392)(void* a);                                           // owner lookup (a == nullptr when none)
    int32_t  (*ordinal11138)(void* a, void* b, uint32_t c);
    uint32_t (*ordinal10653)(void* unit, uint32_t skillId, uint32_t zero, uint32_t zero2);
    uint16_t (*sub_DE5A0)(uint32_t ecx, uint32_t edx, void* unit);
    uint32_t (*ordinal10786)(void* unit, uint32_t value, uint32_t id, uint32_t zero);
    void     (*sub_DC530)(uint32_t a, void* unit, uint32_t b, uint32_t c, uint32_t d);
    void     (*sub_9F210)();
    void     (*sub_DDDE0)(uint32_t flags, void* nullArg, void* owner);
    int32_t  (*ordinal10494)(void* a, uint32_t b);
    void     (*ordinal10243)(void* a, uint32_t b);
    void     (*ordinal10554)(void* a, uint32_t b);
    void     (*ordinal11031)(void* a, uint32_t b);
    uint32_t (*ordinal10973)(void* a, uint32_t b, uint32_t c);
    void     (*ordinal10723)(void* a);
    void     (*ordinal10168)(void* a, uint32_t b);
    void     (*ordinal10137)(void* a, uint32_t b);
    void     (*ordinal10945)(void* a, uint32_t b, uint32_t c);
    void     (*ordinal10334)(uint32_t a, void* b, uint32_t c);
    void     (*ordinal10488)(uint32_t a, uint32_t b, const char* file, uint32_t line);
    uint32_t (*sub_CC2DE0)(void* owner);
    void     (*sub_FF980)(void* unit, uint32_t v);
    void     (*ordinal11103)(void* a, uint32_t b);
    int32_t  (*sub_18A0)();
    uint32_t (*sub_1850)();
    void     (*sub_75070)(uint32_t mode);
    uint32_t (*sub_75E10)(uint32_t act, void* local);
    void*    (*sub_10A0)();
    void*    (*sub_1220)();
    void     (*sub_1B90)();
    uint32_t (*sub_14D0)(uint32_t a, uint32_t b);
    uint32_t (*sub_988B0)(void* unit);
    uint32_t (*sub_DAA90)(uint32_t ecx, void* owner);
    void     (*sub_9F730)();
    void     (*sub_E0460)();
    void     (*sub_EFE40)(void* rec);
    void     (*sub_8EA80)(void* a, void* b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);
    void     (*sub_DED30)(void* game, void* a, void* b);
    const void* sub_98950;                                                       // callback passed to 10911
    uint32_t (*ordinal10871)(void* a, uint32_t b);
    void*    (*ordinal11013)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*ordinal10785)(void* a, uint32_t b);
    void     (*ordinal10911)(void* a, const void* cb);
    void     (*ordinal10807)(void* a, void* b, uint32_t c);
    void     (*ordinal11043)(void* a, uint32_t b);
    void     (*ordinal11129)(void* a, uint32_t b);
    void*    (*ordinal10195)(void* a, uint32_t b);
    void     (*ordinal11111)(void* a, void* b);
    void     (*ordinal11108)(void* a);
    uint32_t (*ordinal10913)(void* a);
    void     (*ordinal10188)(void* a, uint32_t b, uint32_t c, uint32_t d);
    void     (*ordinal10887)(void* a, uint32_t b, uint32_t c, uint32_t d);

    // 0x9CDE0 MonsterSkillRollAndCast.
    uint32_t (*sub_1130)(void* unit);
    uint32_t (*sub_1100)(void* unit);
    uint32_t (*sub_15D0)(uint32_t a, uint32_t b);
    uint32_t (*sub_1180)(uint64_t* seed);
    void*    (*sub_1280)();
    int32_t  (*ordinal11027)();
    int32_t  (*ordinal10224)(void* a, uint32_t b, uint32_t c);
    int32_t  (*sub_47450)(void* edi, void* a, int32_t b, int32_t c, int32_t* d, int32_t* e, uint32_t f);
    int32_t  (*ordinal10057)(void* a);
    int32_t  (*ordinal10494v)();
    void     (*ordinal10973v)();
    void     (*ordinal10913v)();
    void     (*ordinal10551v)();
    void     (*sub_F2380)(void* unit);

    // 0x9F600 MonstatMaxSkillReach.
    void*    (*sub_1B60)();
    void*    (*sub_1420)(uint32_t skillIndex);
    uint32_t (*ordinal10630)();
    int32_t  (*ordinal10306)(void* unit, uint32_t a, uint32_t b);
    void     (*sub_96EA0)(uint32_t param1, uint32_t value);

    // 0x9FD00 SkillUseDispatchByMonstat. The DAT_6fd27338 handler table is retail .data; the host supplies it.
    bool     (*hasSkillHandler)(int16_t code);
    int32_t  (*callSkillHandler)(int16_t code);
    void     (*sub_9EC80_unused)();
    bool     (*sub_9EC80)();
    int32_t  (*sub_9FA30)(void* retaddr, int32_t* param1);
    int32_t  (*ordinal10479)(uint32_t ebp, int32_t param1, void* unit, uint32_t id);
    void     (*sub_9F850)(void* unit, void* retaddr);
    void     (*sub_98550)(uint32_t ebp, void* unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*sub_8E6F0)();
};

// Skill-table record: sgptDataTables+0xB98 base, count at +0xBA0, record stride 0x23C.
static uint8_t* skillRecord(const D2GameBatch97340Env* env, int32_t skillId)
{
    if (skillId < 0 || skillId >= (int32_t)rd32(env->sgptDataTables, 0xBA0)) return nullptr;
    uint8_t* rec = (uint8_t*)rdptr(env->sgptDataTables, 0xB98) + (uint32_t)skillId * 0x23C;
    return rec;
}

// 1.13c D2Game+0x97340 -- PetsReevaluateForPlayer(unit, x, y) with the game in EAX (stdcall ret 0xC).
// Gate: the unit has a player-pets container (+0x44). Then ordinal 10331 on the unit once, and a loop over
// pet types 1 .. count-1 (count re-read each pass from sgpt+0xBF0). For each type with a list entry
// (array base + 0xC*i) and a record (type table +0xBE8 + 0xE0*i): flags = record byte +4; if the first
// gdwBitMasks dword ANDs it, 0x972C0; else if the second does not, 0x964B0; else 0x96C00 with 0x640.
// The 0x972C0 call gets ECX = the 10331 result, and x, y as its stack arguments.
// MISMATCH: none in control flow. The decompile's piStack_4 is the 10331 result (retail [esp-4]); the
// helper bodies read ECX/stack only through 0x972C0, which the 1EF50 port ignores.
void PetsReevaluateForPlayer_97340(void* pGame, void* pUnit, uint32_t x, uint32_t y,
                                   const D2GameBatch97340Env* env)
{
    void* pPlayerData = env->ordinal10920(pUnit);
    if (pPlayerData == nullptr) return;                       // MISMATCH: retail dereferences unguarded here
    void* pPets = rdptr(pPlayerData, 0x44);
    if (pPets == nullptr) return;

    const uint32_t ecx = env->ordinal10331(pUnit);
    for (uint32_t i = 1; i < rd32(env->sgptDataTables, 0xBF0); ++i) {
        void* entry = (uint8_t*)rdptr(pPets, 0) + 0xC * i;   // list i in the pet-list array
        if (entry == nullptr) continue;
        void* rec = (uint8_t*)rdptr(env->sgptDataTables, 0xBE8) + 0xE0 * i;
        if (rec == nullptr) continue;

        const uint32_t flags = rd8(rec, 4);
        if ((rd32(env->gdwBitMasks, 0) & flags) != 0)       env->sub972C0(entry, pGame, ecx, x, y);
        else if ((rd32(env->gdwBitMasks, 4) & flags) == 0)  env->sub964B0(entry, pGame);
        else                                                 env->sub96C00(pGame, pUnit, 0x640, entry);
    }
}

// 1.13c D2Game+0x98E90 -- SummonSpawnForOwner(game ECX, unit EDX, skillId stack); __fastcall, ret 4.
// Gate: skill id in range, record present, ordinal 10511 nonzero. Then 0xE07A0(game); the owner
// (ordinal 10392) must differ from the unit and be non-null; ordinal 11138 on the unit; then the
// 10653 / 0xDE5A0 roll. On success: 10786 with rec+0x138, 0xDC530, rec+0x144 when rec+0x1DC is set,
// 0x9F210. 0xDDDE0(0x80, 0, owner) runs on every path past the owner test; returns 1.
// MISMATCH: the decompile's uStack_c / uStack_18 are uninitialised stack slots of a zeroed 0x1C-dword
// block; passed here as 0. The 11138 arity differs by call site (1 arg here, 3 args in 0x99620); the
// seam takes three and this body passes zeros for the unused ones.
uint32_t SummonSpawnForOwner_98E90(void* pGame, void* pUnit, int32_t skillId,
                                   const D2GameBatch97340Env* env)
{
    uint8_t* rec = skillRecord(env, skillId);
    if (rec == nullptr) return 0;
    if (env->ordinal10511() == 0) return 0;

    env->sub_E07A0(pGame);
    void* owner = env->ordinal10392(nullptr);
    if (owner == pUnit || owner == nullptr) return 0;
    if (env->ordinal11138(pUnit, nullptr, 0) == 0) return 0;

    const uint32_t uVar4 = env->ordinal10653(pUnit, (uint32_t)skillId, 0, 0);
    if ((env->sub_DE5A0(0, uVar4, pUnit) & 1) != 0) {
        env->ordinal10786(pUnit, rd32(rec, 0x138), (uint32_t)skillId, 0);
        env->sub_DC530(0, pUnit, 1, 0, 0);
        if (rd8(rec, 0x1DC) != 0) env->ordinal10786(pUnit, rd32(rec, 0x144), (uint32_t)skillId, 0);
        env->sub_9F210();
    }
    env->sub_DDDE0(0x80, nullptr, owner);
    return 1;
}

// 1.13c D2Game+0x99620 -- SkilPalCompanionSpawn(game ECX, unit EDX, skillId stack); __fastcall, ret 4.
// Gate: skill id in range and record present; ordinal 10511 returns the summon handle (nonzero) and the
// unit's data block (+0x2C) must be non-null. Owner lookup (10392): when it is another unit, 11138 on
// (unit, owner, 0) decides the non-type-0 path (0xFF980, 11103, 0x18A0, 0x1850 -> mode, 0x75070, 0x75E10);
// type 0 returns 0xCC2DE0(owner). Otherwise 10494 picks the owner's type or 6. Then the summon setup
// (10554, 11031, 10723, 10168 on the non-zero-type branch), 0x10A0 or 0x1220 for the level, 10973, 0x1B90,
// 0x14D0(100, level), the assert-style 10488 with SkilPal.cpp line 0x330, 10334, 10137(0x1001),
// 10945(0x12, 1). Returns 1.
// MISMATCH: the decompile's iVar5 is the summon handle copied from iVar1 and read again on the non-zero-type
// branch; kept as iVar1. The Ghidra local stack0xffffffc4 passed to 0x75E10 is passed here as a zeroed block.
uint32_t SkilPalCompanionSpawn_99620(void* pGame, void* pUnit, int32_t skillId,
                                     const D2GameBatch97340Env* env)
{
    (void)pGame;
    uint8_t* rec = skillRecord(env, skillId);
    if (rec == nullptr) return 0;
    void* summon = env->ordinal10511();
    if (summon == nullptr || rdptr(pUnit, 0x2C) == nullptr) return 0;

    env->sub_E07A0(pGame);
    const uint32_t dataPtr = (uint32_t)(uintptr_t)rdptr(pUnit, 0x2C);
    void* owner = env->ordinal10392(nullptr);
    bool sawOwnerPath = false;

    if (owner != pUnit && owner != nullptr) {
        if (env->ordinal11138(pUnit, owner, 0) != 0) {
            if (rd32(pUnit, 0) == 0) return env->sub_CC2DE0(owner);
            env->sub_FF980(pUnit, 1);
            env->ordinal11103(pUnit, 0);
            env->sub_18A0();
            const uint32_t r = env->sub_1850();
            env->sub_75070(r == 0x49 ? 5u : (uint32_t)((r == 0xD3) + 4));
            uint32_t local[28] = {};
            return env->sub_75E10(dataPtr, local);
        }
        if (env->ordinal10494(owner, 0x36) == 0) {
            env->ordinal10243(summon, rd32(owner, 0));
            env->ordinal10554(summon, rd32(owner, 0x0C));
            sawOwnerPath = true;
        }
    }
    if (!sawOwnerPath) {
        env->ordinal10243(summon, 6);
        env->ordinal10554(summon, 0);
    }
    env->ordinal11031(summon, 7);

    uint32_t level = 0x100;
    if (rd32(pUnit, 0) == 0) {
        void* p = env->sub_10A0();
        if (p != nullptr) level = (uint32_t)rd8(p, 0x41) << 8;
    } else {
        const uint32_t type4 = rd32(pUnit, 4);
        void* p = env->sub_1220();
        if (p != nullptr) {
            level = (uint32_t)(int32_t)(int16_t)rd16(p, 0x34) << 8;
            if (type4 == 0xD3) env->ordinal11031(summon, 0xF);
            env->ordinal10723(summon);
            env->ordinal10168(summon, 0x14);
        }
    }
    env->ordinal10973(pUnit, 0x43, 0);
    env->sub_1B90();
    level = env->sub_14D0(100, level);
    env->ordinal10488(7, level, "..\\Source\\D2Game\\SKILLS\\SkilPal.cpp", 0x330);
    env->ordinal10334(7, pUnit, 0);
    env->ordinal10137(summon, 0x1001);
    env->ordinal10945(pUnit, 0x12, 1);
    return 1;
}

// 1.13c D2Game+0x99830 -- SkillSummonUnitForOwner(ECX param_1, EDX param_2, skill param_3 stack).
// Inputs in registers: EBX = game (unaff_EBX) and the summoner pointer that Ghidra shows as unaff_retaddr.
// The first check is the skill range and record. param_2 gets flag 0x40 (+0xC4). 0xE07A0(param_1) leaves
// an ECX value that feeds 0xDAA90. Owner != param_2 and non-null: 0xDAA90 chooses the param_1 or the
// record; the 0x988B0 path re-points param_2. When the record's +0x82 is in range and the pair is nonzero,
// 10786 gives the level, and the summon unit is created (11013) or looked up (10871) and set up with the
// 0x98950 callback. Then the pet list is emptied (10195/11111/11108). The 0xC/0x6D sub-branch sets the
// speed fields (0xB0/0xB1, 0xC/6/7). Otherwise 0xDED30(game, param_2, owner).
// MISMATCH: the decompile's uVar7 adds the record pointer to the 10786 result (retail arithmetic on an
// address-sized value, kept literal here); the Ghidra iVar2 / param_1 reassignments are kept as shown.
// Needs a disassembly pass before any vector: this body is the least certain of the seven.
uint32_t SkillSummonUnitForOwner_99830(void* ecxUnit, void* pUnit, int32_t skillId, void* game,
                                       void* summoner, const D2GameBatch97340Env* env)
{
    uint8_t* rec = skillRecord(env, skillId);
    if (rec == nullptr) return 0;
    if (pUnit != nullptr) wr32(pUnit, 0xC4, rd32(pUnit, 0xC4) | 0x40);

    const uint32_t ecxOut = env->sub_E07A0(ecxUnit);
    void* owner = env->ordinal10392(rdptr(pUnit, 0x2C));
    if (owner == pUnit || owner == nullptr) return 0;

    void* p1 = ecxUnit;
    void* p2 = pUnit;
    uint32_t iVar2 = env->sub_DAA90(ecxOut, owner);
    if (iVar2 == 0) {
        iVar2 = 0;
        p1 = rec;
    } else {
        iVar2 = env->sub_988B0(pUnit);
        p2 = rec;
    }
    const int16_t s = (int16_t)rd16(p1, 0x82);
    if (s >= 0 && (int32_t)s < (int32_t)rd32(env->sgptDataTables, 0xC4) && iVar2 != 0) {
        void* piVar9 = p2;
        iVar2 = env->ordinal10786(p2, rd32(p1, 0x60), (uint32_t)(uintptr_t)summoner, (uint32_t)skillId);
        uint32_t uVar7 = iVar2 + (uint32_t)(uintptr_t)rec;
        if (uVar7 <= (uint32_t)(uintptr_t)rec + 1) uVar7 = (uint32_t)(uintptr_t)rec + 1;

        void* piVar6 = owner;
        iVar2 = env->ordinal10871(owner, (uint32_t)(int32_t)s);
        if (iVar2 == 0) {
            void* created = env->ordinal11013(rd32(summoner, 0x1C), 0x802, uVar7, rd32(p2, 0), rd32(p2, 0x0C));
            if (created == nullptr) return 0;
            env->ordinal10785(created, (uint32_t)(int32_t)(int16_t)rd16(piVar6, 0x82));
            env->ordinal10911(created, env->sub_98950);
            env->ordinal10807(piVar6, created, 1);
            env->ordinal10945(piVar6, (uint32_t)(int32_t)(int16_t)rd16(created, 0x82), 1);
            iVar2 = 1;
        }
        env->ordinal11043(piVar6, uVar7);
        env->sub_8EA80(piVar9, piVar6, 0xC, uVar7, 0, 0, 0);
        env->sub_9F730();
        env->sub_E0460();
        env->sub_EFE40(piVar9);
        env->ordinal11129(summoner, 0);
        void* item = env->ordinal10195(piVar6, 8);
        while (item != nullptr) {
            env->ordinal11111(piVar6, item);
            env->ordinal11108(item);
            item = env->ordinal10195(piVar6, 8);
        }
        const uint32_t a = env->ordinal10973(piVar6, 0xC, 0);
        const uint32_t b = env->ordinal10973(summoner, 0xC, 0);
        if (a != 0 && b < a && env->ordinal10871(piVar6, 0x6D) == 0) {
            void* created2 = env->ordinal11013(rd32(piVar9, 0x1C), 0, 0, rd32(piVar6, 0), rd32(piVar6, 0x0C));
            if (created2 != nullptr) {
                env->ordinal10785(created2, 0x6D);
                env->ordinal10807(piVar6, created2, 1);
                const uint32_t t4 = env->ordinal10973(piVar6, 6, 0);
                const uint32_t t5 = env->ordinal10913(piVar6);
                env->ordinal10188(created2, 0xB0, rd32(piVar9, 0x1C), 0);
                env->ordinal10188(created2, 0xB1, t5 >> 8, 0);
                int32_t v2 = (int32_t)env->sub_14D0((uint32_t)(uintptr_t)piVar6, t4 >> 8) * 0x100;
                int32_t v3 = (int32_t)env->sub_14D0((uint32_t)(uintptr_t)piVar6, t5 >> 8) * 0x100;
                if (v3 < 1) v3 = 1;
                if (v2 < 2) v2 = 1;
                if (v3 <= v2) v2 = v3;
                env->ordinal10887(piVar6, 0xC, 6, 0);
                env->ordinal10887(piVar6, 6, (uint32_t)v2, 0);
                env->ordinal10887(piVar6, 7, (uint32_t)v3, 0);
            }
        }
        (void)iVar2;
        return 1;
    }
    env->sub_DED30(game, p2, owner);
    return 1;
}

// 1.13c D2Game+0x9CDE0 -- MonsterSkillRollAndCast(param_1, param_2 stack); ESI = unit (unaff_ESI),
// EDI = game (unaff_EDI). Unit fields are dword indices in the decompile: [0] type, [4] id at +0x10, [8..9]
// the 64-bit LCG seed at +0x20. Gate: unit non-null and +0xC6 bit 0 clear; type 0 needs id != 0 and id == 0x11
// is rejected, type 1 needs id != 0 and id == 0xC is rejected; attacker (*param_2) +8 bit 0x20 required.
// The seed advances as seed = low32(seed) * 0x6AC690C5 + high32(seed) and rolls 40 % (rejects >= 0x28).
// The skill id from 0x18A0 is range-checked against the table count at +0xA80; 10 / 0x159 / 0x22D force
// uVar3 = 1. Then the 0x1180 range check, 10331/1130/1100/10224, the 0x47450 spawn, 0x1280 and 10057, the
// optional 10494 and 10973 / 10913 / 10551 finish, 0xF2380. Returns 1 on success, 0 otherwise.
// MISMATCH: 0x18A0 returns the skill in EAX; for an out-of-range id the decompile passes extraout_ECX (the
// register 0x18A0 leaves) to 0x15D0, which this body approximates with the table base. Ordinal 10331 returns a
// pointer here but a count in 0x97340 / 0x9FD00; the seam is typed both ways. The decompile calls 10494,
// 10973, 10913 and 10551 with no visible arguments; they are the no-argument seams.
uint32_t MonsterSkillRollAndCast_9CDE0(void* param1, void* param2, void* esiUnit, void* ediGame,
                                       const D2GameBatch97340Env* env)
{
    if (esiUnit == nullptr || (rd8(esiUnit, 0xC6) & 1) != 0) return 0;
    bool bVar8;
    if (rd32(esiUnit, 0) == 0) {
        if (rd32(esiUnit, 0x10) == 0) return 0;
        bVar8 = rd32(esiUnit, 0x10) == 0x11;
    } else {
        if (rd32(esiUnit, 0) != 1) return 0;
        if (rd32(esiUnit, 0x10) == 0) return 0;
        bVar8 = rd32(esiUnit, 0x10) == 0xC;
    }
    void* attacker = rdptr(param2, 0);
    if (bVar8 || attacker == nullptr || (rd8(attacker, 8) & 0x20) == 0) return 0;

    const uint64_t seed = (uint64_t)rd32(esiUnit, 0x20) * 0x6AC690C5ULL + (uint64_t)rd32(esiUnit, 0x24);
    std::memcpy((uint8_t*)esiUnit + 0x20, &seed, 8);
    if ((uint32_t)(seed & 0xFFFFFFFFu) % 100u > 0x27u) return 0;

    int32_t skill = env->sub_18A0();
    void* regBase = env->sgptDataTables;   // iVar4 for a valid id; the MISMATCH note covers the other path
    if (skill < 0 || skill >= (int32_t)rd32(env->sgptDataTables, 0xA80)) skill = -1;
    uint32_t uVar3 = env->sub_15D0((uint32_t)(uintptr_t)regBase, 1);
    if (skill == 10 || skill == 0x159 || skill == 0x22D) uVar3 = 1;

    const int32_t iVar4 = env->ordinal11027();
    if (iVar4 < 0x1e || (uVar3 == 0 && rd32(param1, 0x14) < 10)) {
        uVar3 = env->sub_1180((uint64_t*)((uint8_t*)esiUnit + 0x20));
        if (uVar3 < 0xF) {
            void* pvVar5 = env->ordinal10331Ptr(esiUnit);
            const uint32_t uVar6 = env->sub_1130(esiUnit);
            const uint32_t uVar7 = env->sub_1100(esiUnit);
            int32_t r = env->ordinal10224(pvVar5, uVar7, uVar6);
            int32_t l1 = 0, l2 = 0;
            r = env->sub_47450(ediGame, pvVar5, r, skill, &l1, &l2, 0);
            if (r != 0) {
                void* p = env->sub_1280();
                if (p != nullptr && env->ordinal10057(p) == 0) {
                    if ((int32_t)(uintptr_t)esiUnit < 0x1e &&
                        (int32_t)env->sub_1180((uint64_t*)((uint8_t*)esiUnit + 0x20)) < 0x19 &&
                        env->ordinal10494v() == 0) {
                        env->ordinal10973v();
                        env->ordinal10973v();
                        env->ordinal10913v();
                        env->ordinal10551v();
                    }
                    env->sub_F2380(esiUnit);
                    return 1;
                }
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0x9F600 -- MonstatMaxSkillReach. ECX = the unit (this), stack param_1, EAX = the monstat index.
// Gate: unit non-null, type 0; index in range; record's +0xBE byte positive and below the pet-type count.
// Loop over the skill count (+0xBC of the table); each skill resolves through 0x1420 and 10630; the largest
// value from 10306 / 10786 (at least 1 when 10786 gives less) is passed to 0x96EA0 (the caller of 0x9F600).
// MISMATCH: 0x1B60 and 0x1420 take an argument the decompile does not show (the loop index); the seam takes
// it explicitly. The 0x96EA0 target is a retail seam with the first argument from the stack.
void MonstatMaxSkillReach_9F600(void* thisUnit, uint32_t param1, int32_t monstatIndex,
                                const D2GameBatch97340Env* env)
{
    if (thisUnit == nullptr || rd32(thisUnit, 0) != 0) return;
    uint8_t* rec = skillRecord(env, monstatIndex);
    if (rec == nullptr) return;
    const int8_t cVar1 = (int8_t)rd8(rec, 0xBE);
    if (cVar1 <= 0 || (int32_t)cVar1 >= (int32_t)rd32(env->sgptDataTables, 0xBF0)) return;
    void* table = env->sub_1B60();
    if (table == nullptr) return;

    uint32_t localMax = 0;
    uint32_t localIndex = 0;
    if ((int32_t)rd32(table, 0xBC) > 0) {
        const uint8_t* psVar7 = (const uint8_t*)table + 0xC0;
        do {
            const int16_t sVar2 = (int16_t)rd16(psVar7, 0);
            void* rec2 = env->sub_1420((uint32_t)(int32_t)sVar2);
            if (rec2 != nullptr) {
                const uint32_t id = env->ordinal10630();
                if (id != 0) {
                    int32_t iVar5 = env->ordinal10306(thisUnit, id, 1);
                    int32_t iVar6 = 0;
                    if (iVar5 > 0) {
                        iVar6 = env->ordinal10786(thisUnit, rd32(rec2, 0xC0), (uint32_t)(int32_t)sVar2, (uint32_t)iVar5);
                        if (iVar6 < 1) iVar6 = 1;
                    }
                    if ((int32_t)localMax < iVar6) localMax = (uint32_t)iVar6;
                }
            }
            ++localIndex;
            psVar7 += 2;
        } while ((int32_t)localIndex < (int32_t)rd32(table, 0xBC));
    }
    env->sub_96EA0(param1, localMax);
}

// 1.13c D2Game+0x9FD00 -- SkillUseDispatchByMonstat(param_1 game, param_2 unit, param_3 flag; EAX = skill id;
// EBP and the summoner pointer are the registers Ghidra shows as unaff_EBP / unaff_retaddr). Stdcall ret 0xC.
// Gate: game and unit non-null and 10331(unit) nonzero; skill id in range; record present; param_3 == 0 requires
// 0x9EC80. Mask gate: gdwBitMasks[0] vs record +5, then 10331/10057 and the 11103 / 0x78550(mode 5) exit.
// Record +0x18F selects 10479 (4, 0x1C09, 0x180, 0x804, 0x805) after 0x9FA30. Then the handler table at +0x2C
// (DAT_6fd27338, host-supplied). A missing handler counts as 1; a handler returning 0 is a failure.
// Bit tests on the record: +4 (gdwBitMasks[0x18]) runs the 0x8E6F0 pass over the unit's list (code 8).
// MISMATCH: EBP and the summoner pointer are explicit parameters; the decompile does not tie either to a
// stack slot, so the caller must supply them in retail order. The handler table is data, not transcribed.
int32_t SkillUseDispatchByMonstat_9FD00(int32_t param1, void* param2, int32_t param3,
                                        int32_t skillEax, uint32_t ebp, void* summoner,
                                        const D2GameBatch97340Env* env)
{
    if (param1 == 0 || param2 == nullptr || env->ordinal10331(param2) == 0) return 0;
    uint8_t* rec = skillRecord(env, skillEax);
    if (rec == nullptr) return 0;
    if (param3 == 0 && !env->sub_9EC80()) return 0;

    if ((rd8(env->gdwBitMasks, 0) & rd8(rec, 5)) == 0) {
        if (env->ordinal10331(param2) == 0 || env->ordinal10057(param2) != 0) {
            env->ordinal11103(param2, 0);
            if (rd32(param2, 0) != 0) return 0;
            env->sub_98550(ebp, param2, 0, 5, 0, 0, 1);
            return 0;
        }
    }
    if (rd8(rec, 0x18F) != 0) {
        int32_t local = param1;
        if (env->sub_9FA30(summoner, &local) != 0) {
            uint32_t uVar7;
            switch (rd8(rec, 0x18F)) {
                case 1: uVar7 = 4;      break;
                case 2: uVar7 = 0x1C09; break;
                case 3: uVar7 = 0x180;  break;
                case 4: uVar7 = 0x804;  break;
                case 5: uVar7 = 0x805;  break;
                default: return 0;
            }
            if (env->ordinal10479(ebp, local, param2, uVar7) == 0) return 0;
        }
    }
    const int16_t sVar2 = (int16_t)rd16(rec, 0x2C);
    if (sVar2 < 0) return 0;
    if (sVar2 >= 0x5B) return 0;

    int32_t iVar6 = 1;
    if (env->hasSkillHandler(sVar2)) {
        iVar6 = env->callSkillHandler(sVar2);
        if (iVar6 == 0) return 0;
        if ((rd8(env->gdwBitMasks, 0x14) & rd8(rec, 8)) == 0 && param3 == 0) env->sub_9F850(param2, summoner);
    }
    if ((rd8(env->gdwBitMasks, 0x18) & rd8(rec, 4)) != 0) {
        uint8_t* pcVar3 = (uint8_t*)rdptr(param2, 0xDC);
        while (pcVar3 != nullptr) {
            const uint8_t cVar1 = pcVar3[0];
            pcVar3 = (uint8_t*)rdptr(pcVar3, 0x24);
            if (cVar1 == 8) env->sub_8E6F0();
        }
    }
    return iVar6;
}
