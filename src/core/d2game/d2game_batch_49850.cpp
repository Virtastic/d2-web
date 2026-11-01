// src/core/d2game/d2game_batch_49850.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the GAME/Clients.cpp client
// list and save-header helpers, the CCmd.cpp system/client message dispatchers, two GAME/Game.cpp
// teardown helpers and the TASK.cpp queue increment.
//
// Register-only arguments (Ghidra's in_EAX/unaff_EDI/unaff_ESI, or arguments the ABI passed in a
// register and the callsite dropped) are explicit parameters here and are named in the comment at each
// function. Retail state is modelled as the globals below, each carrying its retail address; records
// are opaque host objects addressed by retail byte offsets. The fog/Storm/D2Net imports are host seams
// on D2GameBatch4Env in retail argument order. The fatal path is Fog Ordinal 10265(id) ->
// Ordinal 10024(text) -> _exit(-1); only the exit is observable and is modelled.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Game/src/GAME/{Clients,CCmd,Game,SCmd,Task}.cpp.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <Windows.h>      // CRITICAL_SECTION no-ops, deterministic GetTickCount
#include <D2BasicTypes.h> // BOOL
#include <Server.h>       // LARGE_INTEGER and the D2Net compat declarations

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement. The 1.13c import
// ordinals are not the 1.10f names: D2Net #10015 here returns the client's game GUID (1.10f #10021,
// SERVER_GetClientGameGUID) and #10001 is the (clientId, file, line) disconnect (1.10f #10015); #10002
// is the (list, clientId, data, size) send (1.10f D2NET_10006). Each seam notes its retail RVA.
struct D2GameBatch4Env {
    uint32_t (*crc16)(const char* s);                                   // 0xA334, Fog #10137
    int32_t  (*strCmpI)(const char* a, const char* b, uint32_t n);      // 0xA280, Storm #509
    uint32_t (*strCopy)(char* dst, const char* src, uint32_t n);        // 0xA280, Storm #501
    void*    (*allocPool)(const char* file, uint32_t line, uint32_t zero); // 0xA2E6, Fog #10045
    int32_t  (*getClientGameGuid)(uint32_t clientId);                   // 0xA364, D2Net #10015
    void     (*disconnectClient)(uint32_t clientId, const char* file, uint32_t line); // 0xA358, D2Net #10001
    int32_t  (*sendToList)(uint32_t list, uint32_t clientId, const void* data, uint32_t len); // 0xA35E, D2Net #10002
    void     (*netPostSend)(void);                                      // 0xA388, D2Net #10003
    void*    (*lockGameByGuid)(int32_t guid);                           // 0x2AAE0, GUID in ESI
    void     (*unlockGame)(void* game);                                 // 0x29BA0
    void*    (*getClientById)(uint32_t clientId);                       // 0x49FE0
    int32_t  (*getPlayerFromClient)(void* client, uint32_t ignoreDeath); // 0x4AD10, client in ESI
    void     (*removeClientFromGame)(void* game, uint32_t clientId, uint32_t triggerSave); // 0x4B7F0, clientId hidden
    void     (*appendPacket)(void* client, const void* data, uint32_t len); // 0x8A3E0, client in ESI
    uint32_t (*checksum)(const void* header, uint32_t size);            // 0x53170
    int32_t  (*getServerUnit)(uint32_t game, uint32_t unitId);          // 0xE03A0
    int32_t  (*findGameIsFree)(const char* name);                       // 0x2ABD0, nonzero = name unused
    int32_t  (*gameCanCreate)(uint32_t clientId);                       // 0x2A850, GAME_VerifyCreateNewGame
    void     (*createEmptyGame)(void);                                  // 0x2C2D0, GAME_CreateNewEmptyGame
    int32_t  (*canClientJoinGame)(uint32_t a);                          // 0xCAD60, CCMD_CanClientJoinGame
    void     (*sendJoinError)(uint32_t ecxError, uint32_t clientId);    // 0x8C4B0, ECX = error
    int32_t  (*verifyJoinGame)(uint32_t clientId, uint16_t gameId, uint8_t classId, uint32_t token,
                               void* accountName, void* a6, uint8_t locale, void* a8, void* a9,
                               void* a10, void** out);                  // 0x2A740, GAME_VerifyJoinGame
    void     (*joinGame)(uint16_t gameId, uint8_t classId, void* nameAndRest, void* accountName); // 0x2BB10
    int32_t  (*clientsCheckState)(uint32_t clientId, uint32_t state);   // 0x49930, CLIENTS_CheckState
    void     (*endGame)(uint32_t clientId);                             // 0x2DC70, GAME_EndGame
    void     (*joinList)(uint32_t clientId);                            // 0x2BA10
    void     (*sendActInit)(uint32_t clientId);                         // 0x2E360, GAME_SendActInit
    void     (*openChar)(uint32_t clientId);                            // 0x2AAC0, GAME_VerifyOpenChar
    void     (*updatePing)(uint32_t clientId, uint32_t word);           // 0x4A780, CLIENTS_UpdatePing
    int32_t  (*verifyDisconnect)(uint32_t clientId);                    // 0x2A6E0, GAME_VerifyDisconnect
    void     (*disconnectById)(uint32_t clientId, uint32_t eventType);// 0x2D7E0, GAME_DisconnectClientById
    void*    (*getCommandClient)(void);                                 // 0x4A0A0
    void     (*packetHandler)(void* game, void* player);                // 0xCC550, packet in EBX
    int32_t  (*saveHeaderPart)(void* game, void* client);               // 0x2D140, CLIENT_SendSaveHeaderPart
    void     (*updateClientPre)(void* client);                          // 0x2C060
    void     (*updateClientMid)(void* client);                          // 0x2BE20
    void     (*disconnectClientObj)(void* game, void* client);          // 0x2D090
};

