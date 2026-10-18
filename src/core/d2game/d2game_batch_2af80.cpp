// 1.13c D2Game alpha batch 3 (12 rows): game statistics, the game-data
// table's close path, client lookup / broadcast helpers, the client save dispatcher, and the task
// queue's clock + balance insert. Source of truth is the 1.13c image (build-wasm/ghidra-d2game/
// decomp/0x<RVA>.c) and the 1.10f sibling named below for each row.
//
// Offset-faithful: every read/write uses the retail offset the decompile shows, and module state is
// carried at its retail address in the statics below. Register arguments Ghidra dropped (EAX/ECX/EDX/
// ESI/EDI) are explicit parameters here; callees that need a register argument are seams in
// D2GameBatch3Env, following d2game_batch_2ae40.cpp.
//
// A missing game lock or a bad callback pointer is a protocol error in retail: it resolves the message
// id, prints it and _exit(-1). d2game_fatal models only the observable outcome.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <Windows.h>
#include <D2BasicTypes.h> // BOOL, needed by the compat Server.h below
#include <Server.h>       // LARGE_INTEGER and QueryPerformanceCounter no-op

namespace {

// D2GameStrc fields (src/core/d2moo/include/D2Game/GAME/Game.h).
constexpr uint32_t kGameLock = 0x18;              // lpCriticalSection
constexpr uint32_t kGameId = 0x28;                // nGameId (uint16)
constexpr uint32_t kGameType = 0x6A;              // nGameType
constexpr uint32_t kGameExpansion = 0x70;         // bExpansion
constexpr uint32_t kGameClientList = 0x88;        // pClientList
constexpr uint32_t kGameLastUsedGuid = 0x90;      // dwLastUsedUnitGUID[UNIT_TYPES_COUNT]
constexpr uint32_t kGameFrame = 0xA8;             // dwGameFrame
constexpr uint32_t kGameUnitList = 0x1120;        // pUnitList[5][128]

// D2ClientStrc fields (src/core/d2moo/include/D2Game/GAME/Clients.h).
constexpr uint32_t kClientSaveToken = 0x60;       // nCharSaveTransactionToken
constexpr uint32_t kClientInfo = 0x68;            // pClientInfo
constexpr uint32_t kClientSaveHeader = 0x17C;     // pSaveHeader
constexpr uint32_t kClientFlags = 0x3D4;          // dwFlags (bit 0x10: save header pending)
constexpr uint32_t kClientNext = 0x4A8;           // pNext
constexpr uint32_t kClientAccount = 0x1D;         // szAccount[16] + unk0x2D, SStrCopy reads 0x32

// D2UnitStrc fields (src/core/d2moo/include/Units/Units.h).
constexpr uint32_t kUnitType = 0x00;              // dwUnitType
constexpr uint32_t kUnitInteractType = 0x68;      // dwInteractType
constexpr uint32_t kUnitInteract = 0x6C;          // nInteract
constexpr uint32_t kUnitFlagEx = 0xC8;            // dwFlagEx (clear bit 1: UNITFLAGEX_UPDATEINV)
constexpr uint32_t kUnitListNext = 0xE4;          // pListNext

// D2PlayerData offsets: the client handle lives at +0x9C (D2GAME_SAVE_WriteFile's #10920 read).
constexpr uint32_t kPlayerDataClient = 0x9C;

// D2TaskStrc fields (1.10f GAME/Task.cpp: nTaskQ, nType, pTaskBalanceLink, pTaskQueueLink).
constexpr uint32_t kTaskQueueLink = 0x10;

// D2GSPacketSrv5A: szText is 32 bytes at +0x08; the scan below reads the first 16.
constexpr uint32_t kSrv5AText = 0x08;
constexpr uint32_t kSrv5ATextScan = 0x10;

// 1.10f D2GSPacketSrv26 layout (D2PacketDef.h).
struct D2GSPacketSrv26 {
    uint8_t nHeader;        //0x00
    uint8_t nMessageType;   //0x01
    uint8_t nLang;          //0x02
    uint8_t nUnitType;      //0x03
    uint32_t dwUnitGUID;    //0x04
    uint8_t nMessageColor;  //0x08
    uint8_t nNameColor;     //0x09
    char szName[16];        //0x0A
    char szMessage[256];    //0x1A
};

struct D2GameStatisticsStrc {
    uint32_t nGamesCount;        //0x00
    uint32_t nActiveMonsters;    //0x04
    uint32_t nInactiveMonsters;  //0x08
    uint32_t nPlayersCount;      //0x0C
};

struct D2LinkStrc {
    D2LinkStrc* pPrev;  //0x00
    D2LinkStrc* pNext;  //0x04
};

struct D2TaskStrc {
    int32_t nTaskQ;                //0x00
    int32_t nType;                 //0x04
    D2LinkStrc pTaskBalanceLink;   //0x08
    D2LinkStrc pTaskQueueLink;     //0x10
};

inline D2TaskStrc* taskFromQueueLink(D2LinkStrc* pLink)
{
    return reinterpret_cast<D2TaskStrc*>(reinterpret_cast<uint8_t*>(pLink) - kTaskQueueLink);
}

// Module state, at its 1.13c address.
uint32_t g_gameGuids[1024];                 // 0x6FD30C08: gnGamesGUIDs, 0 = free, -1 = reserved
void* g_pGameDataTable;                     // 0x6FD31C24: game-data table (game lock at +0x50)
void** g_pEventCallbacks;                   // 0x6FD31C3C: D2 event callback table
D2LinkStrc g_taskQueueLink;                 // 0x6FD30398: global task queue sentinel (pNext = first)
uint32_t g_taskSlots[32];                   // 0x6FD30318: D2TaskStrc* per queue slot
uint32_t g_tasksCreated;                    // 0x6FD303A8: gnNumberOfTasksCreated
uint32_t g_gamesCount;                      // 0x6FD303AC: gnGamesCount
uint32_t g_bHighResolutionClock;            // 0x6FD303B0: gbHasHighResolutionClock
uint32_t g_perfFrequencyLow;                // 0x6FD303A0
uint32_t g_perfFrequencyHigh;               // 0x6FD303A4
CRITICAL_SECTION g_taskQueueLocks[32];      // 0x6FD31C58 + i * 0x18
CRITICAL_SECTION g_taskGlobalLock;          // 0x6FD31F58

inline uint32_t rd32(const void* p, uint32_t o)
{
    uint32_t v;
    std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4);
    return v;
}

