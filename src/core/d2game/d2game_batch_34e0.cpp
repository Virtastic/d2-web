// src/core/d2game/d2game_batch_34e0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: the __ArrayUnwind SEH
// fragment of `eh vector constructor iterator', the type_info destructor and its __unlock(0xE) thunk, the
// type_info scalar deleting destructor, __CRT_INIT@12, the quest reward/GUID helper shared by the quest
// callbacks, the quest ResetQuest helper, the game event-handler dispatch over the list at game+0x74, the
// two 0x5D packet helpers, the NPC-intro packet builder and the NPC-intro flag setter. Stage A batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Host model: retail module state is opaque and is read by its retail byte offsets (rd8/rd16/rd32/rdptr,
// wr8/wr16/wr32/wrptr). Every unmapped callee is a seam on D2GameBatch20Env, in retail argument order.
// The CRT locks and the CRT startup chain collapse to the host libc (as in d2game_batch_3261.cpp), so
// __CRT_INIT keeps its PROCESS/THREAD reason state machine and lets the host own the platform steps.
// A probe passes the env pointer in r[5]; the functions take retail arguments in r[0..3], return in r[4],
// per the retail capture tool/fn/README.md.
//
// Prior art: src/core/d2game/d2game_batch_3261.cpp (0x34B8/0x3502 SEH fragment, CRT locks),
// src/core/d2moo/src/D2Game/PLAYER/PlrIntro.cpp (npcIndexMap, Get/SetNpcIntroFlag) and
// src/core/d2moo/src/D2Game/QUESTS/Quests.cpp (QUESTS_GetQuestData, QUESTS_FastRemovePlayerGUID).
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement here.
struct D2GameBatch20Env {
    // Ordinal_10920 UNITS_GetPlayerData(unit).
    void*    (*getPlayerData)(void* unit);
    // Ordinal_10174 QUESTRECORD_GetQuestState(pQuestFlags, nQuest, nFlag) -> 0/1.
    int32_t  (*getQuestState)(void* pQuestFlags, int32_t nQuest, int32_t nFlag);
    // 0x33830 QUESTS_FastRemovePlayerGUID(pGUIDs, dwGUID).
    int32_t  (*fastRemovePlayerGUID)(void* pGUIDs, int32_t dwGUID);
    // 0x33F30 QUESTS_GetQuestData(pGame, nQuestId).
    void*    (*getQuestData)(void* pGame, int32_t nQuestId);
    // Ordinal_10331 UNITS_GetRoom(unit).
    void*    (*getRoom)(void* unit);
    // Ordinal_10826 DUNGEON_GetLevelIdFromRoom(room).
    uint32_t (*getLevelIdFromRoom)(void* room);
    // Ordinal_10459 level/act table lookup, returning the act byte.
    uint32_t (*levelLookup)(uint32_t levelId);
    // Ordinal_10119 BITMANIP_GetBitState(pBuffer, bit).
    int32_t  (*getBitState)(void* pBuffer, uint32_t bit);
    // Ordinal_10118 BITMANIP_SetBitState(pBuffer, bit).
    void     (*setBitState)(void* pBuffer, uint32_t bit);
    // 0x8A3E0 D2GAME_PACKETS_SendPacket(packet, size).
    void     (*sendPacket)(const void* packet, uint32_t size);
    // IsBadCodePtr(fn): nonzero when fn is not callable.
    int32_t  (*isBadCodePtr)(void* fn);
    // 0x34510's call *0xA0(unit, 5*4): __fastcall handler(unit in ECX, event record in EDX).
    void     (*dispatchHandler)(void* unit, const uint32_t* event);
    // 0x3502's destructor callback argument of __ArrayUnwind.
    void     (*arrayUnwindOne)(void* element);
};
static const D2GameBatch20Env kNoEnv = {};
static const D2GameBatch20Env& d2game_env(const uint32_t* r) {
    return r[5] ? *(const D2GameBatch20Env*)(uintptr_t)r[5] : kNoEnv;
}

// 0x28E5/0x3F76 CRT lock table: the compat critical section is a no-op, so the lock is its index.
static void d2game_crt_lock(uint32_t index)   { (void)index; }
static void d2game_crt_unlock(uint32_t index) { (void)index; }