// ---- module state --------------------------------------------------------------------------------
static void* g_clientList[256];        // 0x6FD307B8 (1.10f gpClientList_6FD43FB8)
static void* g_clientListByName[256];  // 0x6FD303B8 (1.10f gpClientListByName_6FD443D0)
static CRITICAL_SECTION g_clientListLock;      // 0x6FD30BB8
static CRITICAL_SECTION g_clientByNameLock;    // 0x6FD30BD0
static CRITICAL_SECTION g_gameGuidLock;        // 0x6FD30BE8
static int32_t  g_clientListInitialized;       // 0x6FD31C18 (1.10f gbClientListInitialized_6FD447E8)
static int32_t  g_gameGuidSlots[1024];         // 0x6FD30C04 (1.10f gnGamesGUIDs_6FD447F8), -1 free
static uint32_t g_updateFlag;                  // 0x6FD31C38

// Client record offsets (1.13c): +0x00 id, +0x04 state, +0x0D name[16], +0x170 unit id,
// +0x174 pPlayer, +0x178 bUnlockCharacter, +0x17C pSaveHeader, +0x180 nSaveHeaderSize, +0x18C checksum,
// +0x3D4 flags, +0x3D8 last packet tick, +0x4A8 next, +0x4AC next-by-id, +0x4B0 next-by-name,
// +0x1B8/+0x1BC/+0x1C0 packet queue head/tail/free, +0x1C4 count, +0x204 packet node next.
static const char kClientsFile[] = "..\\Source\\D2Game\\GAME\\Clients.cpp";
static const char kCCmdFile[] = "..\\Source\\D2Game\\GAME\\CCmd.cpp";

// 1.13c D2Game+0x49850
// CLIENTS_GetClientIdByName (1.10f 0x6FC33F20): bucket the name with Fog #10137's CRC16, walk the
// by-name client list (next at client+0x4B0) under 0x6FD30BD0, SStrCmpI against the 16-byte name at
// client+0x0D, return the first match's client id (0 when absent). The name arrives in a register.
uint32_t CLIENTS_GetClientIdByName(const char* szName, const D2GameBatch4Env* env)
{
    uint32_t clientId = 0;
    const uint8_t bucket = (uint8_t)env->crc16(szName);   // Ordinal 10137
    EnterCriticalSection(&g_clientByNameLock);            // Ordinal 10050
    void* client = g_clientListByName[bucket];
    while (client != nullptr && clientId == 0) {
        if (env->strCmpI((const char*)client + 0x0D, szName, 0x10) == 0)   // Ordinal 509
            clientId = rd32(client, 0);
        client = rdptr(client, 0x4B0);
    }
    LeaveCriticalSection(&g_clientByNameLock);
    return clientId;
}

