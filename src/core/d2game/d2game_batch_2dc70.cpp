// src/core/d2game/d2game_batch_2dc70.cpp -- ten 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the CCmd client-join and
// client-leave handlers, the client-list lookup/state/ping helpers, the game-name check, and the two
// per-client player/unit-update helpers called from the game frame. Base 0x6FCA0000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets: scalars
// are 32-bit words (rd32/wr32), pointers the decompiles follow (game+0x88 client-list head,
// client+0x4A8 game-list next, client+0x4AC by-id next, client+0x174 player) hold host pointers.
// The client table (0x6FD307B8), its initialised flag (0x6FD31C18), the game GUID table
// (0x6FD30C08) and the two critical sections (0x6FD30BB8, 0x6FD30BE8) are hosted here.
//
// Every unmapped callee is a seam in D2GameBatch7Env, in retail argument order; a Ghidra call that
// passes arguments only in registers gets an explicit parameter, noted at the seam. The fatal path
// Ordinal_10265(code) -> Ordinal_10024(&0x6FD19FF9) -> _exit(-1) is modelled by d2game_fatal(code).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION no-ops, deterministic GetTickCount
#include <D2BasicTypes.h> // BOOL, needed by the compat Server.h below
#include <Server.h>    // Sleep no-op

static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint64_t rd64(const void* p, uint32_t o) { uint64_t v; std::memcpy(&v, (const uint8_t*)p + o, 8); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void wr64(void* p, uint32_t o, uint64_t v) { std::memcpy((uint8_t*)p + o, &v, 8); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2GameBatch7Env {
    // 0x2AAE0 GAME_GetGameByGUID: the decompiles pass the GUID/client key in ESI/ECX.
    void*    (*getGame)(uint32_t key);
    // 0x49FE0 CLIENTS_GetClientFromClientId(game, clientId): game in EAX, id in the argument.
    void*    (*findClientInGame)(void* game, uint32_t clientId);
    // 0x29BA0 GAME_LeaveGamesCriticalSection(game); 0x2C830 client sweep; 0x2D500 send-queue pump
    // (game argument, client in EAX).
    void     (*unlockGame)(void* game);
    void     (*clientSweep)(void* game);
    void     (*pumpClientQueue)(void* client, void* game, uint32_t flag);
    // 0x8A3E0 appends len bytes to the client's SCmd block list (client in EAX).
    void     (*queuePacket)(void* client, const void* data, uint32_t len);
    // 0x2D790 client flush (client in ESI, game in EDI).
    void     (*flushClient)(void* client, void* game);
    // 0x4B7F0 CLIENTS_RemoveClientFromGame(game, clientId, flag).
    void     (*removeClient)(void* game, uint32_t clientId, uint32_t flag);
    // 0xCBDB0 name-termination callback; its buffer arrives in EAX.
    void     (*terminateName)(void* buffer);
    // 0x2BF90 (game in EAX, client in EDI) and 0x4B500 (client, game, two flags).
    void     (*prepareClientState)(void* game, void* client);
    void     (*addClientToGame)(void* client, void* game, uint32_t a3, uint32_t a4);
    // 0x4BCC0 (client in EAX, game, then three flags): NULL or the rejection packet to send.
    void*    (*buildRejectPacket)(void* client, void* game, uint32_t a2, uint32_t a3, uint32_t a4);
    // 0x4AD10 CLIENTS_GetPlayerFromClient(client, ignoreDeath).
    void*    (*getPlayerFromClient)(void* client, int32_t ignoreDeath);
    // 0x4AD80 (client in EBX) and 0x8A2A0 (client in the stack argument).
    void     (*refreshUnitList)(void* client, uint32_t count);
    void     (*sendUnitsUpdate)(void* client);
    // 0xE03A0 SUNIT_GetServerUnit(game in ECX, type in EDX, guid on the stack).
    void*    (*getServerUnit)(void* game, uint32_t unitType, uint32_t unitGuid);
    // 0xCC030 unit update dispatch (unit in EAX).
    int32_t  (*unitUpdateDispatch)(void* unit);
    // 0x8A730 D2GAME_PACKETS_SendPacket_0x8F (client in EAX).
    void     (*sendPacket8F)(void* client);
    // Storm #509 SStrCmp(a, b, n); Storm #501 SStrCopy(dst, src, n).
    int32_t  (*strCmp)(const void* a, const void* b, uint32_t n);
    void     (*strCopy)(void* dst, const void* src, uint32_t n);
    // D2Net #10002 send(channel, clientId, data, len); the other three are D2Net ordinals 10015,
    // 10000 and 10001, whose register-only arguments are lost in the decompiles.
    int32_t  (*send)(uint32_t channel, uint32_t clientId, const void* data, uint32_t len);
    void     (*net10015)(uint32_t clientId);
    void     (*net10000)(void);
    void     (*net10001)(void);
};

// ---- module state, each at its retail address ---------------------------------------------------
static void*    g_clientList[256];        // 0x6FD307B8: by-id client chains, next at client+0x4AC
static uint32_t g_clientListInitialized;  // 0x6FD31C18: gbClientListInitialized
static int32_t  g_gameGuids[1024];        // 0x6FD30C08: game GUID per slot, 0 empty, -1 free/ended
static CRITICAL_SECTION g_clientListLock; // 0x6FD30BB8
static CRITICAL_SECTION g_gameListLock;   // 0x6FD30BE8

// 1.13c D2Game+0x2DC70
// GAME_ClientLeaveGame (CCmd 0x69): resolve the client's game (0x2AAE0), look for the id in the
// game's client list (next at +0x4A8) and bail out through 0x29BA0 when absent; then the client
// record (0x49FE0, code-less fatal when missing) decides the sweep: flag bit 5 at client+0x3D4, or
// a non-zero game state byte +0x6A. Per client the handler queues and sends a one-byte packet twice
// (0x8A3E0), flushes (0x2D790), pumps the send queue (0x2D500), copies 0x32 bytes from client+0x74
// and 0x10 bytes from client+0x34 into stack buffers, removes the client with flag !sweep (0x4B7F0)
// and terminates the name (0xCBDB0). Sweeping repeats over the list from the head; then the game
// lock is released and, for state bytes 1/2, Sleep(100) and D2Net #10000 run.
// partial: the queued/sent byte and the length of the #10002 call come from uninitialised stack
// slots; both are modelled as byte 0 / length 1.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2dc70(uint32_t clientId, const D2GameBatch7Env* env)
{
    env->net10015(clientId);                          // D2Net #10015 (client in EAX)
    void* game = env->getGame(clientId);              // 0x2AAE0
    if (game == nullptr) return;

    void* listed = rdptr(game, 0x88);
    while (listed != nullptr && rd32(listed, 0x00) != clientId) listed = rdptr(listed, 0x4A8);
    if (listed == nullptr) { env->unlockGame(game); return; }   // 0x29BA0

    void* client = env->findClientInGame(game, clientId);       // 0x49FE0
    if (client == nullptr) d2game_fatal(0);                     // code not rendered

    const bool killed = (rd8(client, 0x3D4) & 0x20) != 0;
    const bool sweep = killed || rd8(game, 0x6A) != 0;
    if (sweep) env->clientSweep(game);                          // 0x2C830

    uint32_t id = clientId;
    for (;;) {
        uint8_t packetByte = 0;                    // partial: uninitialised slot in the decompile
        env->queuePacket(client, &packetByte, 1);  // 0x8A3E0
        env->flushClient(client, game);            // 0x2D790
        env->queuePacket(client, &packetByte, 1);  // 0x8A3E0
        env->send(0, id, &packetByte, 1);          // D2Net #10002
        env->pumpClientQueue(client, game, 0);     // 0x2D500

        uint8_t info[0x32];                        // auStack_24 (0x32 bytes)
        std::memset(info, 0, sizeof info);
        env->strCopy(info, (const uint8_t*)client + 0x74, 0x32);          // Storm #501
        wr32(info, 0, rd32(info, 0) & 0xFFFFFFu);                          // auStack_24[0] &= 0xffffff
        uint8_t name[0x10];                        // &stack0xffffffc0
        std::memset(name, 0, sizeof name);
        env->strCopy(name, (const uint8_t*)client + 0x34, 0x10);          // Storm #501

        env->removeClient(game, killed ? 0u : 1u, 0u);   // 0x4B7F0(game, !killed); flag arg unrendered
        if (id != 0) env->terminateName(info);           // 0xCBDB0, buffer in EAX

        if (!killed) break;
        client = rdptr(game, 0x88);
        if (client == nullptr) break;
        id = rd32(client, 0x00);
    }

    if (rdptr(game, 0x18) == nullptr) d2game_fatal(0x100F);
    LeaveCriticalSection((LPCRITICAL_SECTION)rdptr(game, 0x18));
    if (rd8(game, 0x6A) == 1 || rd8(game, 0x6A) == 2) Sleep(100);
    env->net10000();                                   // D2Net #10000
}

// 1.13c D2Game+0x2E360
// GAME_ClientJoinGame (CCmd 0x6B): with the client id in EAX, resolve the game and confirm the id
// is on its client list (else 0x29BA0 and return). 0x4BCC0 either returns a rejection packet (sent
// on channel 0, client removed with flag 1, #10001, game unlocked) or NULL, in which case 0x2BF90
// prepares the client (fatal 0xFB when the client record is missing), 0x4B500 adds the player and
// client state +0x04 becomes 3.
// partial: the rejection send's data/length arrive in registers; only channel 0 is rendered.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2e360(uint32_t clientId, const D2GameBatch7Env* env)
{
    env->net10015(clientId);                          // D2Net #10015
    void* game = env->getGame(clientId);              // 0x2AAE0
    if (game == nullptr) return;

    bool listed = false;
    for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8))
        if (rd32(c, 0x00) == clientId) { listed = true; break; }
    if (!listed) { env->unlockGame(game); return; }

    void* client = env->findClientInGame(game, clientId);           // 0x49FE0
    void* reject = env->buildRejectPacket(client, game, 1, 0, 0);   // 0x4BCC0
    if (reject != nullptr) {
        env->send(0, clientId, reject, 0);             // D2Net #10002(channel 0); length in a register
        env->removeClient(game, 1, 0);                 // 0x4B7F0(game, 1)
        env->net10001();                               // D2Net #10001
        env->unlockGame(game);                         // 0x29BA0
        return;
    }

    env->prepareClientState(game, client);             // 0x2BF90 (game in EAX, client in EDI)
    if (client == nullptr) d2game_fatal(0xFB);
    env->addClientToGame(client, game, 0, 0);          // 0x4B500
    wr32(client, 0x04, 3);                             // dwClientState = 3
    env->unlockGame(game);                             // 0x29BA0
}