inline uint16_t rd16(const void* p, uint32_t o)
{
    uint16_t v;
    std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 2);
    return v;
}

inline void* rdptr(const void* p, uint32_t o)
{
    return reinterpret_cast<void*>(static_cast<uintptr_t>(rd32(p, o)));
}

inline void wr32(void* p, uint32_t o, uint32_t v)
{
    std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4);
}

inline uint8_t rd8(const void* p, uint32_t o)
{
    return static_cast<const uint8_t*>(p)[o];
}

// Ordinal 10265 (message id -> text) + Ordinal 10024 (FOG_DisplayHalt) + _exit(-1).
[[noreturn]] void d2game_fatal(uint32_t nMessageId)
{
    (void)nMessageId;
    _exit(-1);
}

// Game lock at +0x18, with retail's 0x100F failure.
inline void unlockGameOrDie(void* pGame)
{
    LPCRITICAL_SECTION pLock = static_cast<LPCRITICAL_SECTION>(rdptr(pGame, kGameLock));
    if (pLock == nullptr) d2game_fatal(0x100F);
    LeaveCriticalSection(pLock);
}

} // namespace

// Hidden-register callees and imports, one seam each. The address after each name is the 1.13c RVA.
struct D2GameBatch3Env {
    void* (*lockGame)(uint32_t nGameGUID);                                   // 0x2AAE0 GAME_LockGame, returns locked game
    void* (*findGameInTable)(uint32_t nGameGUID, void* pTable);              // 0x2130 hash lookup (EAX=GUID, EDX=table)
    void (*freeGame)(void* pGame, uint32_t nGameGUID);                       // 0x2CD30 (EAX=game, ECX=GUID)
    void (*assertIsGame)(void* pGame, uint32_t nLine);                       // 0xA328 D2_ASSERT
    void (*forEachIngameClient)(void* pGame, void* pfnCallback, const void* pPacket); // 0x49820
    void (*sendSaveHeaderPart)(void* pClient, void* pGame);                  // 0x2D140 (EAX=pClient, stack=pGame)
    void (*processClientPackets)(void* pGame, int32_t nFlag, void* pClient); // 0x2D500 (EAX=pClient)
    void (*saveSinglePlayer)(void* pClient, void* pGame, void* pUnit, int32_t bFlag); // 0x53E50 (ECX=pClient)
    void (*saveToFile)(void* pGame, void* pUnit, const char* szName, int32_t bFlag, uint32_t dwArg); // 0x53F10
    uint32_t (*saveToRealm)(void* pUnit, int32_t bFlag, void* pGame, const char* szName,
                            void* pAccountName, uint32_t nSaveToken, uint32_t dwArg,
                            uint32_t nClientInfo);                           // 0x54000 (ECX=bFlag, EDX=pUnit)
    void* (*getPlayerData)(void* pUnit);                                     // D2Common #10920
    void (*sstrCopy)(void* pDest, const void* pSrc, uint32_t nMax);          // Storm #501 SStrCopy
    void (*hashRemoveGame)(void* pTable, void* pGame);                       // 0x1F30 (same as removeGameFromTable)
};

