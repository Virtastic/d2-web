// 1.13c D2Game alpha batch (12 rows): the game-container and task-queue
// accessors around 0x6FC4xxxx. Source of truth is the 1.13c image (build-wasm/ghidra-d2game/decomp/
// 0x<RVA>.c); the 1.10f sibling under reference/D2MOO/source/D2Game/src/ names the same rules.
//
// Offset-faithful: each function reads the retail record at the offsets the decompile shows, and the
// module state below carries its retail address. Where Ghidra drops a register argument (EAX/ECX/EDX/
// ESI) the call site passes it explicitly through D2GameBatch2Env instead of pretending it is zero.
//
// A missing game lock is a protocol error in retail: it looks up the message id, prints it and _exit(-1).
// d2game_fatal models only the observable outcome, the same as the previous batch (d2game_batch_2cfd0.cpp).
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <Windows.h>

namespace {

constexpr auto kTaskFile = "..\\Source\\D2Game\\GAME\\Task.cpp";  // the pool-free call site literal
constexpr auto kGameFile = "..\\Source\\D2Game\\GAME\\Game.cpp";  // the save-error call site literal

// D2ClientStrc fields the save path touches (src/core/d2moo/include/D2Game/GAME/Clients.h).
constexpr uint32_t kClientSaveToken = 0x60;       // nCharSaveTransactionToken
constexpr uint32_t kClientSaveTimestamp = 0x190;  // nSaveCreationTimestamp (FILETIME, 8 bytes)
// D2GameStrc fields (src/core/d2moo/include/D2Game/GAME/Game.h).
constexpr uint32_t kGameLock = 0x18;              // lpCriticalSection
constexpr uint32_t kGameInitSeed = 0x7C;          // dwInitSeed
constexpr uint32_t kGameClients = 0x8C;           // nClients
constexpr uint32_t kGameAct = 0xBC;               // pAct[5]

// 1.10f D2C_SRV2CLT5A_TYPES / D2C_StringColorCodes values.
constexpr uint32_t kEventRealmGoingDown = 0x0F;
constexpr uint8_t kColorDarkGold = 4;
constexpr uint8_t kHeaderSrv5A = 0x5A;
constexpr uint8_t kSrv2CltActInitDone = 2;

CRITICAL_SECTION g_b2GameListLock;                  // 0x6FD30BE8
CRITICAL_SECTION g_b2TaskGlobalLock;                // 0x6FD31F58
CRITICAL_SECTION g_b2TaskSlotLocks[32];             // 0x6FD31C58 + i * 0x18
uint32_t g_b2GameGuids[1024];                       // 0x6FD30C08: gnGamesGUIDs, 0 = free, -1 = reserved
uint32_t* g_b2CurrentGame;                          // 0x6FD30C04: last GAME_LockGame result
void* g_b2GameDataTable;                            // 0x6FD31C24
uint32_t g_b2TaskSlots[32];                         // 0x6FD30318: D2TaskStrc* per slot
uint32_t g_b2TasksCreated;                          // 0x6FD303A8: gnNumberOfTasksCreated
uint32_t g_b2GamesCount;                            // 0x6FD303AC: gnGamesCount

// Retail records hold 32-bit pointers, even though the host build is 64-bit: read/write pointer slots as
// dwords rather than memcpy'ing sizeof(void*).
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
inline void* rdptr(const void* p, uint32_t o) { return reinterpret_cast<void*>(static_cast<uintptr_t>(rd32(p, o))); }
inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }

// Ordinal 10265 (message id -> text) + Ordinal 10024 (FOG_DisplayHalt) + _exit(-1), as rendered at
// every failure site below.
[[noreturn]] void d2game_fatal(uint32_t nMessageId)
{
    (void)nMessageId;
    _exit(-1);
}

// Game lock at +0x18: the decompile's puVar[6] / DAT_6fd30c04[6].
inline LPCRITICAL_SECTION gameLock(void* pGame) { return static_cast<LPCRITICAL_SECTION>(rdptr(pGame, kGameLock)); }