// 1.13c D2Game+0x49930
// CLIENTS_CheckState(clientId, expected): false when the by-id table is uninitialised; otherwise
// walk the id's chain (next at +0x4AC) under the client-list lock and compare the record's word +4
// with the expected state. The id arrives in ESI in retail.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_game_49930(uint32_t clientId, uint32_t expectedState, const D2GameBatch7Env*)
{
    if (g_clientListInitialized == 0) return false;
    EnterCriticalSection(&g_clientListLock);
    for (void* c = g_clientList[clientId & 0xFF]; c != nullptr; c = rdptr(c, 0x4AC)) {
        if (rd32(c, 0x00) == clientId) {
            const bool match = rd32(c, 0x04) == expectedState;
            LeaveCriticalSection(&g_clientListLock);
            return match;
        }
    }
    LeaveCriticalSection(&g_clientListLock);
    return false;
}

// 1.13c D2Game+0x4A0A0
// CLIENTS_GetClientFromList: take the client-list lock and return the record with this id from the
// by-id chain (next at +0x4AC). The lock is released only on failure, so a non-NULL result is
// returned with the lock held and the caller must release it (0xCAF30's CCmd 0x70 does).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_game_4a0a0(uint32_t clientId, const D2GameBatch7Env*)
{
    EnterCriticalSection(&g_clientListLock);
    void* c = g_clientList[clientId & 0xFF];
    while (c != nullptr) {
        if (rd32(c, 0x00) == clientId) return c;
        c = rdptr(c, 0x4AC);
    }
    LeaveCriticalSection(&g_clientListLock);
    return nullptr;
}