// 1.10f sibling: D2Game.0x6FC3ACD0 GAME_GetStatistics (Game.cpp). 1.13c walks the lists in place
// instead of calling SUNIT_GetNextUnitList. PU lists: index 0 = players, index 1 = monsters.
// Note the 1.13c swap: pStats[1] counts monsters, pStats[3] counts players.
// 1.13c D2Game+0x2AF80 (ord 10059)
void GAME_GetStatistics(D2GameStatisticsStrc* pStats, const D2GameBatch3Env* pEnv)
{
    pStats->nGamesCount = 0;
    pStats->nActiveMonsters = 0;
    pStats->nInactiveMonsters = 0;
    pStats->nPlayersCount = 0;

    for (uint32_t nGuidIndex = 0; nGuidIndex < 1024; ++nGuidIndex)
    {
        const uint32_t nGameGUID = g_gameGuids[nGuidIndex];
        if (nGameGUID == 0 || nGameGUID == 0xFFFFFFFF) continue;

        void* pGame = pEnv->lockGame(nGameGUID);
        if (pGame == nullptr) continue;

        for (int32_t nRoom = 0; nRoom < 128; ++nRoom)
        {
            void* pUnit = rdptr(pGame, kGameUnitList + 0x200 + nRoom * 4); // list 1: monsters
            while (pUnit != nullptr)
            {
                ++pStats->nActiveMonsters;
                pUnit = rdptr(pUnit, kUnitListNext);
            }

            pUnit = rdptr(pGame, kGameUnitList + nRoom * 4);                // list 0: players
            while (pUnit != nullptr)
            {
                ++pStats->nPlayersCount;
                pUnit = rdptr(pUnit, kUnitListNext);
            }
        }

        unlockGameOrDie(pGame);
        ++pStats->nGamesCount;
    }
}

// 1.10f sibling: D2Game.0x6FC3A390 GAME_GetGameServerGameIds (Game.cpp). Same rule; the 1.13c
// element stride is 2 (mov %cx,(%ebx,%esi,2)), nGameId is a uint16 at +0x28.
// 1.13c D2Game+0x2B4D0 (ord 10060)
int32_t GAME_GetGameServerGameIds(uint16_t* pServerTokens, int32_t nMaxCount, const D2GameBatch3Env* pEnv)
{
    int32_t nServerTokenCount = 0;
    for (uint32_t nGuidIndex = 0; nGuidIndex < 1024; ++nGuidIndex)
    {
        if (nServerTokenCount >= nMaxCount) return nServerTokenCount;

        const uint32_t nGameGUID = g_gameGuids[nGuidIndex];
        if (nGameGUID == 0 || nGameGUID == 0xFFFFFFFF) continue;

        void* pGame = pEnv->lockGame(nGameGUID);
        if (pGame == nullptr) continue;

        pServerTokens[nServerTokenCount] = rd16(pGame, kGameId);
        ++nServerTokenCount;
        unlockGameOrDie(pGame);
    }
    return nServerTokenCount;
}