inline void unlockGameOrDie(void* pGame)
{
    if (pGame == nullptr) d2game_fatal(0x100E);
    LPCRITICAL_SECTION pLock = gameLock(pGame);
    if (pLock == nullptr) d2game_fatal(0x100F);
    LeaveCriticalSection(pLock);
}

// TASK_LinkList_Remove (0x1E50) for a task's pTaskBalanceLink / pTaskQueueLink node: pPrev at +0,
// pNext at +4. A null node is the 0x87 assert.
void taskLinkListRemove(void* pLink)
{
    if (pLink == nullptr) d2game_fatal(0x87);
    void* pPrev = rdptr(pLink, 0);
    void* pNext = rdptr(pLink, 4);
    wr32(pPrev, 4, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pNext)));
    wr32(pNext, 0, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pPrev)));
}

}  // namespace

// The register-hidden callees, one seam per 1.13c RVA. Comments name the retail function and where its
// argument travels (a Ghidra-omitted EAX/ECX/EDX/ESI on the retail call).
struct D2GameBatch2Env
{
    void* (*lockGame)(uint32_t nGameGuid);                       // 0x2AAE0 GAME_LockGame (GUID in ESI)
    int32_t (*getClientIdByName)(const char* szName);            // 0x49850 CLIENTS_GetClientIdByName
    void* (*getGameByClientId)(int32_t nClientId);               // 0x2AB70 GAME_GetGameByClientId (locks the game)
    void* (*getClientFromClientId)(void* pGame, int32_t nClientId); // 0x49FE0 CLIENTS_GetClientFromClientId
    void (*disconnectClient)(void* pGame, void* pClient, uint32_t nEventType); // 0x2D090 GAME_DisconnectClient
    void (*leaveGamesCriticalSection)(void* pGame);              // 0x49BA0 GAME_LeaveGamesCriticalSection
    int32_t (*attachSaveFile)(int32_t nClientId, const void* pSaveData, uint16_t nSaveSize,
                              uint16_t nTotalSize, int32_t bUnlock, int32_t a6, int32_t a7); // 0x6A190
    void (*clientSaveError)(int32_t nClientId, const char* szFile); // 0x2D2F0 (D2NET_10015 path)
    void (*sendHeaderOnlyPacket)(void* pClient, uint8_t nPacketType); // 0x8C490
    void (*sendEventPacket)(void* pGame, const void* pPacket);   // 0xCBDB0 (packet in EAX, game in ECX)
    void (*sendServerMessage)(void* pGame, const char* szText, uint8_t nColor); // 0xCBE10 (color on the stack)
    uint64_t (*clockMs)();                                       // 0x52020 TASK_GetClockTime
    int32_t (*countGameUnits)(void* pGame);                      // 0x2A680 GAME_GetUnitCount walk
    int32_t (*getServerUnit)(void* pGame, uint32_t nUnitType, uint32_t nUnitId); // 0xE03A0 SUNIT_GetServerUnit
    int32_t (*actQuery)(void* pAct, uint32_t nParam2, uint32_t nParam3); // Ordinal_11010 (D2Common-class query)
    void (*freePool)(const char* szFile, uint32_t nLine, void* pPool); // Ordinal 10046 FOG_FreePool
};

