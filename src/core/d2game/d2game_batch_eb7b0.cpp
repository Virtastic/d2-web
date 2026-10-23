// src/core/d2game/d2game_batch_eb7b0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the six section writers the
// 1.13c PLRSAVE2_CreateSaveFile (0xEC7F0) factored out of the 1.10f inline body (quest, skills, items,
// pet, iron golem, corpses), the inactive-unit chain free, the CRT __aulldiv helper, and four
// SUnitInactive/AiGeneral chain helpers.
//
// Retail records are opaque host objects read at their retail byte offsets. Every callee src/ does not
// own is a seam on D2GameBatchEEnv, in retail argument order; the 1.10f crosswalk is D2MOO
// PLAYER/PlrSave2.cpp / PlrSaveItems.cpp, which carries the same section writers (prior art):
//   * 0xEB7B0 == the quest block inlined in PLRSAVE2_CreateSaveFile ('!ooW', 6, 298, 3x96 records)
//   * 0xEBC40 == the 'fi' skill block inlined there (one level byte per class skill)
//   * 0xEC0E0/0xEC1E0 == sub_6FC8A0F0 applied to the player/pet inventory ('JM' item sections)
//   * 0xEC020 == PLRSAVE2_WriteIronGolemSection ('fk'), 0xEC260 == PLRSAVE2_WriteCorpsesSection ('MJ')
// The register-only arguments (Ghidra in_EAX/unaff_EBX/unaff_EDI) are explicit parameters below.
// The probes are the FN-path exports; the alpha lane ships no vector, so their default env is inert.
#include "d2game_scalar_helpers.h"
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees src/ does not own. Ordinal notes are the 1.13c imports; the names are the
// 1.10f D2MOO crosswalk (the two ordinal tables are not the same numbering).
struct D2GameBatchEEnv {
    // D2Common #10920 UNITS_GetPlayerData
    void*    (*getPlayerData)(void* pUnit);
    // D2Common #10579 QUESTRECORD_CopyRecordToBuffer
    void     (*copyQuestRecord)(void* pRecord, void* pBuffer, uint32_t nSize, int32_t b);
    // sgptDataTables->nHighestClassSkillCount (record offset 0xBA8)
    uint32_t (*highestClassSkillCount)();
    // D2Common #11120 SKILLS_GetPlayerSkillCount / #10044 SKILLS_GetClassSkillId
    uint32_t (*getPlayerSkillCount)(uint32_t nClassId);
    int32_t  (*getClassSkillId)(uint32_t nClassId, uint32_t i);
    // D2Common #10708 SKILLS_GetSkillById / #10306 SKILLS_GetSkillLevel
    void*    (*getSkillById)(void* pPlayer, int32_t nSkillId, int32_t nSkillLevel);
    int32_t  (*getSkillLevel)(void* pPlayer, void* pSkill, int32_t b);
    // 0xB6A40 sub_6FC7E8B0(pGame, pPlayer, nPetType, 1)
    void*    (*getPet)(void* pGame, void* pPlayer, int32_t nPetType, int32_t a4);
    // D2Common #10460 INVENTORY_GetFirstItem / #10464 GetNextItem / #11147 UnitIsItem
    void*    (*inventoryFirstItem)(void* pInventory);
    void*    (*inventoryNextItem)(void* pItem);
    void*    (*unitIsItem)(void* pItem);
    // 0x53470 D2GAME_SAVE_SerializeItem wrapper(item) writing at pOut
    int32_t  (*serializeItem)(void* pItem, void* pOut, uint32_t nSize, int32_t a4, int32_t a5);
    // 0x53B40 PLRSAVE_WriteItems (unit arrives in EBX in retail)
    int32_t  (*writeItems)(void* pGame, void* pUnit, uint8_t* pOut, uint32_t nSize, int32_t a5);
    // 0x537D0 PLRSAVE_WriteItems' item section (dest, inventory, game, unit, size)
    int32_t  (*writeItemsEx)(uint8_t* pOut, void* pInventory, void* pGame, void* pUnit, uint32_t nSize);
    // D2Common #10284 GetFirstCorpse / #10348 GetNextCorpse / #10155 GetUnitGUIDFromCorpse
    void*    (*firstCorpse)(void* pInventory);
    void*    (*nextCorpse)(void* pCorpse);
    uint32_t (*corpseGuid)(void* pCorpse);
    // D2Common #10508 INVENTORY_GetItemCount / #10744 ITEMS_CheckItemFlag(item, flag)
    int32_t  (*itemCount)(void* pInventory);
    int32_t  (*checkItemFlag)(void* pItem, uint32_t dwFlag);
    // D2Common #10107 quest-record write: (0, item, 3, pQuestRecord, 0x201, 0)
    void     (*questSet)(int32_t a, void* pItem, int32_t b, void* pQuestRecord, uint32_t c, int32_t d);
    // Fog #10046 FOG_FreePool(pool, p, file, line, 0) / #10045 FOG_AllocPool(pool, size, file, line, 0)
    void     (*freePool)(void* pPool, void* p, const char* szFile, int32_t nLine);
    void*    (*allocPool)(void* pPool, uint32_t nSize, const char* szFile, int32_t nLine);
    // D2Common #10850 free of the block at chain+0x24 (argument arrives in a register)
    void     (*freeAux)(void* p);
    // 0x8E6F0 timer free (timer node in a register)
    void     (*freeTimer)(void* pTimer);
    // 0x1F9F0 (ctx, pool, node)
    void     (*freeNodeCallback)(void* pCtx, void* pPool, void* pNode);
    // 0x1E640 / 0x1E3D0, the two branches at the tail of 0x1E900
    void     (*branchList)(void* p1, void* ppThing);
    void     (*branchSingle)(void* p1);
};