// 1.13c D2Game+0x498B0
// CLIENTS_GetNameIfInGame (1.10f sub_6FC33EA0, Clients.cpp:1391): an uninitialized list answers 1.
// Otherwise take the id list lock, walk bucket id&0xFF through client+0x4AC while the result is still 1,
// and on the first record whose +0x00 id equals the client id copy its 16-byte name at +0x0D through
// Storm #501 when a destination was passed (in EBX, explicit here), set the result 0, and unlock;
// no match leaves 1.
int32_t CLIENTS_GetNameIfInGame(uint32_t clientId, char* szName, const D2GameBatch4Env* env)
{
    if (g_clientListInitialized == 0) return 1;
    int32_t result = 1;
    EnterCriticalSection(&g_clientListLock);              // Ordinal 10050
    void* client = g_clientList[clientId & 0xFF];
    while (client != nullptr && result != 0) {
        if (rd32(client, 0) == clientId) {
            if (szName != nullptr) env->strCopy(szName, (const char*)client + 0x0D, 0x10);   // Ordinal 501
            result = 0;
        }
        client = rdptr(client, 0x4AC);
    }
    LeaveCriticalSection(&g_clientListLock);
    return result;
}

// 1.13c D2Game+0x4A110
// CLIENTS_Initialize (1.10f 0x6FC31CD0): create both list locks, zero the 256-entry id list under the
// first and the 256-entry by-name list under the second, then set the initialized flag.
void CLIENTS_Initialize(void)
{
    InitializeCriticalSection(&g_clientListLock);
    EnterCriticalSection(&g_clientListLock);              // Ordinal 10050
    std::memset(g_clientList, 0, sizeof g_clientList);
    LeaveCriticalSection(&g_clientListLock);
    InitializeCriticalSection(&g_clientByNameLock);
    EnterCriticalSection(&g_clientByNameLock);            // Ordinal 10050
    std::memset(g_clientListByName, 0, sizeof g_clientListByName);
    LeaveCriticalSection(&g_clientByNameLock);
    g_clientListInitialized = 1;
}

// 1.13c D2Game+0x4A190
// CLIENTS_AttachSaveFile (1.10f 0x6FC34020): the client id arrives in EAX (explicit here). With the
// list initialized, find the client in bucket id&0xFF (next at client+0x4AC); on the first chunk
// allocate the header through Fog #10045 named Clients.cpp:0xB16, else append behind it; fail
// (0xB1B) when nTotalSize < nSaveHeaderSize + nSize; copy, grow the size, store bUnlockCharacter at
// +0x178, and when the total is reached set flag 8, store the 0x53170 checksum at +0x18C and return 1.
// Delta vs 1.10f: the "[LOAD] CKSUM" GAME_LogMessage is gone.
uint32_t CLIENTS_AttachSaveFile(uint32_t clientId, const void* pSaveData, uint32_t nSize,
                                uint32_t nTotalSize, uint32_t bUnlockCharacter, const D2GameBatch4Env* env)
{
    if (g_clientListInitialized == 0) return 0;
    EnterCriticalSection(&g_clientListLock);              // Ordinal 10050
    void* client = g_clientList[clientId & 0xFF];
    while (client != nullptr && rd32(client, 0) != clientId) client = rdptr(client, 0x4AC);
    uint32_t result = 0;
    if (client != nullptr) {
        const uint32_t have = rd32(client, 0x180);
        uint8_t* dst;
        if (have != 0) {
            dst = (uint8_t*)rdptr(client, 0x17C) + have;
        } else {
            void* header = env->allocPool(kClientsFile, 0xB16, 0);   // Ordinal 10045
            wrptr(client, 0x17C, header);
            dst = (uint8_t*)header;
        }
        const uint32_t total = have + nSize;
        if ((int32_t)nTotalSize < (int32_t)total) d2game_fatal(0xB1B);
        std::memcpy(dst, pSaveData, nSize);
        wr32(client, 0x180, total);
        wr32(client, 0x178, bUnlockCharacter);
        if (total == nTotalSize) {
            wr32(client, 0x3D4, rd32(client, 0x3D4) | 8);
            wr32(client, 0x18C, env->checksum(rdptr(client, 0x17C), total)); // 0x53170
            result = 1;
        }
    }
    LeaveCriticalSection(&g_clientListLock);
    return result;
}