// 1.10f sibling: GAME/Clients.cpp CLIENTS_GetClientFromClientId. The decompile hides both arguments:
// EAX = pGame (0xE9 assert when null), ECX = nClientId; the list walks pNext at +0x4A8.
// 1.13c D2Game+0x49FE0 (ord 10004)
void* CLIENTS_GetClientFromClientId(void* pGame, int32_t nClientId)
{
    if (pGame == nullptr) d2game_fatal(0xE9);

    void* pClient = rdptr(pGame, kGameClientList);
    while (pClient != nullptr)
    {
        if (static_cast<int32_t>(rd32(pClient, 0)) == nClientId) return pClient;
        pClient = rdptr(pClient, kClientNext);
    }
    return nullptr;
}

// 1.10f sibling: D2Game.0x6FC84D40 sub_6FC84D40 (PLAYER/PlrMsg.cpp). The 1.13c entry takes the
// packet in EAX and the game as its single stack argument (ret 4); the packet's szText is 32 bytes at
// +0x08 and the scan stops at the first NUL inside the first 16.
// 1.13c D2Game+0xCBDB0
void sub_6FC84D40(void* pGame, const uint8_t* pPacket5A, const D2GameBatch3Env* pEnv)
{
    for (uint32_t i = 0; i < kSrv5ATextScan; ++i)
    {
        if (pPacket5A[kSrv5AText + i] == 0)
        {
            if (i < kSrv5ATextScan)
            {
                // 0x6FCEB570: j_D2GAME_SendPacket0x5A_6FC3DEC0 (callback in EDI).
                pEnv->forEachIngameClient(pGame, reinterpret_cast<void*>(0x6FCEB570u), pPacket5A);
            }
            return;
        }
    }
}

// 1.10f sibling: D2Game.0x6FC84CD0 sub_6FC84CD0 (PLAYER/PlrMsg.cpp). 1.13c ABI: pGame and nColor are
// stack arguments (ret 8), the message is EDX; the packet is built on the stack, message at +0x1A.
// 1.13c D2Game+0xCBE10
void sub_6FC84CD0(void* pGame, const char* szMessage, uint8_t nColor, const D2GameBatch3Env* pEnv)
{
    D2GSPacketSrv26 packet = {};
    packet.nHeader = 0x26;
    packet.nMessageType = 4;
    packet.nLang = 0;
    packet.nUnitType = 0;      // UNIT_PLAYER
    packet.dwUnitGUID = 0;
    packet.nMessageColor = nColor;
    packet.nNameColor = 0;
    std::memcpy(packet.szMessage, szMessage, sizeof(packet.szMessage));

    // 0x6FCEB580: j_D2GAME_PACKETS_SendPacket0x26_ServerMessage_6FC3DDF0 (callback in EDI).
    pEnv->forEachIngameClient(pGame, reinterpret_cast<void*>(0x6FCEB580u), &packet);
}

