// src/core/d2game/d2game_batch_2cfd0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). These are the GAME/ client
// teardown, game-loop and TASK.cpp queue helpers whose decompiles name no implementation in src/.
//
// The retail module state is modelled as the globals below (each carrying its retail address). A game
// or client record is an opaque host object read by retail byte offsets: scalar fields are 32-bit words
// (rd32/wr32), the few pointer slots the decompiles follow (+0x18 lock, game+0x88 client head,
// client+0x4A8 next) hold host pointers (rdptr/wrptr). The layouts are therefore offset-faithful, not
// byte-identical to the 32-bit retail image.
//
// Every unmapped callee is a host seam in D2GameBatchEnv, in retail argument order (the
// d2client/d2sound seam pattern); a Ghidra call that passes arguments only in registers gets an
// explicit parameter here, noted at the seam. The fatal path is Ordinal_10265(message id) ->
// Ordinal_10024(&0x6FD19FF9, text) -> _exit(-1); only the exit is observable and is modelled.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <Windows.h>   // CRITICAL_SECTION no-ops, IsBadCodePtr, deterministic GetTickCount
#include <D2BasicTypes.h> // BOOL, needed by the compat Server.h below
#include <Server.h>    // LARGE_INTEGER, QueryPerformanceCounter/Frequency no-ops

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement. An argument the
// retail ABI passed in a register (ESI/EAX/ECX) is an explicit parameter.
struct D2GameBatchEnv {
    void*    (*getGame)(void);                              // 0x2AAE0, ESI = game GUID / current client
    void*    (*getGameByGuid)(int32_t guid);                // 0x2AAE0, explicit GUID form
    void*    (*getCurrentGameForClient)(uint32_t clientId);  // 0x2AB70, ESI = client id
    void*    (*getClientById)(uint32_t clientId);           // 0x49FE0
    int32_t  (*gameQuery)(void* game);                      // 0x2A680, ESI = game; one of five counters
    int32_t  (*strCopy)(void* dst, const void* src, uint32_t n); // Storm #501 (SStrCopy)
    void     (*send)(uint32_t channel, uint32_t clientId, const void* data, uint32_t len); // D2Net #10002
    int32_t  (*terminateName)(const void* packet, void* game);   // 0xCBDB0
    void     (*logMessage)(uint32_t code, const char* text);     // 0xCBE10, ECX = code
    void     (*removeClient)(void* game, uint32_t clientId, uint32_t flag); // 0x4B7F0
    void     (*unlockGame)(void* game);                     // 0x29BA0
    void     (*clearTasks)(void);                           // 0x52090
    void     (*gameSweep)(void* game);                      // 0x54320
    void     (*clientFlush)(void* client);                  // 0x2D790
    void     (*closeGame)(void* game);                      // 0x2CE20
    int32_t  (*getServerUnit)(uint32_t a, uint32_t b, uint32_t classEax); // 0xE03A0, EAX = type class
    void     (*fillUnitInfo)(uint32_t esi, uint32_t edi);   // 0xDFB30, ESI/EDI = unit pointers
    void     (*frameAdvance)(void* game);                   // 0x2D980, EDI = game
    uint64_t (*taskClock)(void);                            // 0x52020, ms since boot
    void*    (*allocPool)(const char* file, uint32_t line, uint32_t zero); // Fog #10045
    void     (*taskEnqueue)(void* slot);                    // 0x524C0
    int32_t  (*slotIsBad)(uint32_t index);                  // IsBadCodePtr(&0x6FD2E3D0[index])
    void     (*invokeSlot)(uint32_t index);                 // (&0x6FD2E3D0)[index]()
};

// ---- module state --------------------------------------------------------------------------------
static int32_t   g_gameGuids[1024];   // 0x6FD30C08: game GUID per slot, 0 empty, -1 free/ended
static void*     g_currentGameSlot;   // 0x6FD30C04: 0x38D40's cached game record
static uint32_t  g_lastGameTick;      // 0x6FD31C48: 0x2DF10's last traversal tick
static LARGE_INTEGER g_clockFrequency; // 0x6FD303A0
static uint32_t  g_clockValid;        // 0x6FD303B0: QueryPerformanceFrequency result (!= 0 -> QPC clock)
static CRITICAL_SECTION g_gameListLock; // 0x6FD30BE8