// __ArrayUnwind rule: call the callback on each of `count` elements, advancing `size` bytes.
static void d2game_array_unwind(void* ptr, uint32_t size, int32_t count, void (*callOne)(void*)) {
    uint8_t* element = (uint8_t*)ptr;
    while (--count >= 0) {
        if (callOne != nullptr) callOne(element);
        element += size;
    }
}

// The NPC class-id -> intro bit index table at 1.13c 0x6FD27E50 (count 0x6FD27F68). Same pairs as the
// vendored 1.10f npcIndexMap in src/core/d2moo/src/D2Game/PLAYER/PlrIntro.cpp; a missing id maps to 0.
struct D2NpcIndexMapStrc { int32_t nNpcClassId; int32_t nIndex; };
static const D2NpcIndexMapStrc kNpcIndexMap[] = {
    { -1, 0 }, { 147, 1 }, { 148, 2 }, { 150, 3 }, { 155, 4 }, { 154, 5 }, { 265, 6 },
    { 175, 7 }, { 176, 8 }, { 177, 9 }, { 178, 10 }, { 202, 11 }, { 200, 12 }, { 210, 13 },
    { 201, 14 }, { 198, 15 }, { 199, 16 }, { 244, 17 }, { 245, 18 }, { 246, 19 }, { 251, 20 },
    { 252, 21 }, { 253, 22 }, { 254, 23 }, { 255, 24 }, { 256, 25 }, { 257, 26 }, { 264, 27 },
    { 297, 28 }, { 511, 29 }, { 512, 30 }, { 513, 31 }, { 514, 32 }, { 515, 33 }, { 520, 34 },
};
static constexpr uint32_t kNpcIndexMapCount = sizeof(kNpcIndexMap) / sizeof(kNpcIndexMap[0]);

static uint32_t d2game_npc_index(int32_t nNpcClassId) {
    for (uint32_t i = 0; i < kNpcIndexMapCount; ++i) {
        if (kNpcIndexMap[i].nNpcClassId == nNpcClassId) return (uint32_t)kNpcIndexMap[i].nIndex;
    }
    return 0;
}

// The intro bit buffer hangs off the player-intro record as pFlags->pBuffer (record+4 -> flags object).
static void* d2game_intro_bits(void* pPlayerIntro) {
    return rdptr(rdptr(pPlayerIntro, 4), 0);
}

// __CRT_INIT@12 state (retail 0x6FD2FD68 = live-init count, 0x6FD2FD60 = exit-in-progress flag).
static uint32_t g_crtInitCount = 0;
static uint32_t g_crtExitFlag = 0;

extern "C" {
// 1.13c D2Game+0x3502
// SEH unwind fragment of `eh vector constructor iterator' (0x34B8): the selector word at EBP-0x20 being
// 0 selects the handler, which unwinds the iterator's arguments -- ptr, size, count, callback. Those are
// r[1..3] here; r[0] is the selector. The callback is a seam (env.arrayUnwindOne).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3502(uint32_t* r) {
    if ((int32_t)r[0] == 0)
        d2game_array_unwind((void*)(uintptr_t)r[1], r[2], (int32_t)r[3], d2game_env(r).arrayUnwindOne);
    r[4] = 0;
}

// 1.13c D2Game+0x3559
// type_info::~type_info: store the type_info vftable (no host analogue), __lock(0xE), free the name
// string at +4, then the __unlock(0xE) thunk (0x3596).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3559(uint32_t* r) {
    void* self = (void*)(uintptr_t)r[0];
    d2game_crt_lock(0xE);
    void* name = rdptr(self, 4);
    if (name != nullptr) std::free(name);
    d2game_crt_unlock(0xE);
    r[4] = 0;
}

// 1.13c D2Game+0x3596
// __unlock(0xE) thunk reached from the type_info destructor (via 0x3F76): the compat critical section is
// a no-op.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3596(uint32_t* r) {
    d2game_crt_unlock(0xE);
    r[4] = 0;
}

// 1.13c D2Game+0x359F
// type_info::`scalar deleting destructor'(flags): run the destructor, free the object when bit 0 of
// flags is set (0x1000 is operator delete), and return the object pointer.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_359f(uint32_t* r) {
    void* self = (void*)(uintptr_t)r[0];
    d2game_crt_lock(0xE);
    void* name = rdptr(self, 4);
    if (name != nullptr) std::free(name);
    d2game_crt_unlock(0xE);
    if ((r[1] & 1) != 0) std::free(self);
    r[4] = (uint32_t)(uintptr_t)self;
}