// 1.13c D2Game+0x8C490
// Send a one-byte 0xFF header-only packet to the client in ESI (explicit here) via 0x8A3E0.
void D2GAME_SendHeaderOnlyFF(uint32_t client, const D2GameBatch4Env* env)
{
    const uint8_t b = 0xFF;
    env->appendPacket((void*)(uintptr_t)client, &b, 1);   // 0x8A3E0
}

// 1.13c D2Game+0x2A2D0
// Claim a game-GUID slot: under 0x6FD30BE8, if slot param_2 is -1 store param_1 and release; a second
// claim of the same slot is the 0x185 fatal.
void GAME_SetGameGuidSlot(uint32_t guid, uint16_t slot)
{
    EnterCriticalSection(&g_gameGuidLock);               // Ordinal 10050
    if (g_gameGuidSlots[slot] == -1) {
        g_gameGuidSlots[slot] = (int32_t)guid;
        LeaveCriticalSection(&g_gameGuidLock);
        return;
    }
    d2game_fatal(0x185);
}

// 1.13c D2Game+0x2BC90
// GAME_ResolveGameNameConflict (1.10f 0x6FC35CB0): the game arrives in EAX and the candidate name in
// EDI (both explicit here). A name 0x2ABD0 reports free (nonzero) is copied to game+0x2A through
// Storm #501 with a 0x7FFFFFFF bound; otherwise truncate at 11 characters when the 16-byte field
// cannot hold the "~NN" suffix, then try name~0, name~1, ... until one is free (having passed 99 is
// the 0x25C fatal) and copy that.
void GAME_ResolveGameNameConflict(void* game, char* szGameName, const D2GameBatch4Env* env)
{
    if (env->findGameIsFree(szGameName) != 0) {           // 0x2ABD0
        env->strCopy((char*)game + 0x2A, szGameName, 0x7FFFFFFF);   // Ordinal 501
        return;
    }
    if (0xFU - (uint32_t)std::strlen(szGameName) < 3) szGameName[0xB] = '\0';
    char candidate[16];
    std::snprintf(candidate, sizeof candidate, "%s~%d", szGameName, 0);
    int32_t n = 1;
    int32_t freeName = env->findGameIsFree(candidate);
    if (freeName == 0) {
        do {
            if (n > 99) d2game_fatal(0x25C);
            std::snprintf(candidate, sizeof candidate, "%s~%d", szGameName, n);
            ++n;
            freeName = env->findGameIsFree(candidate);
        } while (freeName == 0);
        if (n > 99) d2game_fatal(0x25C);
    }
    env->strCopy((char*)game + 0x2A, candidate, 0x7FFFFFFF);   // Ordinal 501
}

// 1.13c D2Game+0x2BED0
// GAME_SendPacket0x5CToAllConnectedClients (1.10f 0x6FC36AA0): the client arrives in EAX (explicit).
// Resolve the player (alive flag bit 0 of +0x3D4 set, or 0xE03A0 on +0x1A8/+0x170, clearing +0x174
// when the unit is gone), then to every client of game+0x88 whose state word +4 is 4 append the 5
// bytes {0x5C, player->dwUnitId} through 0x8A3E0.
void GAME_SendPacket0x5CToAllConnectedClients(void* game, uint32_t client, const D2GameBatch4Env* env)
{
    if (client == 0) d2game_fatal(0xFB);
    uint32_t player;
    if ((rd8((void*)(uintptr_t)client, 0x3D4) & 1) == 0) {
        player = rd32((void*)(uintptr_t)client, 0x174);
        if (player == 0) return;
        const int32_t unit = env->getServerUnit(rd32((void*)(uintptr_t)client, 0x1A8),
                                                rd32((void*)(uintptr_t)client, 0x170));   // 0xE03A0
        if (unit == 0) wr32((void*)(uintptr_t)client, 0x174, 0);
        else player = (uint32_t)unit;
    } else {
        player = rd32((void*)(uintptr_t)client, 0x174);
    }
    if (player == 0) return;
    uint8_t packet[5] = { 0x5C, 0, 0, 0, 0 };
    std::memcpy(packet + 1, (const uint8_t*)(uintptr_t)player + 0xC, 4);
    for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8)) {
        if (rd32(c, 4) == 4) env->appendPacket(c, packet, 5);   // 0x8A3E0
    }
}