// 1.13c D2Game+0x4A780
// CLIENTS_UpdatePing(clientId, sentTick, delay): (branches corrected against the decompile) resolve the game and the client in it. With a game
// client and a ping count +0x500 below 10, fold this sample into the 64-bit moving average
// (+0x4F8) as (count*avg + now - sentTick - delay) / (count+1); always send packet 0x8F and stamp
// +0x3D8, then bump the count once more. Without a game client, under the client-list lock the
// sample is recorded at history[count & 0xF] (+0x4B4), the count is bumped and the average of the
// first min(count, 16) samples is stored; 0x8F and the tick stamp follow, then the lock is released.
// partial: the second timestamp (Ghidra's unaff_retaddr) arrives on the stack beside param_1.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_4a780(uint32_t clientId, int32_t sentTick, int32_t delay,
                                        const D2GameBatch7Env* env)
{
    void* game = env->getGame(clientId);               // 0x2AAE0
    if (game != nullptr) {
        bool present = false;
        for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8))
            if (rd32(c, 0x00) == clientId) { present = true; break; }
        if (present) {
            for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8)) {
                if (rd32(c, 0x00) != clientId) continue;
                const DWORD now = GetTickCount();
                uint32_t count = rd32(c, 0x500);
                wr32(c, 0x4B4 + 4 * (count & 0xF), (now - (DWORD)sentTick) - (DWORD)delay);
                count += 1;
                wr32(c, 0x500, count);
                const uint32_t n = (int32_t)count > 0x10 ? 0x10 : count;
                uint64_t sum = 0;
                for (uint32_t i = 0; i < n; ++i) sum += rd32(c, 0x4B4 + 4 * i);
                wr64(c, 0x4F8, sum / n);
                uint8_t pkt[0x21] = { 0x8F };
                env->queuePacket(c, pkt, 0x21);    // 0x8A3E0
                wr32(c, 0x3D8, now);
                break;
            }
            if (rdptr(game, 0x18) == nullptr) d2game_fatal(0x100F);
            LeaveCriticalSection((LPCRITICAL_SECTION)rdptr(game, 0x18));
            return;
        }
        env->unlockGame(game);                     // 0x29BA0
    }

    if (g_clientListInitialized == 0) return;
    EnterCriticalSection(&g_clientListLock);
    for (void* c = g_clientList[clientId & 0xFF]; c != nullptr; c = rdptr(c, 0x4AC)) {
        if (rd32(c, 0x00) != clientId) continue;
        const DWORD now = GetTickCount();
        const uint32_t count = rd32(c, 0x500);
        if ((int32_t)count < 10) {
            const int64_t sum = (int64_t)count * (int64_t)rd64(c, 0x4F8) +
                                (int64_t)(int32_t)((now - (DWORD)sentTick) - (DWORD)delay);
            wr32(c, 0x500, count + 1);
            wr64(c, 0x4F8, (uint64_t)(sum / (int64_t)(count + 1)));
        }
        env->sendPacket8F(c);                      // 0x8A730
        wr32(c, 0x3D8, now);
        wr32(c, 0x500, rd32(c, 0x500) + 1);
        break;
    }
    LeaveCriticalSection(&g_clientListLock);
}