// 1.10f sibling: D2Game.0x6FC39600 GAME_CloseGame (Game.cpp). Same rule; 1.13c passes the game id in
// EAX and the lookup takes EDX = table. The callback arguments are (nGameId, tag, lastUsedGuid,
// dwGameFrame/25) with tag 'D2DV' + 0x13FA ('D2XP') when bExpansion is set.
// 1.13c D2Game+0x2CE20
void GAME_CloseGame(uint32_t nGameGUID, const D2GameBatch3Env* pEnv)
{
    void* pTable = g_pGameDataTable;
    if (pTable == nullptr) return;

    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(static_cast<uint8_t*>(pTable) + 0x50));

    void* pGame = pEnv->findGameInTable(nGameGUID, pTable);
    if (pGame == nullptr)
    {
        LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(static_cast<uint8_t*>(pTable) + 0x50));
        return;
    }

    if (g_pEventCallbacks != nullptr && g_pEventCallbacks[0] != nullptr)
    {
        if (IsBadCodePtr(reinterpret_cast<FARPROC>(g_pEventCallbacks[0]))) d2game_fatal(0x1027);

        const uint32_t nExpansionTag = 0x44324456u +
            (rd32(pGame, kGameExpansion) != 0 ? 0x13FAu : 0u); // 'D2DV' / 'D2XP'
        const int32_t nFrameSeconds = static_cast<int32_t>(rd32(pGame, kGameFrame)) / 25;
        // 1.13c: __fastcall (ECX = nGameId, EDX = tag; stack: lastUsedGuid, frame/25).
        reinterpret_cast<void (*)(uint32_t, uint32_t, uint32_t, int32_t)>(
            g_pEventCallbacks[0])(rd16(pGame, kGameId), nExpansionTag, rd32(pGame, kGameLastUsedGuid), nFrameSeconds);
    }

    if (rdptr(pGame, kGameLock) == nullptr) d2game_fatal(0x1038);
    pEnv->assertIsGame(pGame, 0x1039);

    pEnv->freeGame(pGame, nGameGUID);
    pEnv->hashRemoveGame(g_pGameDataTable, pGame);
}

// 1.13c split of 1.10f sub_6FC37B10's inner loop (GAME/Game.cpp): only single-player games, per
// client, while the save-header flag (dwFlags bit 0x10) is set and a save header is attached.
// Register ABI: EDI = pGame, ESI = pClient.
// 1.13c D2Game+0x2D790
void CLIENTS_FlushClientSaveHeader(void* pGame, void* pClient, const D2GameBatch3Env* pEnv)
{
    const uint8_t nGameType = rd8(pGame, kGameType);
    if (nGameType != 1 && nGameType != 2) return;

    if (pClient == nullptr) d2game_fatal(0xB8A);

    while ((rd8(pClient, kClientFlags) & 0x10) != 0 && rdptr(pClient, kClientSaveHeader) != nullptr)
    {
        pEnv->sendSaveHeaderPart(pClient, pGame);
    }
}

// 1.13c split of 1.10f GAME_UpdateGames (Game.cpp): walk the client list and process each client's
// queues with flag 1. The 1.13c loop advances to pNext before the call, so the tail client is not
// processed by the last iteration (retail quirk, kept).
// Register ABI: EDI = pGame, the single stack argument is unused.
// 1.13c D2Game+0x2D980
void GAME_ProcessAllClientPackets(void* pGame, uint32_t nUnusedArg, const D2GameBatch3Env* pEnv)
{
    (void)nUnusedArg;

    void* pClient = rdptr(pGame, kGameClientList);
    while (pClient != nullptr)
    {
        void* pNextClient = rdptr(pClient, kClientNext);
        pEnv->processClientPackets(pGame, 1, pClient);
        pClient = pNextClient;
    }
}

// Counts the units in a 128-entry unit-list array through pListNext at +0xE4. 1.13c callers pass the
// list base in ESI (GAME_GetPlayerUnitsCount-style callers 0x2B440/0x2B550 call it five times). The
// unreachable NUL check is retail's 0xDF assert (kept as the fatal).
// 1.13c D2Game+0x2A680
int32_t GAME_GetUnitCount(void* const* pUnitLists)
{
    int32_t nCount = 0;
    for (int32_t i = 0; i < 128; ++i)
    {
        void* pUnit = pUnitLists[i];
        while (pUnit != nullptr)
        {
            ++nCount;
            pUnit = rdptr(pUnit, kUnitListNext);
        }
        if (pUnit != nullptr) d2game_fatal(0xDF); // dead branch: the loop only ends on null
    }
    return nCount;
}

// 1.10f sibling: TASK_GetClockTime (GAME/Task.cpp). 1.13c keeps the full 64-bit
// (counter * 1000 / frequency) result in EDX:EAX; the 1.10f comment's "truncated to 32 bits" is a
// 1.10f artifact. Without the high-resolution clock the result is GetTickCount().
// 1.13c D2Game+0x52020
uint64_t TASK_GetClockTime()
{
    if (g_bHighResolutionClock != 0)
    {
        LARGE_INTEGER nCounter;
        QueryPerformanceCounter(&nCounter);
        return static_cast<uint64_t>(nCounter.QuadPart) * 1000u /
            (static_cast<uint64_t>(g_perfFrequencyHigh) << 32 | g_perfFrequencyLow);
    }
    return GetTickCount();
}

