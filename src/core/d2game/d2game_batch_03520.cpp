// src/core/d2game/d2game_batch_03520.cpp -- ten 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: the two MSVC CRT string
// routines, the three TASK.cpp list helpers, the CRT exit path, the 64-bit multiply helper, and three
// GAME.cpp game-record sweeps. Stage A batch lane (OBJECTIVE v9): implementation + marker only, no spec,
// no vector.
//
// Host model: retail's 32-bit module globals are the statics below (retail address noted); a game or
// client record is opaque and read/written through retail byte offsets. The callees whose implementations
// live outside this batch are seam function pointers in D2GameBatchSeams, in retail argument order -- a
// Ghidra call that passed its only argument in a register (ESI/EAX) gets an explicit parameter here.
// Critical sections, __lock and IsBadCodePtr are the compat no-ops (src/core/d2moo/compat/Windows.h):
// the lock/validity paths collapse to their effect. The fatal path is Ordinal_10265(id) ->
// Ordinal_10024(&moduleName, id) -> _exit(-1); only the exit is observable and is modelled.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

// ---- module state --------------------------------------------------------------------------------
static int32_t g_gameSlots[1025];       // 0x6FD30C04: game id per slot (0 empty, -1 free); slot i+1 == 0x6FD30C08+4i
static constexpr int kGameSlotCount = 1024;   // the sweep bound 0x6FD30C08..0x6FD31C08
static uint32_t g_gameListCount;        // 0x6FD31C24: game-list / server-time feature gate
static uint8_t  g_callbackByte;         // 0x6FD31C30
static void*    g_callbacks;            // 0x6FD31C3C: optional callback block, see D2GameCallbackBlock
static uint8_t  g_exitDirect;           // 0x6FD2FD64: exit() re-entered a direct TerminateProcess
static uint8_t  g_exitRunning;          // 0x6FD2FD60
static uint8_t  g_exitMode;             // 0x6FD2FD5C
typedef void (*D2ExitFn)(void);
static D2ExitFn* g_onexitCur;           // 0x6FD3B350: walks down the onexit table
static D2ExitFn* g_onexitBegin;         // 0x6FD3B354: table start
static uint32_t g_inittermFinal[1];     // 0x6FD1F040: __initterm table (empty in this model)
static uint32_t g_inittermExit[1];      // 0x6FD1F04C

// 0x6FD19FF9: the module-name literal retail passes as Ordinal_10024's source text.
static const char kModuleName[] = "D2Game.dll";

// The optional 1.13c callback block at 0x6FD31C3C. Only the two slots the decompiles call are named;
// retail reads no other field before a null test.
struct D2GameCallbackBlock {
    uint8_t pad00[8];
    void  (*fn08)(void);                 // +0x08
    uint8_t pad0C[0x14];
    void  (*fn20)(uint32_t);             // +0x20
};

// Unmapped callees, in the order the decompiles use them (address = retail RVA).
struct D2GameBatchSeams {
    void* (*getGame)(const int32_t* guid);              // 0x2AAE0, guid in a register
    void* (*getGameForClient)(uint32_t clientId);       // 0x2AB70
    void* (*getClientById)(uint32_t clientId);          // 0x49FE0
    void* (*fn4A9B0)(void* game);                       // 0x4A9B0
    void  (*fn4AC60)(void* game);                       // 0x4AC60
    void  (*fn4D090)(void* game);                       // 0x2D090
    void  (*fn8A3E0)(void* out, uint32_t one);          // 0x8A3E0
    void  (*fn49D40)(void);                             // 0x49D40
    void  (*unlockGame)(void);                          // 0x29BA0
    void  (*fn10000)(void);                             // D2Net Ordinal_10000
    void  (*fn10021)(void);                             // Ordinal_10021
    void  (*sendPacket)(const void* packet);            // D2Net Ordinal_10002, frame in a register
    int   (*strCopy)(void* dst, const void* src, int max); // Storm Ordinal_501
    void  (*fn2250)(void);                              // 0x2250
    void  (*initterm)(void* table);                     // __initterm (0x40E4 / 0x411F)
    void  (*crtExitLocked)(void);                       // 0x2A28
    void  (*crtExitProcess)(uint32_t code);             // ___crtExitProcess
    void  (*terminateProcess)(uint32_t code);           // GetCurrentProcess/TerminateProcess
};