// 1.13c D2Game+0x8C4B0
// CCmd 0x68 reject: send a five-byte packet beginning 0xB4 on channel 0 to the client (EDX).
// partial: only byte 0 of the packet is initialised in the decompile; the rest are zeroed here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_8c4b0(uint32_t clientId, const D2GameBatch7Env* env)
{
    uint8_t packet[5] = { 0xB4, 0, 0, 0, 0 };
    env->send(0, clientId, packet, 5);                 // D2Net #10002
}

// 1.13c D2Game+0xCAD60
// CCmd 0x68 join-game validation: only client id 1 may join (else 6), and the first occupied game
// slot (0x6FD30C08 under the game-list lock) must exist (else 6). The packet word must be 0xD
// (else 0x10) and game word +0x8C at most 7 (else 0xF). The name (EDI) must be 2..15 characters;
// otherwise 6. A valid name is compared with each client's name (+0xD, 0x49A20, Storm #509,
// 16 bytes): a duplicate returns 6, an exhausted client list returns 0. Every exit unlocks the game.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_cad60(uint32_t clientId, const char* name,
                                            uint32_t packetWord, const D2GameBatch7Env* env)
{
    if (clientId != 1) return 6;

    EnterCriticalSection(&g_gameListLock);
    int32_t guid = 0;
    if (g_gameGuids[0] != 0 && g_gameGuids[0] != -1) guid = g_gameGuids[0];
    LeaveCriticalSection(&g_gameListLock);
    if (guid == 0) return 6;

    void* game = env->getGame((uint32_t)guid);         // 0x2AAE0
    if (game == nullptr) return 6;
    if (packetWord != 0xD) { env->unlockGame(game); return 0x10; }
    if ((int32_t)rd32(game, 0x8C) > 7) { env->unlockGame(game); return 0xF; }

    if (name != nullptr && name[0] != '\0' && name[1] != '\0') {
        const char* p = name;
        for (int32_t i = 0; ; ++i) {
            if (i >= 0x10) break;                      // no terminator inside 16 bytes
            if (*p++ == '\0') {
                void* c = rdptr(game, 0x88);
                if (c == nullptr) { env->unlockGame(game); return 0; }
                for (;;) {
                    const void* clientName = (const uint8_t*)c + 0x0D;   // 0x49A20 = client + 0xD
                    if (env->strCmp(clientName, name, 0x10) == 0) {       // Storm #509
                        env->unlockGame(game);
                        return 6;                          // name already in the game
                    }
                    c = rdptr(c, 0x4A8);
                    if (c == nullptr) { env->unlockGame(game); return 0; }
                }
            }
        }
    }
    env->unlockGame(game);
    return 6;
}