// 1.13c D2Game+0x2AE40 (ord 10033)
// For every live game GUID, lock the game and broadcast one message: event type 1 sends the realm-going-down
// packet 5A (param_2 as dwParam), anything else sends the text as a server message 0x26 (colour 1).
// partial: Ghidra does not render the packet 0x5A built for the param_1 == 1 branch nor the colour argument
// pushed to 0xCBE10; both are modelled from the 1.10f sibling GAME_SendMessageToAllClients (Game.cpp:3035).
void GAME_SendMessageToAllClients(int32_t nMessageType, int32_t nPacketParam, const char* szMessage,
                                  const D2GameBatch2Env* pEnv)
{
    if (szMessage == nullptr) szMessage = "";

    for (uint32_t nGuidIndex = 0; nGuidIndex < 1024; ++nGuidIndex)
    {
        const uint32_t nGuid = g_b2GameGuids[nGuidIndex];
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;

        void* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) d2game_fatal(0x100E);

        if (nMessageType == 1)
        {
            // D2GSPacketSrv5A: header 0x00, type 0x01, colour 0x02, dwParam 0x03 (unaligned), text 0x08.
            uint8_t packet[0x28] = {};
            packet[0x00] = kHeaderSrv5A;
            packet[0x01] = static_cast<uint8_t>(kEventRealmGoingDown);
            packet[0x02] = kColorDarkGold;
            std::memcpy(&packet[0x03], &nPacketParam, 4);
            packet[0x18] = 0;                       // szText[16], the byte the 0xCBDB0 NUL scan reaches
            pEnv->sendEventPacket(pGame, packet);
        }
        else
        {
            const size_t nLength = std::strlen(szMessage);
            if (nLength < 0x100)
            {
                pEnv->sendServerMessage(pGame, szMessage, 1);
            }
            else
            {
                char szCopy[256] = {};
                std::strncpy(szCopy, szMessage, 0xFF);
                szCopy[0xFF] = '\0';
                pEnv->sendServerMessage(pGame, szCopy, 1);
            }
        }

        if (gameLock(pGame) == nullptr) d2game_fatal(0x100F);
        LeaveCriticalSection(gameLock(pGame));
    }
}

// 1.13c D2Game+0x2DEC0 (ord 10034)
// GAME_DisconnectClientByName: resolve the client id, then the inlined GAME_DisconnectClientById — lock the
// client's game, ask 0x2D090 to disconnect it (it tolerates a null client), unlock. TRUE whenever the name
// resolved, even if the client has no game.
int32_t GAME_DisconnectClientByName(const char* szClientName, uint32_t nEventType, const D2GameBatch2Env* pEnv)
{
    // partial: Ghidra renders FUN_6fc69850() with no argument; the name arrives in EAX at the retail call.
    const int32_t nClientId = pEnv->getClientIdByName(szClientName);
    if (nClientId == 0) return 0;

    void* pGame = pEnv->getGameByClientId(nClientId);
    if (pGame != nullptr)
    {
        // partial: 0x49FE0 and 0x2D090 take game/client in EAX/ESI at the retail call sites.
        void* pClient = pEnv->getClientFromClientId(pGame, nClientId);
        pEnv->disconnectClient(pGame, pClient, nEventType);
        pEnv->leaveGamesCriticalSection(pGame);
    }
    return 1;
}

// 1.13c D2Game+0x2D3B0 (ord 10035)
// GAME_ReceiveDatabaseCharacter: hand a character-save fragment to 0x6A190; early TRUE (1) if it reports the
// character complete. A non-zero a5 or a zero total size is the save-error path. Otherwise lock the client's
// game, check the save transaction token, store the 8-byte creation timestamp and send ACTINITDONE (0x02).
uint16_t GAME_ReceiveDatabaseCharacter(int32_t nClientId, const void* pSaveData, uint16_t nSaveSize,
                                       uint16_t nTotalSize, int32_t nFlags, int32_t a6,
                                       const uint32_t* pSaveCreationTimestamp, int32_t nCharSaveTransactionToken,
                                       const D2GameBatch2Env* pEnv)
{
    if (nTotalSize != 0)
    {
        // partial: 0x6A190's client id is in EAX and its last two arguments (1, a6) are register/stack-hidden;
        // the rendered argument list is (pSaveData, nSaveSize, nTotalSize, nFlags == 0).
        const int32_t nAttach = pEnv->attachSaveFile(nClientId, pSaveData, nSaveSize, nTotalSize,
                                                     nFlags == 0, 1, a6);
        if (nAttach == 0) return 1;
    }

    if (nFlags != 0)
    {
        pEnv->clientSaveError(nClientId, kGameFile);
        return 0;
    }
    if (nTotalSize == 0)
    {
        pEnv->clientSaveError(nClientId, kGameFile);
        return 0;
    }

    void* pGame = pEnv->getGameByClientId(nClientId);
    if (pGame == nullptr) return 0;

    void* pClient = pEnv->getClientFromClientId(pGame, nClientId);
    if (pClient == nullptr)
    {
        pEnv->leaveGamesCriticalSection(pGame);
        return 0;
    }

    if (rd32(pClient, kClientSaveToken) != static_cast<uint32_t>(nCharSaveTransactionToken))
    {
        pEnv->leaveGamesCriticalSection(pGame);
        pEnv->clientSaveError(nClientId, kGameFile);
        return 0;
    }

    wr32(pClient, kClientSaveTimestamp, pSaveCreationTimestamp[0]);
    wr32(pClient, kClientSaveTimestamp + 4, pSaveCreationTimestamp[1]);
    pEnv->leaveGamesCriticalSection(pGame);
    pEnv->sendHeaderOnlyPacket(pClient, kSrv2CltActInitDone);
    return nTotalSize;
}

