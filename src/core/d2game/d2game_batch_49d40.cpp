// src/core/d2game/d2game_batch_49d40.cpp -- 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the GAME/Clients.cpp client-state
// setter, unlock-flag setter and CLIENTS_AddToGame. Stage A batch lane (OBJECTIVE v9): implementation +
// marker only, no spec, no vector.
//
// Records are opaque host objects addressed by retail byte offsets. Register-only arguments (Ghidra's
// in_EAX/unaff_*) are explicit parameters, named per function. Callees outside this batch are seam
// pointers on D2GameBatch12Env in retail argument order. The fatal path Fog #10265(id) -> #10024(text)
// -> _exit(-1) is modelled as d2game_fatal(id): only the exit is observable.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Game/src/GAME/Clients.cpp (sub_6FC33910,
// CLIENTS_AddToGame 0x6FC325E0).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION no-ops, deterministic GetTickCount

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees src/ does not implement, in retail argument order.
struct D2GameBatch12Env {
    void*    (*allocPool)(const char* file, uint32_t line, uint32_t zero); // 0xA2E6, Fog #10045
    uint32_t (*strCopy)(char* dst, const char* src, uint32_t max);         // 0xA280, Storm #501
    uint16_t (*crc16)(const char* s);                                      // 0xA334, Fog #10137
    int32_t  (*shouldTreatClassAsTemplateId)(void* game);                  // 0xB2620
    uint32_t (*getTemplateType)(void* game);                               // 0xB22D0
    uint8_t  (*getClassFromCharTemplate)(uint8_t charTemplate, uint32_t templateType); // Fog #10241
    void     (*levelLoadAct)(void* game, uint8_t act);                     // 0x9D6F0
    void     (*levelSyncDayNight)(void* game);                             // 0x9D610
    void     (*appendPacket)(void* client, const void* data, uint32_t len); // 0x8A3E0
    // MonStats record lookups (sgptDataTables+0xA78 array, stride 0x1A8, count at +0xA80):
    // nonzero when the record exists and dwMonStatsFlags (+0x0C) has the ranged flag bit
    // (byte +0x0F & (byte)gdwBitMasks[0x10]); likewise the spawn flag (byte +0x0C &
    // (byte)gdwBitMasks[0]); nRarity byte at +0x2E.
    int32_t  (*monStatsIsRanged)(int32_t monsterId);
    int32_t  (*monStatsIsSpawnable)(int32_t monsterId);
    uint8_t  (*monStatsRarity)(int32_t monsterId);
};

// ---- module state --------------------------------------------------------------------------------
static void* g_clientList[256];        // 0x6FD307B8
static void* g_clientListByName[256];  // 0x6FD303B8
static CRITICAL_SECTION g_clientListLock;      // 0x6FD30BB8
static CRITICAL_SECTION g_clientByNameLock;    // 0x6FD30BD0
static int32_t g_clientListInitialized;        // 0x6FD31C18

// D2ClientStrc offsets (1.13c): +0x00 id, +0x04 state, +0x08 class, +0x0C char template, +0x0D name[16],
// +0x1D account[16], +0x60 char-save token, +0x68 pClientInfo, +0x88 game->pClientList (in game),
// +0x8C game->nClients, +0x1A8 pGame, +0x1B8/0x1BC/0x1C0 packet head/tail/pool, +0x178 unlock flag,
// +0x1DC hotkeys[16] (8 bytes each), +0x4A8 pNext, +0x4AC pServerNext, +0x4B0 pServerNextByName,
// +0x508 dwLocale, +0x50C dwLangId.
static const char kClientsFile[] = "..\\Source\\D2Game\\GAME\\Clients.cpp";

// 1.13c D2Game+0x49B70
// CLIENTS_SetClientState (1.10f 0x6FC33970-area D2GAME_SetClientState): the client arrives in EAX
// (explicit here); a null client is the 0x95F fatal, otherwise store the state at client+0x04.
void CLIENTS_SetClientState(void* pClient, uint32_t nState)
{
    if (pClient == nullptr) d2game_fatal(0x95F);
    wr32(pClient, 0x04, nState);
}