// The default env is inert (no FN vector in this lane): every seam is null and the probes exist so the
// functions have their 1.13c exports, not so they can run unwired.
static const D2GameBatchEEnv g_batchEEnv{};

static const char kAiGeneral[]     = "..\\Source\\D2Game\\Ai\\AiGeneral.cpp";
static const char kSUnitInactive[] = "..\\Source\\D2Game\\UNIT\\SUnitInactive.cpp";

// ---- 0xEB7B0: quest-record section ('!ooW', version 6, 298 bytes) -------------------------------
static int32_t QuestSection(void* pPlayer, uint8_t** ppSection, const uint8_t* pEnd, const D2GameBatchEEnv* env)
{
    uint8_t* cur = *ppSection;
    if (pEnd < cur + 0x12A) return 1;

    void* playerData = env->getPlayerData(pPlayer);            // D2Common #10920
    if (playerData == nullptr) return 2;

    uint8_t buffer[0x12A] = {};
    wr32(buffer, 0x00, 0x216F6F57);                            // '!ooW'
    wr32(buffer, 0x04, 6);
    wr16(buffer, 0x08, 0x12A);
    for (int32_t i = 0; i < 3; ++i) {
        env->copyQuestRecord(rdptr(playerData, 0x10 + 4 * i),  // pQuestData[3] at +0x10
                             buffer + 0x0A + 96 * i, 96, 1);
    }
    std::memcpy(cur, buffer, sizeof buffer);
    *ppSection = cur + sizeof buffer;
    return 0;
}

// ---- 0xEBC40: 'fi' skill section ---------------------------------------------------------------
// 1.13c D2Game+0xEBC40
// 1.13c writes the marker before resolving the class id (1.10f resolved it first) and leaves the
// section cursor unadvanced on the BAD_CLASS return.
static int32_t SkillSection(void* pPlayer, uint8_t** ppSection, const uint8_t* pEnd, const D2GameBatchEEnv* env)
{
    uint8_t* cur = *ppSection;
    if (pEnd < cur + env->highestClassSkillCount() + 2) return 1;

    wr16(cur, 0x00, 0x6669);                                   // 'fi'
    if (pPlayer != nullptr && rd32(pPlayer, 0x00) == 0) {      // UNIT_PLAYER
        void* playerData = env->getPlayerData(pPlayer);        // D2Common #10920
        void* client = rdptr(playerData, 0x9C);                // pPlayerData->pClient
        if (client != nullptr) {
            const uint8_t classId = rd8(client, 0x08);         // D2ClientStrc.nClassId
            if (classId < 7) {
                const uint32_t count = env->getPlayerSkillCount(classId);
                uint8_t* out = cur + 2;
                for (uint32_t i = 0; i < count; ++i) {
                    const int32_t skillId = env->getClassSkillId(classId, i);
                    void* skill = env->getSkillById(pPlayer, skillId, -1);
                    *out++ = (uint8_t)env->getSkillLevel(pPlayer, skill, 0);
                }
                *ppSection = out;
                return 0;
            }
            return 2;
        }
    }
    d2game_fatal(0x8A4);
}