// The per-game critical section at game+0x18; compat critical sections are no-ops, but the
// null-lock check is a fatal path and is kept.
static void gameUnlockOrDie(void* game)
{
    if (rdptr(game, 0x18) == nullptr) d2game_fatal(0x100F);
}

// ---- TASK.cpp queue state (0x6FD30318..0x6FD31C58) -----------------------------------------------
// A slot descriptor is allocated by Fog #10045 in 0x52650: [0] slot index, [1] queued count,
// [2..3] the circular queue links the empty-slot initialisation points at itself. 0x52390 addresses
// the same descriptor through a tail pointer at word 4, so the model reserves six words; retail's
// allocation size is not recovered (partial).
struct TaskSlot { uintptr_t idx, count, first, last, unk4, unk5; };
static TaskSlot* g_taskSlots[32];             // 0x6FD30318, indexed by counter & 0x1F
static uint32_t  g_taskSlotCounter;           // 0x6FD303A8
static uint32_t  g_taskQueuedTotal;           // 0x6FD303AC
static uintptr_t g_taskHeadVar;               // 0x6FD30398: holds its own address once initialised
static uintptr_t* g_taskTail;                 // 0x6FD3039C
static CRITICAL_SECTION g_taskLock;           // 0x6FD31F58
static CRITICAL_SECTION g_taskSlotLocks[32];  // 0x6FD31C58, stride 0x18

// 1.13c D2Game+0x52140
// TASK_InitializeClock: record the QPC frequency; a nonzero result switches 0x52020 onto the QPC
// clock instead of GetTickCount.
void TASK_InitializeClock(void)
{
    g_clockValid = QueryPerformanceFrequency(&g_clockFrequency) ? 1u : 0u;
}

// 1.13c D2Game+0x29B10
// Count the occupied game slots (0 and -1 are the empty/ended sentinels). Walks the 1024 words in
// groups of four, as retail does, with the loop bound at the word after the last entry.
int32_t GAME_GetActiveGameCount(void)
{
    int32_t n = 0;
    const int32_t* p = &g_gameGuids[1];
    do {
        if (p[-1] != 0 && p[-1] != -1) ++n;
        if (p[0] != 0 && p[0] != -1) ++n;
        if (p[1] != 0 && p[1] != -1) ++n;
        if (p[2] != 0 && p[2] != -1) ++n;
        p += 4;
    } while (p < &g_gameGuids[1025]);
    return n;
}

// 1.13c D2Game+0x2B9A0
// Count the games whose word +0x70 is nonzero, releasing each game's lock on the way. A null lock is
// the 0x100F fatal path.
int32_t GAME_GetGamesWithState(const D2GameBatchEnv* env)
{
    int32_t n = 0;
    const int32_t* slot = &g_gameGuids[0];   // 0x6FD30C08
    do {
        if (*slot != 0 && *slot != -1) {
            void* game = env->getGameByGuid(*slot);
            if (game != nullptr) {
                if (rd32(game, 0x1C * 4) != 0) ++n;
                gameUnlockOrDie(game);
            }
        }
        ++slot;
        if (slot > &g_gameGuids[1023]) return n;   // retail stops past 0x6FD31C07
    } while (true);
}

// 1.13c D2Game+0x38D40
// Cache the current game (0x2AAE0, GUID in ESI, not rendered) and return its word at +0x2F + index.
uint32_t GAME_GetCurrentGameField(int32_t index, const D2GameBatchEnv* env)
{
    EnterCriticalSection(&g_gameListLock);   // Ordinal 10050: enter the game-list lock
    LeaveCriticalSection(&g_gameListLock);
    void* game = env->getGame();
    g_currentGameSlot = 0;
    if (game == nullptr) d2game_fatal(0x69);
    g_currentGameSlot = game;   // DAT_6FD30C04 holds the cached record
    const uint32_t value = rd32(game, 4u * (uint32_t)(index + 0x2F));
    gameUnlockOrDie(game);
    return value;
}

