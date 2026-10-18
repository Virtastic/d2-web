// src/core/d2game/d2game_batch_9d6f0.cpp -- two 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the act loader and the player-pet
// refresh that follows an act change. Stage A batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector.
//
// Host model: retail records are opaque host objects addressed by their retail byte offsets
// (rd8/rd32/rdptr, wr32/wrptr). Every callee src/ does not own is a seam on D2GameBatchBEnv in retail
// argument order; register-only arguments (Ghidra unaff_ESI/unaff_EBX/unaff_EDI) are explicit
// parameters below. The fatal path Fog #10265(id) -> #10024(text) -> _exit(-1) is modelled as
// d2game_fatal(id): only the exit is observable. A probe passes the env pointer in r[5], the retail
// arguments in r[0..1], and reads the result in r[4] (the retail capture tool/fn/README.md).
//
// Prior art (1.10f): D2MOO src/D2Game/GAME/Level.cpp LEVEL_LoadAct (0x6FC3C510) for 0x9D6F0, including
// ARENA_IsInArenaMode/ARENA_GetAlternateStartTown (Arena.cpp) and DUNGEON_AllocAct; D2MOO
// src/D2Game/PLAYER/PlayerPets.cpp PLAYERPETS_GetPetListFromPetType (0x6FC7E2E0) and sub_6FC7E310
// (0x6FC7E310) for 0xBFFA0. The 1.13c ordinal numbering differs from 1.10f; the names below are the
// 1.13c imports reached through the thunks (src/core/fn/import_thunk.cpp):
//   0xAADE #10585 DUNGEON_GetTownLevelIdFromActNo   0xAEF2 #10951 DUNGEON_AllocAct
//   0xA4C0 #10920 UNITS_GetPlayerData               0xA3B8 #10750 unit Y
//   0xA454 #10867 unit X                            0xA2DA/#10024 + 0xA2C2/#10265 fatal path
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees src/ does not own.
struct D2GameBatchBEnv {
    // D2Common #10585 DUNGEON_GetTownLevelIdFromActNo(nAct): the town level id of an act.
    uint32_t (*getTownLevelIdFromActNo)(uint32_t nAct);
    // D2Common #10951 DUNGEON_AllocAct(nAct, dwInitSeed, 0, game, nDifficulty, pMemoryPool,
    // nTownLevelId, 0, 0): the game/pool/difficulty part of the argument list is folded into the seam.
    void*    (*allocAct)(void* pGame, uint32_t nAct, uint32_t nTownLevelId);
    // 0x9D6F0's tail call 0xC2150 ACT2Q3_BrightenEnvironment(game, nAct): no-op unless nAct == 1 (Act II).
    void     (*brightenAct2Quest3)(void* pGame, uint32_t nAct);

    // D2Common #10920 UNITS_GetPlayerData(pUnit).
    void*    (*getPlayerData)(void* pUnit);
    // sgptDataTables->nPetTypeTxtRecordCount (record offset 0xBF0).
    uint32_t (*petTypeTxtRecordCount)();
    // D2Common #10750 / #10867: the unit's Y / X from its dynamic-path block (u16 reads).
    uint32_t (*getUnitYFromPath)(void* pPath);
    uint32_t (*getUnitXFromPath)(void* pPath);
    // 0x97340 sub_6FC7E310(game, player, x, y): re-evaluate the player's pets against its position.
    void     (*updatePlayerPets)(void* pGame, void* pPlayer, uint32_t x, uint32_t y);
};

static const D2GameBatchBEnv kNoEnv{};
static const D2GameBatchBEnv& batchEnv(const uint32_t* r) {
    return r[5] ? *(const D2GameBatchBEnv*)(uintptr_t)r[5] : kNoEnv;
}

// 0x9D6F0 = LEVEL_LoadAct(game, nAct): game in ESI, the act index in EBX (low byte used).
// The game must have an arena control block (D2_ASSERT, fatal 0x27). Not in arena mode: the town level
// id comes from DUNGEON_GetTownLevelIdFromActNo(nAct); in arena mode ARENA_GetAlternateStartTown
// (its own D2_ASSERT, fatal 0x63) returns the arena's alternate start town (+0x00). Allocate the act
// and store it in game->pAct[nAct] (+0xBC + 4*nAct); 1.13c moved the "already loaded" early-out to the
// callers (0x2BF90 tests the slot first). Then run the Act II quest-3 environment brighten (0xC2150).
void LEVEL_LoadAct_113c(void* pGame, uint8_t nAct, const D2GameBatchBEnv* env)
{
    void* pArena = rdptr(pGame, 0x1D28);                       // pGame->pArenaCtrl
    if (pArena == nullptr) d2game_fatal(0x27);                 // ARENA_IsInArenaMode's D2_ASSERT

    uint32_t nTownLevelId;
    if ((rd8(pArena, 0x08) & 0x02) == 0) {                     // !(fFlags & GAMEFLAG_ARENA_MODE)
        nTownLevelId = env->getTownLevelIdFromActNo(nAct);     // D2Common #10585
    } else {
        if (pArena == nullptr) d2game_fatal(0x63);             // ARENA_GetAlternateStartTown's D2_ASSERT
        nTownLevelId = rd32(pArena, 0x00);                     // nAlternateStartTown
    }

    void* pAct = env->allocAct(pGame, nAct, nTownLevelId);     // D2Common #10951
    wrptr(pGame, 0xBC + (uint32_t)nAct * 4, pAct);             // pGame->pAct[nAct]
    env->brightenAct2Quest3(pGame, nAct);                      // 0xC2150
}