// 1.13c D2Game+0x49D40
// CLIENTS_SetUnlockCharacter (1.10f sub_6FC33910): the client arrives in EAX (explicit here); a null
// client is the 0x7E1 fatal, otherwise set bUnlockCharacter (client+0x178) to 1.
EMSCRIPTEN_KEEPALIVE void d2_game_49d40(void* pClient)
{
    if (pClient == nullptr) d2game_fatal(0x7E1);
    wr32(pClient, 0x178, 1);
}

// 1.13c D2Game+0x4A9B0
// CLIENTS_AddToGame (1.10f 0x6FC325E0): the game is param_1; the client id arrives in EAX, the class /
// char template in BL, the name and account as stack args (EBP-relative), the char-save transaction
// token in EDI and the locale in SI (all explicit here). Requires the client list initialized, a game
// and fewer than 8 clients, else returns null. Allocates 0x146 dwords (0x518 bytes) through Fog #10045
// named Clients.cpp:0x331, zeroes them, then fills id, last-packet tick (GetTickCount + 180000 at
// +0x3D8), pGame, class (or char template + DATATBLS class from ARENA template type), name and account
// through Storm #501, transaction token, pClientInfo 0, lang id at +0x50C, state 0, empty packet list,
// the 16 hotkey entries (skill -1, hand 0, item GUID -1), links it at the head of game+0x88, pushes it
// into the id and by-name lists (Fog #10137 CRC16 of the name) and increments game+0x8C.
// Delta vs 1.10f: the two "[CLIENT] ClientAddToGame" GAME_LogMessage calls are gone; the decompile
// renders the CRC16 argument as the 0x331 line constant (register reuse) but 1.10f and the call's
// dataflow both hash the client name.
EMSCRIPTEN_KEEPALIVE void d2_game_4a9b0(void* pGame, uint32_t nClientId, uint8_t nClassIdOrCharTemplate,
                                        const char* szClientName, const char* szAccount,
                                        int32_t nCharSaveTransactionToken, uint32_t nLocale,
                                        void** ppClient, const D2GameBatch12Env* env)
{
    *ppClient = nullptr;
    if (g_clientListInitialized == 0 || pGame == nullptr) return;
    if ((int32_t)rd32(pGame, 0x8C) >= 8) return;

    void* client = env->allocPool(kClientsFile, 0x331, 0);   // Fog #10045
    std::memset(client, 0, 0x146 * 4);
    wr32(client, 0x00, nClientId);
    wr32(client, 0x3D8, (uint32_t)GetTickCount() + 180000);
    wrptr(client, 0x1A8, pGame);
    if (env->shouldTreatClassAsTemplateId(pGame) == 0) {     // 0xB2620
        wr8(client, 0x08, nClassIdOrCharTemplate);
    } else {
        wr8(client, 0x0C, nClassIdOrCharTemplate);
        const uint32_t templateType = env->getTemplateType(pGame);   // 0xB22D0
        wr8(client, 0x08, env->getClassFromCharTemplate(nClassIdOrCharTemplate, templateType));
    }
    env->strCopy((char*)client + 0x0D, szClientName, 0x7FFFFFFF);   // Storm #501
    env->strCopy((char*)client + 0x1D, szAccount, 0x7FFFFFFF);
    wr32(client, 0x60, (uint32_t)nCharSaveTransactionToken);
    wr32(client, 0x68, 0);
    wr32(client, 0x50C, nLocale);
    CLIENTS_SetClientState(client, 0);                        // 0x49B70
    wr32(client, 0x1B8, 0);
    wr32(client, 0x1BC, 0);
    wr32(client, 0x1C0, 0);
    for (uint32_t i = 0; i < 16; ++i) {
        wr16(client, 0x3DC + i * 8, 0xFFFF);
        wr8 (client, 0x3DE + i * 8, 0);
        wr32(client, 0x3E0 + i * 8, 0xFFFFFFFF);
    }
    wrptr(client, 0x4A8, rdptr(pGame, 0x88));
    wrptr(pGame, 0x88, client);

    EnterCriticalSection(&g_clientListLock);
    wrptr(client, 0x4AC, g_clientList[nClientId & 0xFF]);
    g_clientList[nClientId & 0xFF] = client;
    LeaveCriticalSection(&g_clientListLock);

    const uint8_t bucket = (uint8_t)env->crc16(szClientName);  // Fog #10137
    EnterCriticalSection(&g_clientByNameLock);
    wrptr(client, 0x4B0, g_clientListByName[bucket]);
    g_clientListByName[bucket] = client;
    LeaveCriticalSection(&g_clientByNameLock);

    wr32(pGame, 0x8C, rd32(pGame, 0x8C) + 1);
    *ppClient = client;
}