// 1.13c D2Game+0x2CFD0
// Client teardown wrapper: 0x2AB70 (client id in ESI) finds the current game, 0x49FE0 the client
// record; a missing record is the 0x9D9 fatal. It builds the 0x28-byte removal packet (5A 00 04 at
// the front), copies 16 bytes of the client name from +0x0D behind it, runs the name-termination
// callback 0xCBDB0(packet, game), then 0x4B7F0(game, clientId, 1) and releases the game lock.
// partial: Ghidra renders the copy at the first local, the callsite disassembly puts the 16 bytes at
// packet+8; the copy destination follows the disassembly.
void GAME_RemoveClientById(uint32_t clientId, const D2GameBatchEnv* env)
{
    if (clientId == 0) return;
    void* game = env->getCurrentGameForClient(clientId);   // 0x2AB70
    if (game == nullptr) return;
    void* client = env->getClientById(clientId);           // 0x49FE0
    uint8_t packet[0x28];
    std::memset(packet, 0, sizeof packet);
    packet[0] = 0x5A; packet[1] = 0x00; packet[2] = 0x04;
    if (client == nullptr) d2game_fatal(0x9D9);
    env->strCopy(packet + 8, (const uint8_t*)client + 0x0D, 0x10);   // Ordinal 501
    env->terminateName(packet, game);                      // 0xCBDB0
    env->removeClient(game, clientId, 1);                  // 0x4B7F0
    env->unlockGame(game);                                 // 0x29BA0
}

// 1.13c D2Game+0x2D820
// GAME_CloseAllGames (the 1.10f identity; the bodies are not line-identical): clear the task queues,
// then visit every occupied game slot, walk its client list (next at client+0x4A8), send byte 0xB0 on
// channel 0, clear the client word +0x174 when bit 0 of +0x3D4 is clear, that word is nonzero and
// 0xE03A0 finds no unit, run 0x54320 and 0x2D790, send a second byte on channel 1, remove the client
// with flag 0, release the game lock and close the game.
// partial: the second send's byte is an uninitialised stack slot (stack0xffffffe3) in the decompile and
// is modelled as the first send's one byte local (0xB0) -- unproven; the type word 0xE03A0 reads from
// EAX is not rendered and is passed as 0.
void GAME_CloseAllGames(const D2GameBatchEnv* env)
{
    env->clearTasks();   // 0x52090
    int32_t* nextSlot = g_gameGuids;
    for (;;) {
        const int32_t guid = *nextSlot;
        if (guid != 0 && guid != -1) {
            void* game = env->getGameByGuid(guid);
            if (game == nullptr) d2game_fatal(0x11B3);
            void* client = rdptr(game, 0x22 * 4);       // game+0x88: client list head
            while (client != nullptr) {
                void* next = rdptr(client, 0x12A * 4);  // client+0x4A8
                uint8_t local_d = 0xB0;
                env->send(0, rd32(client, 0), &local_d, 1);              // Ordinal 10002
                if ((rd8(client, 0x3D4) & 1) == 0 && rd32(client, 0x5D * 4) != 0 &&
                    env->getServerUnit(rd32(client, 0x6A * 4), rd32(client, 0x5C * 4), 0) == 0)
                    wr32(client, 0x5D * 4, 0);
                env->gameSweep(game);                                    // 0x74320
                env->clientFlush(client);                                // 0x2D790
                env->send(1, rd32(client, 0), &local_d, 1);              // Ordinal 10002
                env->removeClient(game, 0, 0);                           // 0x4B7F0(game, 0)
                client = next;
            }
            gameUnlockOrDie(game);
            env->closeGame(game);                                        // 0x2CE20
        }
        if (++nextSlot > &g_gameGuids[1023]) return;
    }
}