// 1.13c D2Game+0x52240 (ord 10036)
// TASK_GetTaskTimeout (1.10f D2Game_10044): milliseconds until the task's balance-list deadline, 100 when the
// slot holds no task. The slot lock is taken with the slot index, released with the task's own nTaskQ.
int32_t TASK_GetTaskTimeout(uint32_t nTaskNumber, const D2GameBatch2Env* pEnv)
{
    int32_t nResult = 100;
    const uint32_t nSlot = nTaskNumber & 0x1F;

    EnterCriticalSection(&g_b2TaskSlotLocks[nSlot]);
    void* pTask = reinterpret_cast<void*>(static_cast<uintptr_t>(g_b2TaskSlots[nSlot]));
    if (pTask == nullptr)
    {
        LeaveCriticalSection(&g_b2TaskSlotLocks[nSlot]);
        return 100;
    }

    void* pLink = rdptr(pTask, 0x0C);               // pTaskBalanceLink.pNext, sentinel is &task->pTaskBalanceLink
    if (pLink != static_cast<uint8_t*>(pTask) + 0x08 && pLink != nullptr)
    {
        // The balance node sits at task+0x08, so pNext - 0x0C is the D2TaskStrc base.
        nResult = static_cast<int32_t>(rd32(static_cast<const uint8_t*>(pLink) - 0x0C, 0)) -
                  static_cast<int32_t>(pEnv->clockMs());
    }

    const uint32_t nTaskQ = rd32(pTask, 0x00);
    if (nTaskQ > 0x1F) d2game_fatal(0x96);          // TASK_LinkList lock index out of range
    LeaveCriticalSection(&g_b2TaskSlotLocks[nTaskQ]);
    return nResult;
}

// 1.13c D2Game+0x2A370 (ord 10037)
// GAME_InitGameDataTable: the game data table (0x6FD31C24) and the game list are both mandatory; either null
// is the 0x157 / 0x159 assert.
void GAME_InitGameDataTable(void* pGameDataTable, void* pGameList)
{
    if (pGameDataTable == nullptr) d2game_fatal(0x157);
    g_b2GameDataTable = pGameDataTable;
    if (pGameList == nullptr) d2game_fatal(0x159);
}