// __RTC_Initialize(void): the retail callback range begins and ends at DAT_6fd1d35c, so its
// initialization loop performs no calls. The host runtime has no D2Game RTC callbacks to register.
static void d2_fn_d2game_405c() {} // 1.13c D2Game+0x0405C

// 1.13c D2Game+0x35BB
// __CRT_INIT@12(hinst, reason): the MSVC 2003 CRT startup. PROCESS_ATTACH records the OS version and runs
// heap/multithread/RTC/cmdline/environment/io/argv/envp/cinit, incrementing the live-init count;
// PROCESS_DETACH decrements it and, when no __cexit handler ran, tears the CRT down; THREAD_ATTACH
// allocates the per-thread data record; THREAD_DETACH frees it. The host libc owns all of that state, so
// the platform steps collapse to the reason state machine and the counter (as in d2game_batch_3261.cpp).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_35bb(uint32_t* r) {
    switch (r[1]) {
    case 0: // DLL_PROCESS_DETACH
        if (g_crtInitCount > 0) {
            --g_crtInitCount;
            // __cexit when 0x6FD2FD60 is clear, then __ioterm/__mtterm/__heap_term: host-owned.
            (void)g_crtExitFlag;
            r[4] = 1;
            return;
        }
        r[4] = 0;
        return;
    case 1: // DLL_PROCESS_ATTACH
        // GetVersionExA + __heap_init + __mtinit + __RTC_Initialize + command line/environment +
        // __RTC_Initialize's callback range is empty (start == end), so this step has no side effects.
        d2_fn_d2game_405c();
        // __ioinit/__setargv/__setenvp/__cinit: all succeed under the host runtime in this order; a
        // failure at any point unwinds the steps already taken and returns 0.
        ++g_crtInitCount;
        r[4] = 1;
        return;
    case 2: // DLL_THREAD_ATTACH
        // Retail calloc(1, 0x8C)s a _ptiddata, initializes it and stores the current thread id.
        // The host runtime carries its own per-thread data; success.
        r[4] = 1;
        return;
    case 3: // DLL_THREAD_DETACH
        // Retail __freeptd(NULL).
        r[4] = 1;
        return;
    default: // DLL_THREAD_ATTACH of a private reason
        r[4] = 1;
        return;
    }
}

// 1.13c D2Game+0x34440
// ResetQuest(pGame in EAX, nQuestId in ECX): clear bNotIntro (+9), bActive (+0xA) and fLastState (+0xB)
// of the quest record; a missing record is the retail 0x5F3 assert.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34440(uint32_t* r) {
    void* pQuest = d2game_env(r).getQuestData((void*)(uintptr_t)r[0], (int32_t)r[1]);
    if (pQuest == nullptr) d2game_fatal(0x5F3);
    wr8(pQuest, 9, 0);
    wr8(pQuest, 0xA, 0);
    wr8(pQuest, 0xB, 0);
    r[4] = 0;
}

// 1.13c D2Game+0x34510
// Event dispatch: walk the list at game+0x74 (unit record at node, next at node+4); for a unit whose byte
// +0xA is 1, take handler[5] from the table at unit+0xA0 (0xA0 + 5*4), reject a non-callable pointer with
// the retail 0x23E assert, and call it __fastcall(unit, event) where event is the 16-byte record
// {ecxArg, 5, game, edxArg} built on the stack. r[0]=game (EAX), r[1]=ecx arg, r[2]=edx arg.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34510(uint32_t* r) {
    const D2GameBatch20Env& env = d2game_env(r);
    void* pGame = (void*)(uintptr_t)r[0];
    const uint32_t event[4] = { r[1], 5, r[0], r[2] };
    void* node = rdptr(pGame, 0x74);
    while (node != nullptr) {
        void* unit = rdptr(node, 0);
        void* handler = rdptr(unit, 0xA0 + 5 * 4);
        if (handler != nullptr && rd8(unit, 0xA) == 1) {
            if (env.isBadCodePtr(handler) != 0) d2game_fatal(0x23E);
            env.dispatchHandler(unit, event);
        }
        node = rdptr(node, 4);
    }
    r[4] = 0;
}