// 1.10f sibling: the queue-balancing body of TASK_Create (GAME/Task.cpp 0x6FC405B0); 1.13c factored
// it into this function (1.13c TASK_Create 0x52650 calls it with the freshly created task).
// Balances the new task into each queue slot's type-ordered list, then pushes it on the global task
// queue at +0x10. Globals: 0x6FD30318 slots, 0x6FD303A8 tasks created, 0x6FD303AC games count,
// 0x6FD31C58 per-slot locks (stride 0x18), 0x6FD30398 queue sentinel.
// 1.13c D2Game+0x524C0
void TASK_InsertIntoQueues(D2TaskStrc* pTask)
{
    uint32_t nPerTask = g_gamesCount / g_tasksCreated;
    uint32_t nSlotLimit = g_tasksCreated - 1;
    if (nSlotLimit > 0x1F) nSlotLimit = 0x20;

    int32_t nRemainingSlots = static_cast<int32_t>(nSlotLimit) - static_cast<int32_t>(g_gamesCount % g_tasksCreated);

    for (D2LinkStrc* pQueueLink = g_taskQueueLink.pNext; pQueueLink != &g_taskQueueLink;
         pQueueLink = pQueueLink->pNext)
    {
        --nRemainingSlots;

        D2TaskStrc* pQueuedTask = taskFromQueueLink(pQueueLink);

        // The 64-bit (nType, nPerTask + !(nRemainingSlots < 0)) - 1 high dword: nType minus one when
        // the low dword is zero.
        int32_t nBalanceSteps = pQueuedTask->nType -
            ((nPerTask + (nRemainingSlots >= 0 ? 1u : 0u)) == 0 ? 1 : 0);

        LPCRITICAL_SECTION pSlotLock = &g_taskQueueLocks[pQueuedTask->nTaskQ];
        EnterCriticalSection(pSlotLock);

        D2LinkStrc* pBalanceSelf = &pQueuedTask->pTaskBalanceLink;
        D2LinkStrc* pBalanceIterator = pBalanceSelf->pPrev;
        D2LinkStrc* pUnlinkedLink = pBalanceIterator;

        while (nBalanceSteps != 0 && (--nBalanceSteps, pBalanceIterator != pBalanceSelf))
        {
            pBalanceIterator = pBalanceIterator->pNext;
        }

        D2LinkStrc* pIteratorNext = pBalanceIterator->pNext;
        pBalanceIterator->pNext = pBalanceSelf;
        pBalanceSelf->pPrev = pBalanceIterator;

        LeaveCriticalSection(pSlotLock);

        if (pIteratorNext != pBalanceSelf)
        {
            D2LinkStrc* pTaskBalanceLink = &pTask->pTaskBalanceLink;

            pUnlinkedLink->pNext = nullptr;

            // Insert pIteratorNext into the new task's balance list, ordered by the queued task's slot
            // (nTaskQ, stored at -0x10 from a queue link), walking back from the head sentinel.
            D2LinkStrc* pCurrent = pTaskBalanceLink->pPrev;
            while (pCurrent != pTaskBalanceLink)
            {
                const int32_t nQueuedSlot = taskFromQueueLink(pBalanceIterator)->nTaskQ;
                const int32_t nCurrentSlot = taskFromQueueLink(pCurrent)->nTaskQ;
                if (nQueuedSlot - nCurrentSlot <= 0) break;
                pCurrent = pCurrent->pPrev;
            }

            if (pCurrent->pNext == pTaskBalanceLink)
            {
                pIteratorNext->pPrev = pCurrent;
                pUnlinkedLink->pNext = pTaskBalanceLink;
                pCurrent->pNext = pIteratorNext;
                pTaskBalanceLink->pPrev = pUnlinkedLink;
            }
            else
            {
                for (;;)
                {
                    D2LinkStrc* pNext = pIteratorNext->pNext;
                    pIteratorNext->pPrev = pCurrent;
                    D2LinkStrc* pCurrentNext = pCurrent->pNext;
                    pIteratorNext->pNext = pCurrentNext;
                    pCurrentNext->pPrev = pIteratorNext;
                    pCurrent->pNext = pIteratorNext;
                    if (pNext == nullptr) break;

                    pCurrent = pIteratorNext;
                    pIteratorNext = pNext;

                    D2LinkStrc* pCompare = pCurrent->pNext;
                    if (pCompare != pTaskBalanceLink)
                    {
                        do
                        {
                            const int32_t nNextSlot = taskFromQueueLink(pNext)->nTaskQ;
                            const int32_t nCompareSlot = taskFromQueueLink(pCompare)->nTaskQ;
                            if (nNextSlot - nCompareSlot <= 0) break;
                            pCurrent = pCompare;
                            pCompare = pCompare->pNext;
                        } while (pCompare != pTaskBalanceLink);
                    }

                    if (pCurrent->pNext == pTaskBalanceLink)
                    {
                        pIteratorNext->pPrev = pCurrent;
                        pUnlinkedLink->pNext = pTaskBalanceLink;
                        pCurrent->pNext = pIteratorNext;
                        pTaskBalanceLink->pPrev = pUnlinkedLink;
                        break;
                    }
                }
            }
        }
    }

    D2LinkStrc* pTaskQueueLink = &pTask->pTaskQueueLink;
    pTaskQueueLink->pPrev = &g_taskQueueLink;
    pTaskQueueLink->pNext = g_taskQueueLink.pNext;
    g_taskQueueLink.pNext->pPrev = pTaskQueueLink;
    g_taskQueueLink.pNext = pTaskQueueLink;
}