// 1.13c D2Game+0x2ABD0
// GAME_IsGameNameUnique(name): for every occupied game slot, resolve the game and compare its
// 16-byte name at +0x2A with the argument (Storm #509). A match releases the lock and returns 0;
// every other game releases the lock after the null-lock fatal check. Returns 1 when no game
// matches.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_2abd0(const void* name, const D2GameBatch7Env* env)
{
    for (int32_t i = 0; i < 1024; ++i) {
        if (g_gameGuids[i] == 0 || g_gameGuids[i] == -1) continue;
        void* game = env->getGame((uint32_t)g_gameGuids[i]);       // 0x2AAE0
        if (game == nullptr) continue;
        if (env->strCmp(name, (const uint8_t*)game + 0x2A, 0x10) == 0) {   // Storm #509
            LeaveCriticalSection((LPCRITICAL_SECTION)rdptr(game, 0x18));
            return 0;
        }
        if (rdptr(game, 0x18) == nullptr) d2game_fatal(0x100F);
        LeaveCriticalSection((LPCRITICAL_SECTION)rdptr(game, 0x18));
    }
    return 1;
}

// 1.13c D2Game+0x2BE20
// CLIENTS_ValidatePlayerUnit (client in ESI): with client flag bit 0 clear, keep +0x174 when it is
// non-zero and look the unit up with 0xE03A0(game +0x1A8, type +0x16C, guid +0x170), clearing
// +0x174 on failure; with the flag set, +0x174 is the unit directly. A unit whose type word is 0
// (player) is passed to 0xCC030, and a non-zero result clears client word +0x1B0; any other unit
// type is the 0xEB0 fatal.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2be20(void* client, const D2GameBatch7Env* env)
{
    void* unit = nullptr;
    if ((rd8(client, 0x3D4) & 1) == 0) {
        if (rd32(client, 0x174) == 0) return;
        unit = env->getServerUnit(rdptr(client, 0x1A8), rd32(client, 0x16C), rd32(client, 0x170));
        if (unit == nullptr) wr32(client, 0x174, 0);
    } else {
        unit = rdptr(client, 0x174);
    }
    if (unit == nullptr) return;

    if (rd32(unit, 0x00) == 0) {                       // dwUnitType = UNIT_PLAYER
        if (env->unitUpdateDispatch(unit) != 0) wr32(client, 0x1B0, 0);
        return;
    }
    d2game_fatal(0xEB0);
}

// 1.13c D2Game+0x2C060
// CLIENTS_SendUnitUpdates (client in EAX): require the outgoing packet list head +0x1B8, a player
// (0x4AD10) and client word +0x1B0 at least 10. A player unit type other than 0 is the 0xEC4
// fatal. The number of list entries is (499 - packetList[0]) / 9; fewer than one returns, 0x38 or
// more is the 0xECE fatal. Otherwise 0x4AD80 refreshes the list (client in EBX) and 0x8A2A0 sends
// the 0x16 units-update packet.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2c060(void* client, const D2GameBatch7Env* env)
{
    const uint32_t head = (uint32_t)(uintptr_t)rdptr(client, 0x1B8);
    if (head == 0) return;
    void* player = env->getPlayerFromClient(client, 0);        // 0x4AD10
    if (player == nullptr) return;
    if ((int32_t)rd32(client, 0x1B0) < 10) return;

    if (rd32(player, 0x00) != 0) d2game_fatal(0xEC4);          // not UNIT_PLAYER
    const uint32_t n = (499u - rd32((const void*)(uintptr_t)head, 0x00)) / 9;
    if ((int32_t)n < 1) return;
    if (n >= 0x38) d2game_fatal(0xECE);

    env->refreshUnitList(client, n);                           // 0x4AD80, client in EBX
    env->sendUnitsUpdate(client);                              // 0x8A2A0
}