// 1.13c D2Game+0x49580
// The client arrives in EAX (explicit here); a null client is the 0xB9B fatal, otherwise set the 0x20
// bit of dwFlags (client+0x3D4). Called from GAME_CreateNewEmptyGame (0x2C2D0) for game type 1 right
// after CLIENTS_AddToGame. 1.10f has no matching helper; 0x20 is not in D2ClientFlagsEx.
EMSCRIPTEN_KEEPALIVE void d2_game_49580(void* pClient)
{
    if (pClient == nullptr) d2game_fatal(0xB9B);
    wr32(pClient, 0x3D4, rd32(pClient, 0x3D4) | 0x20);
}

// 1.13c D2Game+0x2BF90
// Act-init helper (1.10f inlined in GAME_SendActInit 0x6FC36C60): the game arrives in EAX and the
// client in EDI (both explicit here); a null client is the 0x1BF fatal (the 1.10f D2_ASSERT line).
// When the act record at game+0xBC+4*nAct (nAct = client+0x1AC) is null, load the act (0x9D6F0),
// then synchronize the day/night cycle with the client (0x9D610) and set the client state (+0x04) to 2.
EMSCRIPTEN_KEEPALIVE void d2_game_2bf90(void* pGame, void* pClient, const D2GameBatch12Env* env)
{
    if (pClient == nullptr) d2game_fatal(0x1BF);
    const uint8_t nAct = rd8(pClient, 0x1AC);
    if (rd32(pGame, 0xBC + (uint32_t)nAct * 4) == 0) env->levelLoadAct(pGame, nAct);   // 0x9D6F0
    env->levelSyncDayNight(pGame);                                                     // 0x9D610
    wr32(pClient, 0x04, 2);
}

// 1.13c D2Game+0x8A730
// D2GAME_PACKETS_SendPacket0x8F (1.10f 0x6FC3FA90): the client arrives in ESI (explicit here). Append
// the 0x21-byte D2GSPacketSrv8F whose only non-zero byte is the 0x8F header, through 0x8A3E0.
EMSCRIPTEN_KEEPALIVE void d2_game_8a730(void* pClient, const D2GameBatch12Env* env)
{
    uint8_t packet[0x21] = {};
    packet[0] = 0x8F;
    env->appendPacket(pClient, packet, 0x21);   // 0x8A3E0
}

// 1.13c D2Game seed step shared by the bounds roll below: lSeed = nLowSeed * 0x6AC690C5 + nHighSeed,
// stored back as the 8-byte union; the low dword is the random value.
static uint32_t d2game_seed_step(void* pSeed)
{
    const uint64_t v = (uint64_t)rd32(pSeed, 0) * 0x6AC690C5ull + rd32(pSeed, 4);
    std::memcpy(pSeed, &v, 8);
    return (uint32_t)v;
}