// 1.13c D2Game+0x2D2F0
// Kick a client (1.10f sub_6FC36B20): Ordinal 10015 (D2Net #10015, SERVER_GetClientGameGUID) leaves the
// game GUID in EAX/ESI; lock that game (0x2AAE0). When the client is in the game's list (game+0x88,
// next client+0x4A8), 0x4B7F0(game, clientId, 1) removes it. The game lock is then released; a null
// lock is the 0x100F fatal. Finally Ordinal 10001 (D2Net #10001) disconnects (clientId, file, line).
// partial: the decompile renders the first call's result dropped and gives the function two parameters;
// the file argument is the caller's string pointer.
void GAME_KickClientFromGame(uint32_t clientId, const char* szFile, uint32_t nLine, const D2GameBatch4Env* env)
{
    const int32_t guid = env->getClientGameGuid(clientId);   // Ordinal 10015
    void* game = env->lockGameByGuid(guid);                  // 0x2AAE0
    if (game != nullptr) {
        bool present = false;
        for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8))
            if (rd32(c, 0) == clientId) { present = true; break; }
        if (!present) {
            env->unlockGame(game);                           // 0x29BA0
            env->disconnectClient(clientId, szFile, nLine);  // Ordinal 10001
            return;
        }
        env->removeClientFromGame(game, clientId, 1);        // 0x4B7F0
        if (rdptr(game, 0x18) == nullptr) d2game_fatal(0x100F);
        LeaveCriticalSection((LPCRITICAL_SECTION)rdptr(game, 0x18));   // game+0x18 = held lock
    }
    env->disconnectClient(clientId, szFile, nLine);          // Ordinal 10001
}

// 1.13c D2Game+0xCAE80
// CCMD_ProcessClientMessage (1.10f 0x6FC31C00): the message record arrives in EAX (explicit); its first
// word is the client id. Ordinal 10015 yields the game GUID, 0x2AAE0 locks the game (null -> return).
// The client must be in the list (game+0x88, next +0x4A8) or the game is unlocked and the function
// returns; 0x49FE0 fetches the client (null is the 0x1F3 fatal). Stamp GetTickCount at client+0x3D8,
// take the player through 0x4AD10(client, 0) and, when it is a player unit (type word 0), run the
// packet gate 0xCC550 (packet in EBX, unit in ESI, client on the stack; modelled as two arguments).
// Unlock 0x29BA0. The packet size is not read.
void CCMD_ProcessClientMessage(const uint32_t* msg, const D2GameBatch4Env* env)
{
    const uint32_t clientId = msg[0];
    const int32_t guid = env->getClientGameGuid(clientId);   // Ordinal 10015
    void* game = env->lockGameByGuid(guid);                  // 0x2AAE0
    if (game == nullptr) return;
    bool present = false;
    for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8))
        if (rd32(c, 0) == clientId) { present = true; break; }
    if (!present) {
        env->unlockGame(game);                               // 0x29BA0
        return;
    }
    void* client = env->getClientById(clientId);             // 0x49FE0
    if (client == nullptr) d2game_fatal(0x1F3);
    wr32(client, 0x3D8, (uint32_t)GetTickCount());
    void* player = (void*)(uintptr_t)env->getPlayerFromClient(client, 0);   // 0x4AD10
    if (player != nullptr && rd32(player, 0) == 0) env->packetHandler(game, player);   // 0xCC550
    env->unlockGame(game);                                   // 0x29BA0
}