// 1.13c D2Game+0xEC020
// ---- 0xEC020: PLRSAVE2_WriteIronGolemSection ('fk') --------------------------------------------
// Retail advances by the serializer's length even when it reported failure; only the present flag is
// cleared (same as the 1.10f body).
static int32_t IronGolemSection(void* pGame, void* pPlayer, uint8_t** ppSection,
                                const uint8_t* pEnd, int32_t a5, const D2GameBatchEEnv* env)
{
    if (rd32(pGame, 0x70) == 0) return 0;                      // pGame->bExpansion

    uint8_t* cur = *ppSection;
    wr16(cur, 0x00, 0x666B);                                   // 'fk'

    void* golem = env->getPet(pGame, pPlayer, 3 /* PETTYPE_GOLEM */, 1);
    void* golemItem = nullptr;
    bool hasGolem = false;
    if (golem != nullptr && rd32(golem, 0x04) == 0x123 /* MONSTER_IRONGOLEM */ &&
        env->getSkillById(pPlayer, 90 /* SKILL_IRONGOLEM */, -1) != nullptr &&
        rdptr(golem, 0x60) != nullptr) {
        void* first = env->inventoryFirstItem(rdptr(golem, 0x60));
        if (first != nullptr) {
            golemItem = env->unitIsItem(first);                // INVENTORY_UnitIsItem
            hasGolem = golemItem != nullptr;
        }
    }

    wr8(cur, 0x02, hasGolem ? 1 : 0);
    uint8_t* out = cur + 3;
    if (hasGolem) {
        const int32_t n = env->serializeItem(golemItem, out, (uint32_t)(pEnd - out), 0, a5);
        if (n < 1) wr8(cur, 0x02, 0);
        out += n;
    }
    *ppSection = out;
    return 0;
}

// ---- 0xEC0E0: item section through 0x53B40 -----------------------------------------------------
// game/unit/a5 are the formal arguments; the write cursor arrived in EAX and its address in EDI.
static int32_t WriteItemsSection(void* pGame, void* pUnit, uint8_t** ppSection,
                                 const uint8_t* pEnd, int32_t a6, const D2GameBatchEEnv* env)
{
    uint8_t* cur = *ppSection;
    const int32_t n = env->writeItems(pGame, pUnit, cur, (uint32_t)(pEnd - cur), a6);
    if (n < 0) return 3;
    *ppSection = cur + n;
    return 0;
}

// ---- 0xEC1E0: pet item section ('fj') ----------------------------------------------------------
// pGame arrived in EAX, pPlayer in ECX; the marker is written before the pet lookup and there is no
// bounds check in front of it (the 1.10f caller had one).
static int32_t PetSection(void* pGame, void* pPlayer, uint8_t** ppSection,
                          const uint8_t* pEnd, const D2GameBatchEEnv* env)
{
    uint8_t* cur = *ppSection;
    if (rd32(pGame, 0x70) != 0) {
        wr16(cur, 0x00, 0x666A);                               // 'fj'
        cur += 2;
        void* pet = env->getPet(pGame, pPlayer, 7 /* PETTYPE_HIREABLE */, 1);
        if (pet != nullptr) {
            const int32_t n = env->writeItemsEx(cur, rdptr(pet, 0x60), pGame, pet,
                                                (uint32_t)(pEnd - cur));
            if (n < 0) return 5;
            cur += n;
        }
        *ppSection = cur;
    }
    return 0;
}

