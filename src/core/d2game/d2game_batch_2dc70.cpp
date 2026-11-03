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
// Retail records are 32-bit images (game +0x88 head, client +0x4A8 next, ...): a link is a 32-bit word whose value is the host address of the linked record (wasm memory is below 4 GB).
static inline void* rdptr(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return (void*)(uintptr_t)v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

#include "d2game_batch7_env.h"

// ---- module state, each at its retail address ---------------------------------------------------
static void*    g_clientList[256];        // 0x6FD307B8: by-id client chains, next at client+0x4AC
static uint32_t g_clientListInitialized;  // 0x6FD31C18: gbClientListInitialized
static int32_t  g_gameGuids[1024];        // 0x6FD30C08: game GUID per slot, 0 empty, -1 free/ended
static CRITICAL_SECTION g_clientListLock; // 0x6FD30BB8
static CRITICAL_SECTION g_gameListLock;   // 0x6FD30BE8

// 1.13c D2Game+0x2DC70
// GAME_ClientLeaveGame (CCmd 0x69, stdcall(clientId, announce)): D2Net #10015 turns the id into the game GUID, 0x2AAE0 resolves the game, and the
// id is looked up in the game's client list (next at +0x4A8; absent: 0x29BA0 and out). The client record (0x49FE0; missing is fatal) decides the
// sweep: flag bit 5 at client+0x3D4 (killed), or a non-zero game state byte +0x6A (0x2C830). Per client: queue byte 5 (0x8A3E0), flush (0x2D790),
// queue byte 6, send byte 0xB0 (D2Net #10002, channel 0), pump (0x2D500), build the 0x5A leave packet on the stack (type 3, 4; the 16-byte name
// from client+0xD at +8, the 0x32-byte string from client+0x1D at +0x18, both SStrCopy), remove the client (0x4B7F0 with !killed) and, when
// `announce` is set, broadcast the packet (0xCBDB0). While killed it repeats from the list head. Then the game lock (+0x18; null is fatal 0x100F)
// is left, state bytes 1/2 sleep 100 ms, and D2Net #10000 closes with the last id.
static void sstrcpy(uint8_t* dst, const uint8_t* src, uint32_t n) {   // Storm SStrCopy: up to n-1 bytes of src, always NUL-terminated
    uint32_t i = 0; while (i + 1 < n && src[i]) { dst[i] = src[i]; ++i; } dst[i] = 0;
}
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2dc70(uint32_t clientId, uint32_t announce, const D2GameBatch7Env* env)
{
    const uint32_t guid = env->net10015(clientId);    // D2Net #10015
    void* game = env->getGame(guid);                  // 0x2AAE0
    if (game == nullptr) return;

    void* listed = rdptr(game, 0x88);
    while (listed != nullptr && rd32(listed, 0x00) != clientId) listed = rdptr(listed, 0x4A8);
    if (listed == nullptr) { env->unlockGame(game); return; }   // 0x29BA0

    void* client = env->findClientInGame(game, clientId);       // 0x49FE0
    if (client == nullptr) d2game_fatal(0xBA6);

    const bool killed = (rd8(client, 0x3D4) & 0x20) != 0;
    const bool sweep = killed || rd8(game, 0x6A) != 0;
    if (sweep) env->clientSweep(game);                          // 0x2C830

    uint32_t id = clientId;
    for (;;) {
        const uint8_t b5 = 5, b6 = 6, bB0 = 0xB0;
        env->queuePacket(client, &b5, 1);          // 0x8A3E0
        env->flushClient(client, game);            // 0x2D790
        env->queuePacket(client, &b6, 1);          // 0x8A3E0
        env->send(0, id, &bB0, 1);                 // D2Net #10002
        env->pumpClientQueue(client, game, 0);     // 0x2D500

        uint8_t pkt[0x4C];                         // the stack packet at B+0x10
        std::memset(pkt, 0, sizeof pkt);
        pkt[0] = 0x5A; pkt[1] = 3; pkt[2] = 4;
        sstrcpy(pkt + 0x18, (const uint8_t*)client + 0x1D, 0x32);
        sstrcpy(pkt + 0x08, (const uint8_t*)client + 0x0D, 0x10);

        env->removeClient(game, id, killed ? 0u : 1u);   // 0x4B7F0(EAX = id, game, !killed)
        if (announce != 0) env->terminateName(pkt, game);   // 0xCBDB0

        if (!killed) break;
        client = rdptr(game, 0x88);
        if (client == nullptr) break;
        id = rd32(client, 0x00);
    }

    if (rdptr(game, 0x18) == nullptr) d2game_fatal(0x100F);
    LeaveCriticalSection((LPCRITICAL_SECTION)rdptr(game, 0x18));
    if (rd8(game, 0x6A) == 1 || rd8(game, 0x6A) == 2) Sleep(100);
    env->net10000(id);                                 // D2Net #10000
}

// 1.13c D2Game+0x2E360
// GAME_ClientJoinGame (CCmd 0x6B): with the client id in EAX, #10015 and 0x2AAE0 resolve the game and the id must be on its client list (else 0x29BA0
// and return). 0x4BCC0 (client in EAX; game, 1, 0, 0, 0 on the stack) returns 0 or a rejection code: a code sends {0xB4, code LE} on channel 0,
// removes the client with flag 1 (0x4B7F0, EAX = id), calls #10001(id, file string, 0x633) and unlocks. Otherwise 0x2BF90 prepares the client (EAX
// game, EDI client; a missing record is fatal 0xFB), 0x4B500 (EAX 0; client, game, 0, 0) adds the player, client state +4 becomes 3, and the game is unlocked.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2e360(uint32_t clientId, const D2GameBatch7Env* env)
{
    const uint32_t guid = env->net10015(clientId);    // D2Net #10015
    void* game = env->getGame(guid);                  // 0x2AAE0
    if (game == nullptr) return;

    bool listed = false;
    for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8))
        if (rd32(c, 0x00) == clientId) { listed = true; break; }
    if (!listed) { env->unlockGame(game); return; }

    void* client = env->findClientInGame(game, clientId);           // 0x49FE0
    const uint32_t code = env->buildRejectPacket(client, game, 1, 0, 0);   // 0x4BCC0
    if (code != 0) {
        uint8_t pkt[5] = { 0xB4, (uint8_t)code, (uint8_t)(code >> 8), (uint8_t)(code >> 16), (uint8_t)(code >> 24) };
        env->send(0, clientId, pkt, 5);                // D2Net #10002
        env->removeClient(game, clientId, 1);          // 0x4B7F0
        env->net10001(clientId, 0x6FD1CD98u, 0x633u);  // D2Net #10001
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
    void* game = env->getGame(env->net10015(clientId));   // 1.13c D2Game+0x4A780: D2Net #10015 (0xA364) turns the id into the GUID first
    if (game != nullptr) {
        bool present = false;
        for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8))
            if (rd32(c, 0x00) == clientId) { present = true; break; }
        if (present) {
            for (void* c = rdptr(game, 0x88); c != nullptr; c = rdptr(c, 0x4A8)) {
                if (rd32(c, 0x00) != clientId) continue;
                const DWORD now = env->tick ? env->tick() : GetTickCount();
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
    if (env->enterClientList) env->enterClientList();   // thunk 0xA328 (ECX = lock, EDX = line 0xA84)
    EnterCriticalSection(&g_clientListLock);
    for (void* c = g_clientList[clientId & 0xFF]; c != nullptr; c = rdptr(c, 0x4AC)) {
        if (rd32(c, 0x00) != clientId) continue;
        const DWORD now = env->tick ? env->tick() : GetTickCount();
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

// FN-probe accessor: the module's client table and its init flag (0x6FD307B8 / 0x6FD31C18).
extern "C" void d2_game_4a780_state(void*** list, uint32_t** init) { *list = g_clientList; *init = &g_clientListInitialized; }

// CCmd 0x68 reject: the five-byte 0xB4 packet lives in d2game_8c4b0.cpp (retail passes the payload dword in EAX; this caller leaves it 0).
void D2Game_8C4B0_PacketSendB4(void (*)(uint32_t, uint32_t, const void*, uint32_t), uint32_t, uint32_t);
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_8c4b0(uint32_t clientId, const D2GameBatch7Env* env)
{
    static const D2GameBatch7Env* s_env;
    s_env = env;
    D2Game_8C4B0_PacketSendB4([](uint32_t z, uint32_t id, const void* p, uint32_t n) { s_env->send(z, id, p, n); }, 0, clientId);   // D2Net #10002
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