// 1.13c D2Game+0xCAF30
// CCMD_ProcessClientSystemMessage (1.10f 0x6FC31C04): the message record is param_1; word 0 is the
// client id, byte 4 the system command. The 1.13c opcodes are the 1.10f ones shifted one up (0x67..0x70
// for NEWGAME..), and 0x70 is a new one. Case bodies follow the decompile; where the retail ABI passed
// arguments in registers the seam takes the packet record so it can read them.
void CCMD_ProcessClientSystemMessage(const uint32_t* msg, const D2GameBatch4Env* env)
{
    const uint32_t clientId = msg[0];
    switch ((uint8_t)rd32(msg, 4)) {
    case 0x67:   // NEWGAME (1.10f 0x66)
        if (env->gameCanCreate(clientId) != 0) env->createEmptyGame();   // 0x2A850, 0x2C2D0
        return;
    case 0x68: { // JOINGAME (1.10f 0x67)
        if (g_updateFlag == 0 && env->canClientJoinGame(rd32(msg, 0xC)) != 0) {   // 0xCAD60
            env->sendJoinError(rd32(msg, 0xC), clientId);                        // 0x8C4B0
            env->disconnectClient(clientId, kCCmdFile, 0x148);                   // Ordinal 10001
        }
        uint8_t account[52] = {0}, a[4] = {0}, b[4] = {0}, c[4] = {0}, d[4] = {0};
        uint32_t e = 0;
        void* out = nullptr;
        if (env->verifyJoinGame(clientId, rd16(msg, 9), rd8(msg, 0xB), rd32(msg, 5),
                                account, a, rd8(msg, 0x18), b, c, d, &out) != 0) { // 0x2A740
            env->joinGame(rd16(msg, 9), rd8(msg, 0xB), (void*)((const uint8_t*)msg + 0x19), account); // 0x2BB10
        }
        return;
    }
    case 0x69:   // ENDGAME
        if (env->clientsCheckState(clientId, 4) != 0) env->endGame(clientId);    // 0x49930, 0x2DC70
        return;
    case 0x6A:   // JOINLIST
        env->joinList(clientId);                                                 // 0x2BA10
        return;
    case 0x6B:   // JOINACT
        if (env->clientsCheckState(clientId, 1) != 0) env->sendActInit(clientId); // 0x49930, 0x2E360
        return;
    case 0x6C:   // OPENCHAR
        if (rd32(msg, 6) < 0x2000) env->openChar(clientId);                      // 0x2AAC0
        else d2game_fatal(0x1A5);
        return;
    case 0x6D:   // 1.10f 0x6C, ping
        env->updatePing(clientId, rd32(msg, 5));                                 // 0x4A780
        return;
    case 0x6E:   // DISCONNECT
        if (env->verifyDisconnect(clientId) != 0) env->disconnectById(clientId, 1); // 0x2A6E0, 0x2D7E0
        return;
    case 0x6F:   // REMOVEFROMGAME: no body in the 1.13c decompile
        return;
    case 0x70: { // new in 1.13c: flag the client at word 0x141 and release the list lock
        void* client = env->getCommandClient();                                  // 0x4A0A0
        if (client != nullptr) {
            wr32(client, 0x141 * 4, 1);
            LeaveCriticalSection(&g_clientListLock);
        }
        return;
    }
    default:
        return;
    }
}