// ---- 0xEC260: PLRSAVE2_WriteCorpsesSection ('MJ') ----------------------------------------------
// The 1.13c decompile has six removed-unreachable-block warnings: the arms that write the marker and
// serialize the corpse's items were dropped. They are reconstructed from the 1.10f prior art; the
// per-item quest write loop is the visible 1.13c delta and sits where the decompile shows it.
static int32_t CorpsesSection(void* pGame, void* pPlayer, uint8_t** ppSection,
                              const uint8_t* pEnd, const D2GameBatchEEnv* env)
{
    void* inventory = rdptr(pPlayer, 0x60);
    if (inventory == nullptr) return 2;

    void* corpseUnit = nullptr;
    for (void* corpse = env->firstCorpse(inventory); corpse != nullptr;
         corpse = env->nextCorpse(corpse)) {
        const uint32_t guid = env->corpseGuid(corpse);
        if (pGame == nullptr) d2game_fatal(0x2D3);

        void* unit = nullptr;
        for (void* u = rdptr(pGame, 0x1120 + (guid & 0x7F) * 4); u != nullptr; u = rdptr(u, 0xE4)) {
            if (rd32(u, 0x0C) == guid) { unit = u; break; }
        }
        if (unit != nullptr) {
            void* unitInventory = rdptr(unit, 0x60);
            if (unitInventory != nullptr && env->itemCount(unitInventory) != 0) {
                // 1.13c delta: walk the corpse's items; an item without flag 4 gets a quest write.
                for (void* item = env->inventoryFirstItem(unitInventory); item != nullptr;
                     item = env->inventoryNextItem(item)) {
                    void* checked = env->unitIsItem(item);
                    if (checked != nullptr && env->checkItemFlag(checked, 4) == 0) {
                        void* playerData = env->getPlayerData(pPlayer);
                        const uint32_t difficulty = rd8(pGame, 0x6D);   // pGame->nDifficulty
                        env->questSet(0, checked, 3,
                                      rdptr(playerData, 0x10 + difficulty * 4), 0x201, 0);
                    }
                }
                corpseUnit = unit;
                break;
            }
        }
    }

    uint8_t* cur = *ppSection;
    if (cur + 4 > pEnd) return 1;
    wr16(cur, 0x00, 0x4D4A);                                   // 'MJ'
    wr16(cur, 0x02, corpseUnit != nullptr ? 1 : 0);
    cur += 4;

    if (corpseUnit == nullptr) {
        *ppSection = cur;
        return 0;
    }

    if (cur + 12 > pEnd) return 1;
    wr32(cur, 0x00, 0);                                        // retail writes an unknown dword here
    void* path = rdptr(corpseUnit, 0x2C);                      // pDynamicPath
    wr32(cur, 0x04, path != nullptr ? rd32(path, 0x08) : 0);   // CLIENTS_GetUnitX
    wr32(cur, 0x08, path != nullptr ? rd32(path, 0x0C) : 0);   // CLIENTS_GetUnitY
    cur += 12;

    const int32_t n = env->writeItemsEx(cur, rdptr(corpseUnit, 0x60), pGame, corpseUnit,
                                        (uint32_t)(pEnd - cur));
    if (n < 0) return 4;
    *ppSection = cur + n;
    return 0;
}

// ---- 0x1EBD0: free an inactive-unit list's monster chain ---------------------------------------
// The list node arrived in EAX; +0x0C is D2InactiveUnitListStrc.pInactiveMonsters (D2UnitStrc's
// pListNext at +0xE4 and pool at +0x08 belong to the other uses of these helpers).
static void FreeInactiveChains(void* unit, const D2GameBatchEEnv* env)
{
    void* pool = rdptr(unit, 0x08);
    void* node = rdptr(unit, 0x0C);
    while (node != nullptr) {
        void* next = rdptr(node, 0x58);
        for (void* inner = rdptr(node, 0x20); inner != nullptr; ) {
            void* innerNext = rdptr(inner, 0x04);
            env->freePool(pool, inner, kAiGeneral, 0x246);
            inner = innerNext;
        }
        void* aux = rdptr(node, 0x24);
        if (aux != nullptr) env->freeAux(aux);                  // D2Common #10850
        env->freePool(pool, node, kSUnitInactive, 0x8D);
        node = next;
    }
}

// ---- 0x20B30: free an AiGeneral chain (node unlink pointer at +0x04) ---------------------------
static void FreeAiChain(void* pool, void* node, const D2GameBatchEEnv* env)
{
    while (node != nullptr) {
        void* next = rdptr(node, 0x04);
        env->freePool(pool, node, kAiGeneral, 0x246);
        node = next;
    }
}

// ---- 0x1FB40: free an inactive-unit list, running the 0x1F9F0 callback first when eligible ------
// pool is the formal argument; ctx (EBX) and the list head (EAX) are register arguments.
static void FreeInactiveList(void* pool, void* ctx, void* node, const D2GameBatchEEnv* env)
{
    while (node != nullptr) {
        void* next = rdptr(node, 0x00);
        const int32_t threshold = rd32(node, 0x04);
        if (threshold == 0 || (int32_t)rd32(ctx, 0xA8) <= threshold) {
            env->freeNodeCallback(ctx, pool, node);
        }
        env->freePool(pool, node, kSUnitInactive, 0x127);
        node = next;
    }
}

// ---- 0x1E900: walk a unit's timer chain, then branch on *(*ppThing+0x14) -----------------------
// unit arrived in EDX, ppThing on the stack; the chain node's kind byte is at +0x00, next at +0x24.
static void TimerWalkAndBranch(void* p1, void* unit, void* ppThing, const D2GameBatchEEnv* env)
{
    if (unit != nullptr) {
        for (void* timer = rdptr(unit, 0xDC); timer != nullptr; timer = rdptr(timer, 0x24)) {
            if (rd8(timer, 0x00) == 2) env->freeTimer(timer);   // 0x8E6F0
        }
    }
    if (rd32(rdptr(ppThing, 0x00), 0x14) == 0) env->branchList(p1, ppThing); // 0x1E640
    else env->branchSingle(p1);                                             // 0x1E3D0
}