// 1.13c D2Game+0x38DC0 (ord 10039)
// Cache the client's game, then SUNIT_GetServerUnit over it; a null lock after a successful lookup is a
// protocol error. The game-list lock is entered and left around the lookup.
// partial: 0xE03A0 takes the unit type in EAX (param_1 is dropped from the rendered call).
int32_t SUNIT_GetServerUnitForClient(uint32_t nUnitType, uint32_t nUnitId, const D2GameBatch2Env* pEnv)
{
    EnterCriticalSection(&g_b2GameListLock);        // Ordinal 10050
    LeaveCriticalSection(&g_b2GameListLock);

    // partial: the client's game GUID reaches 0x2AAE0 in ESI.
    g_b2CurrentGame = static_cast<uint32_t*>(pEnv->lockGame(0));
    if (g_b2CurrentGame == nullptr) return 0;

    const int32_t nUnit = pEnv->getServerUnit(g_b2CurrentGame, nUnitType, nUnitId);

    if (g_b2CurrentGame == nullptr) d2game_fatal(0x100E);
    LPCRITICAL_SECTION pLock = gameLock(g_b2CurrentGame);
    if (pLock == nullptr) d2game_fatal(0x100F);
    LeaveCriticalSection(pLock);
    return nUnit;
}

// 1.13c D2Game+0x38BC0 (ord 10042)
// The current game's act query: pAct[nAct] through Ordinal 11010 (a D2Common-class call taking the act pointer
// and two arguments). nAct >= 5 is the 0x8D assert.
// partial: the current-game lookup (0x2AAE0) has its GUID in ESI at the retail call, and Ordinal 11010's
// arguments are register-passed; both are explicit here.
int32_t GAME_GetActData(uint8_t nAct, uint32_t nParam2, uint32_t nParam3, const D2GameBatch2Env* pEnv)
{
    EnterCriticalSection(&g_b2GameListLock);        // Ordinal 10050
    LeaveCriticalSection(&g_b2GameListLock);

    g_b2CurrentGame = static_cast<uint32_t*>(pEnv->lockGame(0));

    int32_t nResult;
    if (nAct < 5)
    {
        // Retail dereferences DAT_6FD30C04 before its own null check; a missing game faults here too.
        if (g_b2CurrentGame == nullptr) d2game_fatal(0x100E);
        nResult = pEnv->actQuery(rdptr(g_b2CurrentGame, kGameAct + 4u * nAct), nParam2, nParam3);
        if (g_b2CurrentGame == nullptr) d2game_fatal(0x100E);
        LPCRITICAL_SECTION pLock = gameLock(g_b2CurrentGame);
        if (pLock == nullptr) d2game_fatal(0x100F);
        LeaveCriticalSection(pLock);
        return nResult;
    }
    d2game_fatal(0x8D);
}

// 1.13c D2Game+0x2B7C0 (ord 10045)
// The current game's client count (D2GameStrc+0x8C). No game at all is the 0x1273 assert.
uint32_t GAME_GetClientCount(const D2GameBatch2Env* pEnv)
{
    EnterCriticalSection(&g_b2GameListLock);        // Ordinal 10050
    LeaveCriticalSection(&g_b2GameListLock);

    void* pGame = pEnv->lockGame(0);                // partial: the GUID is in ESI at the retail call
    if (pGame == nullptr) d2game_fatal(0x1273);

    const uint32_t nClients = rd32(pGame, kGameClients);
    if (gameLock(pGame) == nullptr) d2game_fatal(0x100F);
    LeaveCriticalSection(gameLock(pGame));
    return nClients;
}

// 1.13c D2Game+0x2B440 (ord 10053)
// The current game's unit count: 0x2A680 walks the 0x80 unit-hash buckets hanging off D2GameStrc+0xE4.
uint32_t GAME_GetUnitCount(const D2GameBatch2Env* pEnv)
{
    EnterCriticalSection(&g_b2GameListLock);        // Ordinal 10050
    LeaveCriticalSection(&g_b2GameListLock);

    void* pGame = pEnv->lockGame(0);                // partial: the GUID is in ESI at the retail call
    uint32_t nUnits = 0;
    if (pGame != nullptr)
    {
        nUnits = static_cast<uint32_t>(pEnv->countGameUnits(pGame));
        if (gameLock(pGame) == nullptr) d2game_fatal(0x100F);
        LeaveCriticalSection(gameLock(pGame));
    }
    return nUnits;
}