// 1.13c D2Game+0x2DF10
// One pass of the game loop. With both arguments zero it is rate-limited to one pass per 40 ms; it
// stamps the traversal tick, and for every occupied slot: 0x2D980, then, unless the game word +0x8C
// is set, closes the game once +0x1DC0 is older than 300000 ms or the game time word +0xA8 lies in
// (0x5DC, 0x1D4C); otherwise clears +0x1DC0. Always releases the lock and returns the QPC delta in
// ms (0 when no game was visited).
int32_t GAME_UpdateGames(int32_t param_1, int32_t param_2, const D2GameBatchEnv* env)
{
    bool visited = false;
    LARGE_INTEGER start;
    QueryPerformanceCounter(&start);
    const uint32_t now = GetTickCount();
    uint32_t stamp = g_lastGameTick;
    if (param_1 == 0 && param_2 == 0) {
        stamp = now;
        if (now - g_lastGameTick < 0x28) return 0;
    }
    g_lastGameTick = stamp;
    int32_t* slot = g_gameGuids;
    for (;;) {
        if (*slot != 0 && *slot != -1) {
            void* game = env->getGameByGuid(*slot);
            if (game != nullptr) {
                visited = true;
                env->frameAdvance(game);                 // 0x2D980
                if (rd32(game, 0x23 * 4) == 0) {
                    if (rd32(game, 0x770 * 4) == 0) wr32(game, 0x770 * 4, now);
                    const uint32_t age = now - rd32(game, 0x770 * 4);
                    const int32_t gameTime = (int32_t)rd32(game, 0x2A * 4);
                    if (age > 300000 || (gameTime > 0x5DC && gameTime < 0x1D4C)) {
                        env->unlockGame(game);           // 0x29BA0
                        env->closeGame(game);            // 0x2CE20
                        goto next_slot;
                    }
                } else {
                    wr32(game, 0x770 * 4, 0);
                }
                env->unlockGame(game);                   // 0x29BA0
            }
        }
    next_slot:
        if (++slot > &g_gameGuids[1023]) {
            if (!visited) return 0;
            LARGE_INTEGER end;
            QueryPerformanceCounter(&end);
            return (int32_t)(uint32_t)(end.LowPart - start.LowPart);
        }
    }
}

// 1.13c D2Game+0x2B050
// Walk param_2 table entries starting at param_5, and for each one 0xE03A0 recognises, run 0xDFB30
// and invoke the function-pointer slot at 0x6FD2E3D0 indexed by the word param_4 points at (a bad
// slot is the 0x141F fatal). Returns how many entries were recognised; releases the game lock.
// partial: the decompiler folds the array cursor into "param_5 - param_4 + param_4", which advances
// the read from param_5 while param_4 itself supplies the slot index; both are kept literally, and
// the type word 0xE03A0 takes in EAX is not rendered (passed as 0).
int32_t GAME_InvokeUnitCallbacks(uint32_t param_1, int32_t param_2, uint32_t param_3,
                                 int32_t* param_4, int32_t param_5, const D2GameBatchEnv* env)
{
    (void)param_1; (void)param_3;
    EnterCriticalSection(&g_gameListLock);   // Ordinal 10050
    LeaveCriticalSection(&g_gameListLock);
    void* game = env->getGame();
    uint32_t n = 0;
    if (game == nullptr) return 0;
    if (param_2 > 0) {
        const uintptr_t delta = (uintptr_t)(intptr_t)param_5 - (uintptr_t)(intptr_t)param_4;
        int32_t i = 0;
        do {
            const uint32_t id = *(const uint32_t*)(uintptr_t)(delta + (uintptr_t)(intptr_t)param_4);
            if (env->getServerUnit((uint32_t)(uintptr_t)game, id, 0) != 0) {
                env->fillUnitInfo((uint32_t)(uintptr_t)game, id);       // 0xDFB30
                if (*param_4 != 0) {
                    if (env->slotIsBad((uint32_t)*param_4) != 0) d2game_fatal(0x141F);
                    env->invokeSlot((uint32_t)*param_4);
                }
                ++n;
            }
            ++i;
            ++param_4;
        } while (i < param_2);
    }
    gameUnlockOrDie(game);
    return (int32_t)n;
}

// 1.13c D2Game+0x2AD80
// Write a game log line: terminate at 0xFF, and when it starts with "<ABEND> " take the low nibble of
// byte 8 as the abort code into game word +0x1DC8, then 0xCBE10(code, text). The code passed on the
// non-abort path is the caller's live ECX (the decompiler's extraout_ECX), modelled as an argument.
void GAME_LogMessage(uint32_t param_1, char* text, uint32_t ecxIn, const D2GameBatchEnv* env)
{
    EnterCriticalSection(&g_gameListLock);   // Ordinal 10050
    LeaveCriticalSection(&g_gameListLock);
    void* game = env->getGame();
    if (game == nullptr) d2game_fatal(0x1511);
    text[0xFF] = '\0';
    uint32_t code = ecxIn;
    if (std::strncmp("<ABEND> ", text, 8) == 0) {   // retail's _strncmp
        code = (uint8_t)text[8] & 0xF;
        wr32(game, 0x772 * 4, code);
    }
    env->logMessage(code, text);   // 0xCBE10
    gameUnlockOrDie(game);
}