// 1.10f sibling: D2Game.0x6FC8A500 D2GAME_SAVE_WriteFile (PLAYER/PlrSave.cpp). The 1.13c body keeps
// the same three arms, factored into 0x53E50 (single player: attach the save to the client),
// 0x54000 (realm callback table present) and 0x53F10 (write <name>.d2s). Register ABI: EBX = pGame,
// ESI = pUnit; stack arguments are the character name and dwArg (ret 8).
// 1.13c D2Game+0x54320
uint32_t D2GAME_SAVE_WriteFile(void* pGame, void* pUnit, const char* szName, uint32_t dwArg,
                               const D2GameBatch3Env* pEnv)
{
    if (pUnit == nullptr) return 0;

    wr32(pUnit, kUnitFlagEx, rd32(pUnit, kUnitFlagEx) & ~2u); // UNITFLAGEX_UPDATEINV

    // SUNIT_GetInteractInfo: a player target => bInteractsWithPlayer.
    int32_t bInteractsWithPlayer = 0;
    if (rd8(pUnit, kUnitInteract) != 0 && rd32(pUnit, kUnitInteractType) == 0)
    {
        bInteractsWithPlayer = 1;
    }

    void* pClient = nullptr;
    if (rd32(pUnit, kUnitType) == 0)
    {
        void* pPlayerData = pEnv->getPlayerData(pUnit);
        pClient = rdptr(pPlayerData, kPlayerDataClient);
    }

    char szAccountName[50] = {};
    pEnv->sstrCopy(szAccountName, static_cast<const uint8_t*>(pClient) + kClientAccount, 0x32);

    const uint8_t nGameType = rd8(pGame, kGameType);
    if (nGameType == 1 || nGameType == 2)
    {
        pEnv->saveSinglePlayer(pClient, pGame, pUnit, bInteractsWithPlayer);
        return 1;
    }

    if (g_pEventCallbacks != nullptr)
    {
        return pEnv->saveToRealm(pUnit, bInteractsWithPlayer, pGame, szName, szAccountName,
                                 rd32(pClient, kClientSaveToken), dwArg, rd32(pClient, kClientInfo));
    }

    pEnv->saveToFile(pGame, pUnit, szName, bInteractsWithPlayer, dwArg);
    return 1;
}