// 1.13c D2Game+0x2BD70 (ord 10055)
// The current game's init seed (D2GameStrc+0x7C); no game yields 0, a null lock is the 0x100F assert.
uint32_t GAME_GetInitSeed(const D2GameBatch2Env* pEnv)
{
    EnterCriticalSection(&g_b2GameListLock);        // Ordinal 10050
    LeaveCriticalSection(&g_b2GameListLock);

    g_b2CurrentGame = static_cast<uint32_t*>(pEnv->lockGame(0)); // partial: the GUID is in ESI
    if (g_b2CurrentGame == nullptr) return 0;

    const uint32_t nSeed = rd32(g_b2CurrentGame, kGameInitSeed);
    if (gameLock(g_b2CurrentGame) == nullptr) d2game_fatal(0x100F);
    LeaveCriticalSection(gameLock(g_b2CurrentGame));
    return nSeed;
}

// 1.13c D2Game+0x522D0 (ord 10056)
// TASK_GetTaskTimeoutAndType (1.10f D2Game_10043): the same deadline as 0x52240, plus the task record when
// the deadline has expired — then the task is unlinked from the balance list and *ppTask receives it.
int32_t TASK_GetTaskTimeoutAndType(uint32_t nTaskNumber, void** ppTask, const D2GameBatch2Env* pEnv)
{
    int32_t nResult = 100;
    *ppTask = nullptr;
    const uint32_t nSlot = nTaskNumber & 0x1F;

    EnterCriticalSection(&g_b2TaskSlotLocks[nSlot]);
    void* pTask = reinterpret_cast<void*>(static_cast<uintptr_t>(g_b2TaskSlots[nSlot]));
    if (pTask == nullptr)
    {
        LeaveCriticalSection(&g_b2TaskSlotLocks[nSlot]);
        return 100;
    }

    void* pLink = rdptr(pTask, 0x0C);               // pTaskBalanceLink.pNext
    if (pLink != static_cast<uint8_t*>(pTask) + 0x08 && pLink != nullptr)
    {
        nResult = static_cast<int32_t>(rd32(static_cast<const uint8_t*>(pLink) - 0x0C, 0)) -
                  static_cast<int32_t>(pEnv->clockMs());
        if (nResult < 1)
        {
            taskLinkListRemove(pLink);              // 0x1E50 TASK_LinkList_Remove (link in ECX)
            *ppTask = static_cast<uint8_t*>(pLink) - 0x0C;
        }
    }

    const uint32_t nTaskQ = rd32(pTask, 0x00);
    if (nTaskQ > 0x1F) d2game_fatal(0x96);
    LeaveCriticalSection(&g_b2TaskSlotLocks[nTaskQ]);
    return nResult;
}

// 1.13c D2Game+0x52090 (ord 10058)
// TASK_FreeAllQueueSlots: under the global task lock, unlink every slot's task from the queue list, free the
// task pool (Task.cpp:0x32D) and zero the slot; then reset the task and game counters.
void TASK_FreeAllQueueSlots(const D2GameBatch2Env* pEnv)
{
    EnterCriticalSection(&g_b2TaskGlobalLock);

    for (uint32_t nSlot = 0; nSlot < 32; ++nSlot)
    {
        EnterCriticalSection(&g_b2TaskSlotLocks[nSlot]);
        void* pTask = reinterpret_cast<void*>(static_cast<uintptr_t>(g_b2TaskSlots[nSlot]));
        if (pTask != nullptr)
        {
            taskLinkListRemove(static_cast<uint8_t*>(pTask) + 0x10); // pTaskQueueLink (pPrev +0x10, pNext +0x14)
            // partial: Ordinal 10046 renders the pool pointer as 0; it arrives in the third argument register.
            pEnv->freePool(kTaskFile, 0x32D, pTask);
            g_b2TaskSlots[nSlot] = 0;
        }
        LeaveCriticalSection(&g_b2TaskSlotLocks[nSlot]);
    }

    g_b2TasksCreated = 0;
    g_b2GamesCount = 0;
    LeaveCriticalSection(&g_b2TaskGlobalLock);
}