// 1.13c D2Game+0x2D500
// Client packet flush (1.10f sub_6FC39030, the caller passes the "1"): pGame is param_1, the client
// arrives in EAX (explicit), a3 is param_2. With a null game the 0xEE2 fatal; with a non-null client:
// when a3 is set and the client state word +4 is 4, run 0x2C060 (under the 0x6FD31C38 flag) and
// 0x2BE20; with game type byte +0x6A 1 or 2 run 0x2D140. Then drain the queue at client+0x1B8
// (next node link at +0x204, data at +4, size at +0):
//  - empty (head == tail) resets both and returns; a zero-size node returns;
//  - 0xA35E(1, clientId, data, size) sends; zero is success: 0xA388, ++count(+0x1C4), clear the node
//    link and push the node back onto the +0x1B8 list; at more than 2 sent, 0x2D090(game) and return;
//  - nonzero: zero the size, push the node onto the free list +0x1C0, ++failures, reset the count;
//    with game type 1/2 and more than 2 failures, return. A success ends the pass.
// partial: the decompile's head/tail/free-list link arithmetic is transcribed literally; the
// 1.10f list helpers (PopHead/Reset) are the prior art for the same fields.
void GAME_FlushClientPackets(void* game, uint32_t client, uint32_t a3, const D2GameBatch4Env* env)
{
    if (game == nullptr) d2game_fatal(0xEE2);
    if (client == 0) d2game_fatal(0xEE3);
    if (a3 != 0 && rd32((void*)(uintptr_t)client, 4) == 4) {
        if (g_updateFlag != 0) env->updateClientPre((void*)(uintptr_t)client);   // 0x2C060
        env->updateClientMid((void*)(uintptr_t)client);                          // 0x2BE20
    }
    if (rd8(game, 0x6A) == 1 || rd8(game, 0x6A) == 2)
        env->saveHeaderPart(game, (void*)(uintptr_t)client);                     // 0x2D140
    bool sent = false;
    uint32_t failures = 0;
    for (;;) {
        uint32_t node = rd32((void*)(uintptr_t)client, 0x1B8);
        if (rd32((void*)(uintptr_t)client, 0x1BC) == node) {
            wr32((void*)(uintptr_t)client, 0x1BC, 0);
            wr32((void*)(uintptr_t)client, 0x1B8, 0);
        }
        if (node == 0) return;
        wr32((void*)(uintptr_t)client, 0x1B8, rd32((void*)(uintptr_t)node, 0x204));
        const uint32_t len = rd32((void*)(uintptr_t)node, 0);
        if (len == 0) return;
        const int32_t rc = env->sendToList(1, client, (const void*)(uintptr_t)(node + 4), len); // 0xA35E
        if (rc == 0) {
            env->netPostSend();                                                  // 0xA388
            const uint32_t count = rd32((void*)(uintptr_t)client, 0x1C4) + 1;
            wr32((void*)(uintptr_t)client, 0x1C4, count);
            wr32((void*)(uintptr_t)node, 0x204, 0);
            sent = true;
            if (rd32((void*)(uintptr_t)client, 0x1B8) != 0)
                wr32((void*)(uintptr_t)client, 0x1BC, node);
            wr32((void*)(uintptr_t)client, 0x1B8, node);
            if (count > 2) {
                env->disconnectClientObj(game, (void*)(uintptr_t)client);        // 0x2D090
                return;
            }
        } else {
            wr32((void*)(uintptr_t)node, 0, 0);
            wr32((void*)(uintptr_t)node, 0x204, rd32((void*)(uintptr_t)client, 0x1C0));
            ++failures;
            wr32((void*)(uintptr_t)client, 0x1C0, node);
            wr32((void*)(uintptr_t)client, 0x1C4, 0);
        }
        if ((rd8(game, 0x6A) == 1 || rd8(game, 0x6A) == 2) && failures > 2) return;
        if (sent) return;
    }
}

// 1.13c D2Game+0x52160
// TASK_QueueIncrement: param_2 is the record, param_3 the queue; the amount arrives in EAX (explicit).
// Add the amount to the record's first word, then walk the list at queue+8 until the node whose key
// (node-0xC) is not greater than the new value; link record+0xC (with the caller's back pointer in
// record+0x10) in front of it. A missing node is the 0x6C fatal, a null link the 0x6D fatal.
void TASK_QueueIncrement(uint32_t delta, uint32_t* rec, void* queue)
{
    const uint32_t value = rec[0] + delta;
    rec[0] = value;
    void* node = rdptr(queue, 8);
    while (node != (void*)((uint8_t*)queue + 8) && !(rd32((uint8_t*)node - 0xC, 0) != value &&
                                                      (int32_t)(rd32((uint8_t*)node - 0xC, 0) - value) > 0))
        node = rdptr(node, 0);
    if (node == nullptr) d2game_fatal(0x6C);
    uint32_t* link = rec + 3;
    *link = (uint32_t)(uintptr_t)node;
    uint32_t* next = (uint32_t*)rdptr(node, 4);
    rec[4] = (uint32_t)(uintptr_t)next;
    *next = (uint32_t)(uintptr_t)link;
    wrptr(node, 4, link);
}