// 1.13c D2Game+0x34AA0
// 0x5D packet helper: send the 6-byte {0x5D, byte, 0x00, 0x0C, 0, 0} when the unit's room has no level id
// or its act is >= minLevel. r[0]=unit (ESI), r[1]=packet byte, r[2]=minLevel; the player-data word the
// retail code computes at playerData+0x9C is not stored into the packet.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34aa0(uint32_t* r) {
    const D2GameBatch20Env& env = d2game_env(r);
    void* unit = (void*)(uintptr_t)r[0];
    bool send;
    void* room = env.getRoom(unit);
    if (room == nullptr) {
        send = true;
    } else {
        const uint32_t nLevelId = env.getLevelIdFromRoom(room);
        send = nLevelId != 0 && env.levelLookup(nLevelId) >= (uint8_t)r[2];
    }
    if (!send) { r[4] = 0; return; }

    if (unit != nullptr && rd32(unit, 0) == 0) {
        void* pPlayerData = env.getPlayerData(unit);
        (void)rd32(pPlayerData, 0x9C); // computed by retail and dropped
    }
    uint8_t packet[6] = { 0x5D, (uint8_t)r[1], 0x00, 0x0C, 0x00, 0x00 };
    env.sendPacket(packet, 6);
    r[4] = 0;
}

// 1.13c D2Game+0x34B20
// 0x5D packet helper: send the 6-byte {0x5D, byte, 0x02, 0x00, 0, 0}. r[0]=unit (EAX), r[1]=packet byte;
// as in 0x34AA0 the player-data word computed at playerData+0x9C is dropped.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34b20(uint32_t* r) {
    const D2GameBatch20Env& env = d2game_env(r);
    void* unit = (void*)(uintptr_t)r[0];
    if (unit != nullptr && rd32(unit, 0) == 0) {
        void* pPlayerData = env.getPlayerData(unit);
        (void)rd32(pPlayerData, 0x9C); // computed by retail and dropped
    }
    uint8_t packet[6] = { 0x5D, (uint8_t)r[1], 0x02, 0x00, 0x00, 0x00 };
    env.sendPacket(packet, 6);
    r[4] = 0;
}

// 1.13c D2Game+0x34C70
// Build the 0x91 packet's NPC-intro payload: fill the twelve u16s at buffer+2 with 0xFFFF, then for each
// of `count` NPC ids append the id u16 (advancing the write cursor) when its intro bit is set. Returns 1
// when at least one id was appended, 0 otherwise. r[0]=pPlayerIntro, r[1]=count, r[2]=npc id array,
// r[3]=packet buffer (the caller writes the 0x91/act header at +0/+1).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34c70(uint32_t* r) {
    const D2GameBatch20Env& env = d2game_env(r);
    void* pPlayerIntro = (void*)(uintptr_t)r[0];
    const int32_t nCount = (int32_t)r[1];
    const uint32_t* pNpcIds = (const uint32_t*)(uintptr_t)r[2];
    uint8_t* packet = (uint8_t*)(uintptr_t)r[3];
    void* pBuffer = d2game_intro_bits(pPlayerIntro);

    std::memset(packet + 2, 0xFF, 24);
    uint8_t* out = packet + 2;
    uint32_t result = 0;
    for (int32_t i = 0; i < nCount; ++i) {
        const uint32_t nIndex = d2game_npc_index((int32_t)pNpcIds[i]);
        if (env.getBitState(pBuffer, nIndex) != 0) {
            wr16(out, 0, (uint16_t)pNpcIds[i]);
            out += 2;
            result = 1;
        }
    }
    r[4] = result;
}

// 1.13c D2Game+0x34D00
// Set the NPC-intro flags of the player's intro record for one act: each NPC id is mapped to its bit
// index (missing ids set bit 0) and the bit is set in the record's flag buffer. r[0]=unit (EAX),
// r[1]=pGame, r[2]=npc id array, r[3]=count (EDI).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34d00(uint32_t* r) {
    const D2GameBatch20Env& env = d2game_env(r);
    void* unit = (void*)(uintptr_t)r[0];
    void* pGame = (void*)(uintptr_t)r[1];
    const uint32_t* pNpcIds = (const uint32_t*)(uintptr_t)r[2];
    const int32_t nCount = (int32_t)r[3];

    void* pPlayerData = env.getPlayerData(unit);
    void* pPlayerIntro = rdptr(pPlayerData, 0x60 + (uint32_t)rd8(pGame, 0x6D) * 4);
    void* pBuffer = d2game_intro_bits(pPlayerIntro);
    for (int32_t i = 0; i < nCount; ++i) {
        env.setBitState(pBuffer, d2game_npc_index((int32_t)pNpcIds[i]));
    }
    r[4] = 0;
}
}