// Unit coordinate as the 1.13c inlined accessors 0x1100/0x1130 read it (sub_1100.md/sub_1130.md):
// object/missile/item-ish types (2, 4, 5) keep a dword in the unit's data block at +0xC/+0x10; any
// other type goes through D2Common #10867/#10750 when the block exists, else 0.
static uint32_t unitCoordX(void* pUnit, const D2GameBatchBEnv* env)
{
    const uint32_t nType = rd32(pUnit, 0x00);
    void* pData = rdptr(pUnit, 0x2C);
    if (nType == 2 || (nType >= 4 && nType <= 5)) return rd32(pData, 0x0C);
    return pData ? env->getUnitXFromPath(pData) : 0;           // D2Common #10867
}

static uint32_t unitCoordY(void* pUnit, const D2GameBatchBEnv* env)
{
    const uint32_t nType = rd32(pUnit, 0x00);
    void* pData = rdptr(pUnit, 0x2C);
    if (nType == 2 || (nType >= 4 && nType <= 5)) return rd32(pData, 0x10);
    return pData ? env->getUnitYFromPath(pData) : 0;           // D2Common #10750
}

// 0xBFFA0: player-pet refresh after an act change (game in EBX, the player unit in EDI, both explicit).
// Requires pet type 7 to exist (nPetTypeTxtRecordCount > 7, sgptDataTables+0xBF0) and the player to
// have a pet container (+0x44). The 1.13c container's first dword is the pet-list array base; list 7
// (7 * sizeof(D2PetListStrc) = 0x54) holds the D2PetData chain (nFlags +0x00, nUnitGUID +0x04,
// pNext +0x14). Skip pets whose nFlags bit 0 is set; the first unflagged one must still exist as a
// monster unit in the game (pUnitList entry 1 = game+0x1320, 128 buckets by GUID & 0x7F, node id +0xC,
// chain +0xE4). A null game is fatal (0x2D3). When the pet exists, the player unit is handed to
// 0x97340 sub_6FC7E310(game, player, x, y) with the player's own coordinates (y evaluated first, the
// retail argument push order); the unit's type/data offsets used for those coordinates are the same as
// the accessors above.
void PLAYERPETS_UpdatePetsIfPresent_113c(void* pGame, void* pUnit, const D2GameBatchBEnv* env)
{
    void* pPlayerData = env->getPlayerData(pUnit);             // D2Common #10920
    if (pPlayerData == nullptr) return;
    if (env->petTypeTxtRecordCount() <= 7) return;             // sgptDataTables->nPetTypeTxtRecordCount

    void* pPets = rdptr(pPlayerData, 0x44);                    // pPlayerData->pPlayerPets
    if (pPets == nullptr) return;
    void* pPetList = rdptr(pPets, 0x00);                       // 1.13c pet-list array base
    void* pPet = pPetList ? rdptr(pPetList, 0x54) : nullptr;   // list 7's first D2PetData
    if (pPet == nullptr) return;

    while ((rd8(pPet, 0x00) & 1) != 0) {                       // skip flagged (dead/removed) pets
        pPet = rdptr(pPet, 0x14);                              // D2PetDataStrc->pNext
        if (pPet == nullptr) return;
    }

    if (pGame == nullptr) d2game_fatal(0x2D3);

    const uint32_t nPetGUID = rd32(pPet, 0x04);                // D2PetDataStrc->nUnitGUID
    void* pUnitNode = rdptr(pGame, 0x1320 + (nPetGUID & 0x7F) * 4);  // pUnitList[1] (monsters)
    if (pUnitNode == nullptr) return;
    while (rd32(pUnitNode, 0x0C) != nPetGUID) {
        pUnitNode = rdptr(pUnitNode, 0xE4);
        if (pUnitNode == nullptr) return;
    }

    const uint32_t y = unitCoordY(pUnit, env);                 // pushed first (stack arg order)
    const uint32_t x = unitCoordX(pUnit, env);
    env->updatePlayerPets(pGame, pUnit, x, y);                 // 0x97340
}

// 1.13c D2Game+0x9D6F0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9d6f0(uint32_t* r)
{
    LEVEL_LoadAct_113c((void*)(uintptr_t)r[0], (uint8_t)r[1], &batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0xBFFA0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_bffa0(uint32_t* r)
{
    PLAYERPETS_UpdatePetsIfPresent_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], &batchEnv(r));
    r[4] = 0;
}