// ---- CRT string routines -------------------------------------------------------------------------
// 1.13c D2Game+0x3520
int d2game_strncmp(const char* s1, const char* s2, size_t maxCount)
{
    if (maxCount == 0) return 0;
    // Retail first counts s1's bytes through its terminator (capped at maxCount), then compares.
    size_t limit = 0;
    while (limit < maxCount && s1[limit] != '\0') ++limit;
    if (limit < maxCount) ++limit;                       // include the compared terminator
    for (size_t i = 0; i < limit; ++i) {
        const unsigned char a = (unsigned char)s1[i], b = (unsigned char)s2[i];
        if (a != b) return b <= a ? 1 : -1;              // retail: ~0xFFFFFFFE = 1, ~0 = -1
    }
    return 0;
}

// 1.13c D2Game+0x2E50
char* d2game_strncpy(char* dest, const char* source, size_t count)
{
    if (count == 0) return dest;
    char* d = dest;
    for (;;) {
        const char c = *source++;
        *d++ = c;
        if (--count == 0) return dest;
        if (c == '\0') break;
    }
    while (count-- > 0) *d++ = '\0';                      // zero-fill the remainder
    return dest;
}

// ---- TASK.cpp doubly-linked list ----------------------------------------------------------------
// D2LinkStrc layout: +0 next, +4 prev. 1.10f's equivalents are declared at Task.h:60/63 (RVAs differ).
struct D2TaskLink { D2TaskLink* next; D2TaskLink* prev; };

// 1.13c D2Game+0x1E10
void D2Task_LinkList_InsertBeforeHead(D2TaskLink* head, D2TaskLink* entry)
{
    if (head == nullptr) d2game_fatal(0x77);
    if (entry == nullptr) d2game_fatal(0x78);
    entry->next = head;
    entry->prev = head->prev;
    head->prev->next = entry;                            // *(head->prev) = entry
    head->prev = entry;                                  // *(head+4) = entry
}

// 1.13c D2Game+0x1E50
void D2Task_LinkList_Remove(D2TaskLink* entry)
{
    if (entry == nullptr) d2game_fatal(0x87);
    entry->next->prev = entry->prev;                     // *(entry->next + 4) = entry->prev
    entry->prev->next = entry->next;                     // *(entry->prev) = entry->next
}

// ---- GAME.cpp object release (caller 0x2CE20 closeGame) -----------------------------------------
// EAX = object whose first dword is its vtable; the return-address slot doubles as the "unlock" flag.
// 1.13c D2Game+0x1F30
void GAME_1F30(void* object, bool leaveLock, const D2GameBatchSeams& s)
{
    s.fn2250();                                          // 0x2250
    (*(void (**)(void))rdptr(object, 0))();              // (**(code **)*object)()
    if (leaveLock) {
        // LeaveCriticalSection((LPCRITICAL_SECTION)(object + 0x14 words)): compat critical section is a no-op.
        (void)object;
    }
}

// ---- CRT exit path ------------------------------------------------------------------------------
// 1.13c D2Game+0x2979
void d2game_doexit(const D2GameBatchSeams& s, uint32_t code, int skipOnexit, int keepProcess)
{
    // __lock(8): compat no-op.
    if (g_exitDirect == 1) s.terminateProcess(code);
    g_exitRunning = 1;
    g_exitMode = (uint8_t)keepProcess;
    if (skipOnexit == 0) {
        if (g_onexitBegin != nullptr) {
            while ((g_onexitCur -= 1) >= g_onexitBegin) {
                if (*g_onexitCur != nullptr) (*g_onexitCur)();
            }
        }
        s.initterm(g_inittermFinal);
    }
    s.initterm(g_inittermExit);
    s.crtExitLocked();                                   // 0x2A28
    if (keepProcess != 0) return;
    g_exitDirect = 1;
    s.crtExitProcess(code);
}

// ---- 64-bit multiply helper ----------------------------------------------------------------------
// 1.13c D2Game+0xA090
int64_t d2game_allmul(uint32_t lo1, int32_t hi1, uint32_t lo2, int32_t hi2)
{
    if (hi2 == 0 && hi1 == 0) return (int64_t)((uint64_t)lo1 * lo2);
    const uint64_t x = ((uint64_t)(uint32_t)hi1 << 32) | lo1;
    const uint64_t y = ((uint64_t)(uint32_t)hi2 << 32) | lo2;
    return (int64_t)(x * y);                             // CONCAT44: same low 64 bits either way
}