// 1.13c D2Game+0xCA760
// MONSTERREGION level monster selection, extracted in 1.13c from the 1.10f per-level body of
// MONSTERREGION_InitializeAll (0x6FC67600): param_1 is the LevelsTxt record, param_2 the
// D2MonsterRegionStrc; the difficulty>0 flag arrives in EAX and the D2SeedStrc in EBX (both explicit).
// nSpawn = min(min(levels->nNumMon, 13), levels->nNumNormalMonsters or nNumNightmareHellMonsters), and
// the matching id array (wNormalMonsters +0x36 or wNightmareHellMonsters +0x68) is picked from without
// repetition. Each iteration rolls a bounded index into the shrinking list (power-of-two bound uses a
// mask, otherwise modulo); on the first pick, a level with nRangedSpawn rolls up to 20/4 unrolled
// checks for a ranged MonStats flag. The chosen id is removed with memmove; when it is a spawnable
// MonStats record its id and nRarity are written into pMonData (+0x14, stride 0x34), nTotalRarity
// (+0x11) and nSpawnCount (+0x12) accumulate, and nMonCount (+0x10) increments. An empty spawn list
// returns early. The ranged retry is unrolled 5 check+roll steps per pass, 4 passes (20 rolls), same as 1.10f.
EMSCRIPTEN_KEEPALIVE void d2_game_ca760(const void* pLevels, void* pMonsterRegion, void* pSeed,
                                        uint32_t bNightmareHell, const D2GameBatch12Env* env)
{
    uint32_t nSpawn = (uint32_t)rd8(pLevels, 0x32);
    if (nSpawn > 0xD) nSpawn = 0xD;
    uint32_t nList = rd8(pLevels, 0x33);
    if (bNightmareHell != 0) nList = rd8(pLevels, 0x34);
    if (nList < nSpawn) nSpawn = nList;

    int16_t ids[25] = {};
    {
        uint32_t nCopy;
        const uint8_t* src;
        if (bNightmareHell == 0) {
            nCopy = rd8(pLevels, 0x33);
            src = (const uint8_t*)pLevels + 0x36;
        } else {
            nCopy = rd8(pLevels, 0x34);
            src = (const uint8_t*)pLevels + 0x68;
        }
        std::memcpy(ids, src, (size_t)nCopy * 2);
    }
    uint8_t* pMonData = (uint8_t*)pMonsterRegion + 0x14;

    for (uint32_t spawned = 0; spawned < nSpawn; ++spawned) {
        if ((int32_t)nList < 1) return;
        const uint32_t powerOfTwo = (nList - 1) & nList;
        uint32_t index = 0;
        int16_t monsterId = 0;
        // roll + pick
        uint32_t r = d2game_seed_step(pSeed);
        index = powerOfTwo == 0 ? (nList - 1) & r : r % nList;
        monsterId = ids[index];
        if (spawned == 0 && rd8(pLevels, 0x31) != 0) {
            for (uint32_t attempts = 0; attempts < 0x14; attempts += 5) {
                if (env->monStatsIsRanged(monsterId)) break;
                r = d2game_seed_step(pSeed);
                index = powerOfTwo == 0 ? (nList - 1) & r : r % nList;
                monsterId = ids[index];
                if (env->monStatsIsRanged(monsterId)) break;
                r = d2game_seed_step(pSeed);
                index = powerOfTwo == 0 ? (nList - 1) & r : r % nList;
                monsterId = ids[index];
                if (env->monStatsIsRanged(monsterId)) break;
                r = d2game_seed_step(pSeed);
                index = powerOfTwo == 0 ? (nList - 1) & r : r % nList;
                monsterId = ids[index];
                if (env->monStatsIsRanged(monsterId)) break;
                r = d2game_seed_step(pSeed);
                index = powerOfTwo == 0 ? (nList - 1) & r : r % nList;
                monsterId = ids[index];
                if (env->monStatsIsRanged(monsterId)) break;
                r = d2game_seed_step(pSeed);
                index = powerOfTwo == 0 ? (nList - 1) & r : r % nList;
                monsterId = ids[index];
            }
        }
        --nList;
        if ((int32_t)index < (int32_t)nList)
            std::memmove(&ids[index], &ids[index + 1], (size_t)(nList - index) * 2);
        if (env->monStatsIsSpawnable(monsterId)) {
            wr16(pMonData, 0x00, (uint16_t)monsterId);
            const uint8_t rarity = env->monStatsRarity(monsterId);
            wr8(pMonData, 0x02, rarity);
            wr8(pMonsterRegion, 0x11, (uint8_t)(rd8(pMonsterRegion, 0x11) + rarity));
            pMonData += 0x34;
            wr8(pMonsterRegion, 0x12, (uint8_t)(rd8(pMonsterRegion, 0x12) + 1));
            wr8(pMonsterRegion, 0x10, (uint8_t)(rd8(pMonsterRegion, 0x10) + 1));
        }
    }
}