// 1.13c D2Game+0x2B550
// Fill the caller's summary record from the current game: word +0x28 (16-bit) and +0x7C..+0xA4,
// five 0x2A680 counters at words 3..7, five percentages over game word +0x1DB4 when nonzero (and a
// 0x12-word reset at +0x1D70 when it exceeds 200), three string copies, uptime in minutes, a tenth
// of game word +0xA8, and the game/level/class bytes. Finally word +0x1D28's +8 and game word +0x1C
// at words 0x2A/0x2B. Returns 0 when no game is current, else 1.
uint32_t GAME_GetGameSummary(uint32_t param_1, uint32_t* out, const D2GameBatchEnv* env)
{
    (void)param_1;
    EnterCriticalSection(&g_gameListLock);   // Ordinal 10050
    LeaveCriticalSection(&g_gameListLock);
    void* game = env->getGame();
    if (game == nullptr) return 0;
    out[0] = (uint16_t)rd32(game, 0x0A * 4);
    out[1] = rd32(game, 0x1F * 4);
    out[2] = rd32(game, 0x23 * 4);
    out[10] = rd32(game, 0x24 * 4);
    out[11] = rd32(game, 0x25 * 4);
    out[12] = rd32(game, 0x26 * 4);
    out[13] = rd32(game, 0x27 * 4);
    out[14] = rd32(game, 0x28 * 4);
    out[15] = rd32(game, 0x29 * 4);
    out[3] = (uint32_t)env->gameQuery(game);   // 0x2A680 x5 but its own count too; five consecutive reads
    out[4] = (uint32_t)env->gameQuery(game);
    out[5] = (uint32_t)env->gameQuery(game);
    out[6] = (uint32_t)env->gameQuery(game);
    out[7] = (uint32_t)env->gameQuery(game);
    const uint32_t total = rd32(game, 0x76D * 4);
    if (total != 0) {
        out[0x10] = (rd32(game, 0x75E * 4) * 100) / total;
        out[0x11] = (rd32(game, 0x761 * 4) * 100) / total;
        out[0x12] = (rd32(game, 0x762 * 4) * 100) / total;
        out[0x13] = (rd32(game, 0x75D * 4) * 100) / total;
        out[0x14] = (rd32(game, 0x76B * 4) * 100) / total;
        out[0x15] = total;
        if (total > 200) {
            for (uint32_t i = 0; i < 0x12; ++i) wr32(game, 4u * (0x75C + i), 0);
        }
    }
    env->strCopy((uint8_t*)out + 0x64, (const uint8_t*)game + 0x2A, 0x10);  // Ordinal 501, out+0x19
    const uint32_t now = GetTickCount();
    out[0x17] = (now - rd32(game, 0x76F * 4)) / 60000;
    out[0x16] = (uint32_t)((int32_t)rd32(game, 0x2A * 4) / 100);
    out[0x18] = rd32(game, 0x2B * 4);
    ((uint8_t*)out)[0xA4] = rd8(game, 0x6B);
    ((int8_t*)out)[0xA5] = (int8_t)rd32(game, 0x1B * 4);
    ((uint8_t*)out)[0xA6] = rd8(game, 0x6E);
    env->strCopy((uint8_t*)out + 0x74, (const uint8_t*)game + 0x3A, 0x10);  // Ordinal 501, out+0x1D
    env->strCopy((uint8_t*)out + 0x84, (const uint8_t*)game + 0x4A, 0x20);  // Ordinal 501, out+0x21
    if (rd32(game, 0x74A * 4) == 0) d2game_fatal(0x139);
    out[0x2A] = rd32((const void*)(uintptr_t)rd32(game, 0x74A * 4), 8);
    out[0x2B] = rd32(game, 0x7 * 4);
    gameUnlockOrDie(game);
    return 1;
}