// ---- GAME.cpp game-record sweeps -----------------------------------------------------------------
// Packet frame used by both sweeps: the locals span frame -0x48..-0x05, i.e. 0x45 bytes.
// The first copy writes the game's name string unbounded (Ordinal_501 max 0x7FFFFFFF) into the 16-byte
// frame head; retail relies on the source being short, and the opcode/address fields then overwrite from
// +4. The bytes outside the fields each branch writes are retail stack garbage; the buffer is zeroed
// here so a replay starts from a fixed value.
static constexpr uint32_t kFrameSize = 0x45;

// 1.13c D2Game+0x2BA10
void GAME_2BA10(const D2GameBatchSeams& s, const void* clientAddr16)
{
    uint8_t pkt[kFrameSize] = {};
    for (int i = 0; i < kGameSlotCount; ++i) {
        const int32_t guid = g_gameSlots[i + 1];         // &DAT_6fd30c08 + 4*i
        if (guid == 0 || guid == -1) continue;
        void* game = s.getGame(&g_gameSlots[i + 1]);
        if (game == nullptr) continue;
        const uint32_t a = rd32(game, 0x8C);                        // puVar3[0x23]
        const uint32_t b = rd32(game, 0x28);                        // puVar3[10]
        s.strCopy(pkt, (const uint8_t*)game + 0x2A, 0x7FFFFFFF);    // game name into frame head
        if (rdptr(game, 0x18) == nullptr) d2game_fatal(0x100F);
        // LeaveCriticalSection(game+0x18): compat no-op.
        wr8 (pkt, 4,  0xB2);                             // uStack_44
        wr16(pkt, 0x35, (uint16_t)a);                    // uStack_13
        wr16(pkt, 0x37, (uint16_t)b);                    // uStack_11
        s.strCopy(pkt + 5, clientAddr16, 0x10);          // auStack_43
        s.sendPacket(pkt);
    }
    wr8 (pkt, 0x10, 0xB2);                               // local_38
    wr16(pkt, 0x39, 0);                                  // local_7
    wr16(pkt, 0x3B, 0xFFFF);                             // local_5
    s.strCopy(pkt + 0x11, kModuleName, 0x10);            // local_37
    s.sendPacket(pkt);
}

// 1.13c D2Game+0x2BB10
void GAME_2BB10(const D2GameBatchSeams& s, uint32_t param_1, uint32_t param_4)
{
    if (g_gameListCount == 0) return;
    // EnterCriticalSection(&0x6FD30BE8) / LeaveCriticalSection: compat no-ops.
    const int32_t* slot = &g_gameSlots[param_1 & 0xFFFF]; // (&DAT_6fd30c04)[param_1 & 0xffff]
    void* game = nullptr;
    if (*slot != 0 && *slot != -1) game = s.getGame(slot);
    if (game == nullptr) return;
    s.fn10021();
    void* p = s.fn4A9B0(game);                           // FUN_6fc6a9b0
    const D2GameCallbackBlock* cb = (const D2GameCallbackBlock*)g_callbacks;
    if (p == nullptr) {
        if (cb != nullptr && cb->fn20 != nullptr) {
            if (cb->fn20 == nullptr) d2game_fatal(0x769); // IsBadCodePtr collapses to a null test
            cb->fn20(param_4);
        }
        s.unlockGame();                                  // FUN_6fc49ba0
        s.sendPacket(nullptr);                           // Ordinal_10002(0), frame in a register
        s.fn10000();
        return;
    }
    s.fn4AC60(game);                                     // FUN_6fc4ac60
    wr8(p, 0x1AC, g_callbackByte);                       // *(undefined1 *)(puVar2 + 0x6b)
    s.unlockGame();
    if (cb != nullptr && cb->fn08 != nullptr) {
        if (cb->fn08 == nullptr) d2game_fatal(0x795);
        cb->fn08();
        s.fn49D40();                                     // FUN_6fc69d40
        return;
    }
    uint32_t local = 0;
    s.fn8A3E0(&local, 1);                                // FUN_6fcaa3e0(&stack0xfffffffc, 1)
}

// 1.13c D2Game+0x2D7E0
void GAME_2D7E0(const D2GameBatchSeams& s, uint32_t clientId)
{
    if (clientId == 0) return;
    void* game = s.getGameForClient(clientId);           // FUN_6fc4ab70
    if (game == nullptr) return;
    s.getClientById(clientId);                           // FUN_6fc69fe0, result unused
    s.fn4D090(game);                                     // FUN_6fc4d090
    s.unlockGame();                                      // FUN_6fc49ba0
}