// ---- 0x20CF0: push a D2MinionListStrc node onto pAiControl->pMinionList (+0x34) ---------------
// unit arrived in EAX, owner unit in EDI (GUID at +0x0C). The AI control is unit+0x14 (pMonsterData)
// -> +0x28 (pAiControl) and retail dereferences it unconditionally; the 8-byte node holds
// {dwMinionGUID, pNext} and is allocated from the unit's pool.
static void AddTargetNode(void* unit, void* ctx, const D2GameBatchEEnv* env)
{
    if (unit == nullptr || rd32(unit, 0x00) != 1 /* UNIT_MONSTER */) return;

    void* monsterData = rdptr(unit, 0x14);
    void* aiControl = monsterData != nullptr ? rdptr(monsterData, 0x28) : nullptr;
    uint32_t* node = (uint32_t*)env->allocPool(rdptr(unit, 0x08), 8, kAiGeneral, 499);
    node[0] = ctx != nullptr ? rd32(ctx, 0x0C) : 0xFFFFFFFFu;  // owner GUID, or -1
    node[1] = 0;
    if (rd32(aiControl, 0x34) != 0) node[1] = rd32(aiControl, 0x34);
    wr32(aiControl, 0x34, (uint32_t)(uintptr_t)node);
}

// ---- FN-path exports (r[0..3] args, r[4] result; argument 5 rides in r[5]) ----------------------

// 1.13c D2Game+0xEB7B0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_eb7b0(uint32_t* r)
{
    r[4] = (uint32_t)QuestSection((void*)(uintptr_t)r[0], (uint8_t**)(uintptr_t)r[1],
                                  (const uint8_t*)(uintptr_t)r[2], &g_batchEEnv);
}

// 1.13c D2Game+0xEBC40
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ebc40(uint32_t* r)
{
    r[4] = (uint32_t)SkillSection((void*)(uintptr_t)r[0], (uint8_t**)(uintptr_t)r[1],
                                  (const uint8_t*)(uintptr_t)r[2], &g_batchEEnv);
}

// 1.13c D2Game+0xEC020
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ec020(uint32_t* r)
{
    r[4] = (uint32_t)IronGolemSection((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1],
                                      (uint8_t**)(uintptr_t)r[2], (const uint8_t*)(uintptr_t)r[3],
                                      (int32_t)r[5], &g_batchEEnv);
}

// 1.13c D2Game+0xEC0E0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ec0e0(uint32_t* r)
{
    r[4] = (uint32_t)WriteItemsSection((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1],
                                       (uint8_t**)(uintptr_t)r[2], (const uint8_t*)(uintptr_t)r[3],
                                       (int32_t)r[5], &g_batchEEnv);
}

// 1.13c D2Game+0xEC1E0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ec1e0(uint32_t* r)
{
    r[4] = (uint32_t)PetSection((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1],
                                (uint8_t**)(uintptr_t)r[2], (const uint8_t*)(uintptr_t)r[3],
                                &g_batchEEnv);
}

// 1.13c D2Game+0xEC260
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ec260(uint32_t* r)
{
    r[4] = (uint32_t)CorpsesSection((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1],
                                    (uint8_t**)(uintptr_t)r[2], (const uint8_t*)(uintptr_t)r[3],
                                    &g_batchEEnv);
}

// 1.13c D2Game+0x1EBD0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1ebd0(uint32_t* r)
{
    FreeInactiveChains((void*)(uintptr_t)r[0], &g_batchEEnv);
    r[4] = 0;
}

// 1.13c D2Game+0x3E70
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3e70(uint32_t* r)
{
    const uint64_t q = d2game_aulldiv(r[0], r[1], r[2], r[3]);
    r[4] = (uint32_t)q;
    r[5] = (uint32_t)(q >> 32);
}

// 1.13c D2Game+0x20B30
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_20b30(uint32_t* r)
{
    FreeAiChain((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], &g_batchEEnv);
    r[4] = 0;
}

// 1.13c D2Game+0x1FB40
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1fb40(uint32_t* r)
{
    FreeInactiveList((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1],
                     (void*)(uintptr_t)r[2], &g_batchEEnv);
    r[4] = 0;
}

// 1.13c D2Game+0x1E900
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1e900(uint32_t* r)
{
    TimerWalkAndBranch((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1],
                       (void*)(uintptr_t)r[2], &g_batchEEnv);
    r[4] = 0;
}

// 1.13c D2Game+0x20cf0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_20cf0(uint32_t* r)
{
    AddTargetNode((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], &g_batchEEnv);
    r[4] = 0;
}