// 1.13c D2Game+0x52390
// Task queue insert (Task.cpp): stamp the record (0x52020's low word at [0], the caller's two words at
// [1]/[2]) and link its embedded node at rec+3 into the slot behind the tail pointer. An empty list is
// initialised and returns 0; a null node, null record node or missing head is a fatal (0x77/0x78/0x6C).
// partial: the tail-based link at words 4/5 of the slot descriptor and the two unlinks through it are
// transcribed literally; the descriptor's retail size (Fog #10045 in 0x52650) is not recovered.
uint32_t TASK_InsertTask(uint32_t* rec, uint32_t param_2, uint32_t param_3, const D2GameBatchEnv* env)
{
    uint32_t result = 0;
    rec[0] = (uint32_t)env->taskClock();   // 0x52020 returns a 64-bit count; the low word is stored
    rec[1] = param_2;
    rec[2] = param_3;
    EnterCriticalSection(&g_taskLock);
    uintptr_t* tail = g_taskTail;
    if (g_taskHeadVar == 0) {
        g_taskHeadVar = (uintptr_t)&g_taskHeadVar;
        g_taskTail = &g_taskHeadVar;
        LeaveCriticalSection(&g_taskLock);
        return 0;
    }
    if (tail != &g_taskHeadVar && tail != nullptr) {
        const uint32_t slot = (uint32_t)tail[-4];
        EnterCriticalSection(&g_taskSlotLocks[slot & 31]);
        uintptr_t* node = (uintptr_t*)(rec + 3);    // rec word 3 (byte 12)
        if (tail - 2 == nullptr) {
            result = 0x77;
        } else if (node == nullptr) {
            result = 0x78;
        } else {
            wrptr(rec, 12, tail - 2);
            const uintptr_t prev = tail[-1];
            wrptr(rec, 16, (const void*)prev);
            wrptr((void*)prev, 0, node);
            tail[-1] = (uintptr_t)node;
            LeaveCriticalSection(&g_taskSlotLocks[slot & 31]);
            tail[-3] += 1;
            g_taskQueuedTotal += 1;
            wr32((void*)*tail, 4, (uint32_t)tail[1]);
            wr32((void*)tail[1], 0, (uint32_t)*tail);
            const uintptr_t head = g_taskHeadVar;
            if (head != 0) {
                *tail = head;
                const uintptr_t next = (uintptr_t)rdptr((const void*)head, 4);
                tail[1] = next;
                wrptr((void*)next, 0, tail);
                wrptr((void*)head, 4, tail);
                LeaveCriticalSection(&g_taskLock);
                return 1;
            }
            result = 0x6C;
        }
        LeaveCriticalSection(&g_taskSlotLocks[slot & 31]);
        d2game_fatal(result);
    }
    LeaveCriticalSection(&g_taskLock);
    return result;
}

// 1.13c D2Game+0x52650
// Round-robin task queue slot: initialise the list head once, take the next slot number (counter &
// 0x1F), and, when the slot has no descriptor yet, allocate one through Fog #10045 named
// "..\\Source\\D2Game\\GAME\\Task.cpp":0x3F7, set [0]=slot, [1]=0, self-link [2]/[3], publish it and
// 0x524C0 it. Returns the slot number.
uint32_t TASK_GetQueueSlot(const D2GameBatchEnv* env)
{
    EnterCriticalSection(&g_taskLock);
    if (g_taskHeadVar == 0) {
        g_taskHeadVar = (uintptr_t)&g_taskHeadVar;
        g_taskTail = &g_taskHeadVar;
    }
    const uint32_t index = g_taskSlotCounter & 0x1F;
    g_taskSlotCounter = g_taskSlotCounter + 1;
    EnterCriticalSection(&g_taskSlotLocks[index]);
    if (g_taskSlots[index] == nullptr) {
        TaskSlot* slot = (TaskSlot*)env->allocPool("..\\Source\\D2Game\\GAME\\Task.cpp", 0x3F7, 0);
        slot->idx = index;
        slot->count = 0;
        slot->first = (uintptr_t)&slot->first;
        slot->last = (uintptr_t)&slot->first;
        g_taskSlots[index] = slot;
        env->taskEnqueue(slot);   // 0x524C0
    }
    LeaveCriticalSection(&g_taskSlotLocks[index]);
    LeaveCriticalSection(&g_taskLock);
    return index;
}
