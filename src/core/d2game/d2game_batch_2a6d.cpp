// src/core/d2game/d2game_batch_2a6d.cpp -- ten 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: eight MSVC 2003 Release
// CRT routines the linker pulled into the DLL (sprintf, the process-exit path, __initterm, the doexit
// SEH fragment and __lock, __SEH_prolog/__SEH_epilog) and two GAME.cpp unit-list helpers (the party
// packet 0x8D sender and the pet-list teardown). Stage A batch lane (OBJECTIVE v9): implementation +
// marker only, no spec, no vector.
//
// Host model: retail's 32-bit module globals are the statics below (retail address noted); a unit, pet
// list or client record is opaque and read/written through retail byte offsets. Register-only arguments
// (Ghidra's in_EAX/in_ECX/unaff_EBX/unaff_ESI/unaff_EDI and the injected stack frame operands) are
// explicit parameters here and are named in each function's comment. Callees outside this batch are seam
// function pointers on D2GameBatch2a6dEnv, in retail argument order. The fatal path is Fog Ordinal
// 10265(id) -> Ordinal 10024(text) -> _exit(-1); only the exit is observable and is modelled.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Game/src/PLAYER/PlayerPets.cpp (sub_6FC7CDC0) and
// GAME/SCmd.cpp (D2GAME_PACKETS_SendPacket0x8D_6FC3F960 / ...0x9B_6FC3FB30); the CRT routines are part
// of the compiler runtime, cross-checked against src/core/d2client/d2client_batch_3664.cpp.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdarg>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>      // CRITICAL_SECTION no-ops
#include <D2BasicTypes.h>
#include <Server.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// The CRT FILE descriptor _sprintf fills before handing it to _output: VS2003 layout, the fields the
// decompile touches.
struct CrtFileStrc {
    uint32_t _ptr;    // 0x00
    int32_t  _cnt;    // 0x04
    uint32_t _base;   // 0x08
    uint32_t _flag;   // 0x0C
};

// Host seams for the callees the cached decompiles name but this batch does not implement. Comments name
// the retail 1.13c RVA (and import ordinal where it is one).
struct D2GameBatch2a6dEnv {
    // CRT glue.
    void     (*invoke)(uint32_t entry);                                  // 0x28F7 `call eax` target
    void*    (*getModuleHandle)(const char* name);                       // kernel32 GetModuleHandleA
    void*    (*getProcAddress)(void* module, const char* name);          // kernel32 GetProcAddress
    void     (*exitProcess)(uint32_t code);                              // kernel32 ExitProcess
    int32_t  (*mtInitLockNum)(int32_t file);                             // 0x3F8B __mtinitlocknum
    void     (*amsgExit)(int32_t code);                                  // 0x3820 __amsg_exit
    int32_t  (*output)(CrtFileStrc* file, const uint8_t* format, va_list args); // 0x43E0 _output
    int32_t  (*flsbuf)(int32_t ch, CrtFileStrc* file);                   // 0x4239 __flsbuf
    // GAME.cpp helpers.
    void     (*freePet)(void* game, uint32_t petGuid);                   // 0x977E0 (ECX game, EDX guid)
    void     (*iterateLivingPlayers)(void* game, uint32_t callback, void* arg); // 0xFCFFC10 (EDI game, EBX callback)
    uint32_t (*allocDrlgDelete)(void* unit, uint32_t unitType, uint32_t unitId); // 0x2A478, D2Common Ordinal 10331
    void     (*ord10885)(uint32_t value);                                // 0x2A44E, D2Common Ordinal 10885
    void*    (*getRoom)(void* unit);                                     // 0x2A478, D2Common Ordinal 10331 (1-arg form)
    void     (*modeInit)(void* unit, uint32_t mode);                     // 0x2A496, D2Common Ordinal 11103
    void*    (*getOwner)(void* game, uint32_t arg2, uint32_t arg3);      // 0xE03A0 SUNIT_GetOwner (3 args in 1.13c)
    void     (*modeChange)(void* game, const void* modeChange, uint32_t arg3); // 0x75E10 (1.10f D2GAME_ModeChange)
    void*    (*getPlayerData)(void* player);                             // 0x2A4C0, D2Common Ordinal 10920
    uint32_t (*hirelingCost)(void* unit);                                // 0x2AC22, D2Common Ordinal 11097
    void     (*appendPacket)(void* client, const void* packet, uint32_t len); // 0x8A3E0 (client in EAX/ESI)
    void     (*removeUnit)(void* game, void* unit);                      // 0xE20A0 SUNIT_RemoveUnit
    void     (*freePool)(void* pool, void* mem, const char* file, uint32_t line, uint32_t reserved); // 0x2A2EC, Ordinal 10046
    uint16_t (*getPartyId)(void* unit, void* game);                      // 0x9BEB0 (EAX unit, EDI game)
};

// ---- module state --------------------------------------------------------------------------------
// 0x6FD1F210: CRT lock table, 8-byte CRITICAL_SECTION stride (the decompile indexes it as dwords).
static uint8_t  g_crtLockTable[0x800];
static uint32_t g_exceptionList;     // FS:[0] SEH chain head (single-threaded here)

static const char kPlayerPetsFile[] = "..\\Source\\D2Game\\PLAYER\\PlayerPets.cpp";  // the free call-site literal

static void leave_crt_lock(uint32_t index) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_crtLockTable + (size_t)index * 8));
}

// 1.13c D2Game+0x2A6D (_sprintf): VS2003 unbounded formatted print. The destination FILE is fixed at
// {_cnt = 0x7FFFFFFF, _flag = 0x42, _base = _ptr = dest} and _output counts what it wrote; the caller's
// last vararg is the va_list start. A NULL destination skips the terminator. The result (the printed
// length) is returned through pResult so the exported shape stays `EMSCRIPTEN_KEEPALIVE void`.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2a6d(char* dest, const char* format, int* pResult,
                                       const D2GameBatch2a6dEnv* env, ...) {
    CrtFileStrc file;
    file._cnt = 0x7FFFFFFF;
    file._flag = 0x42;
    file._base = (uint32_t)(uintptr_t)dest;
    file._ptr = file._base;

    va_list args;
    va_start(args, env);
    const int result = env->output(&file, (const uint8_t*)format, args);
    va_end(args);

    if (dest != nullptr) {
        file._cnt -= 1;
        if (file._cnt < 0) env->flsbuf(0, &file);
        else *(char*)(uintptr_t)file._ptr = '\0';
    }
    *pResult = result;
}

// 1.13c D2Game+0x28B5 (___crtExitProcess): if mscoree.dll is loaded and exports CorExitProcess, run it on
// the code, then ExitProcess(code) -- which does not return. Both host lookups and the exit are seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_28b5(int32_t code, const D2GameBatch2a6dEnv* env) {
    void* module = env->getModuleHandle("mscoree.dll");
    if (module != nullptr) {
        void* corExitProcess = env->getProcAddress(module, "CorExitProcess");
        if (corExitProcess != nullptr)
            ((void (*)(int32_t))(uintptr_t)corExitProcess)(code);
    }
    env->exitProcess((uint32_t)code);
}

// 1.13c D2Game+0x28F7 (__initterm): private CRT convention -- the begin pointer arrives in EAX, the end
// is the stack argument. Walk [begin, end) in ascending dword order and call every non-NULL entry with no
// arguments. Each entry is a retail initialiser address, hence the seam for the call; the walk is the rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_28f7(uint32_t* begin, uint32_t* end, const D2GameBatch2a6dEnv* env) {
    for (uint32_t* p = begin; p < end; ++p)
        if (*p != 0) env->invoke(*p);
}

// 1.13c D2Game+0x2A28: SEH-injected fragment inside doexit -- its operands are doexit's frame slot
// [EBP+0x10] and EDI, not an argument list. If they differ, release CRT lock 8 (0x3F76's inline).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2a28(uint32_t stack10, uint32_t edi) {
    if (stack10 != edi) leave_crt_lock(8);
}

// 1.13c D2Game+0x402B (__lock): lazily initialise the CRT lock for _File (0x3F8B) and abort with message
// 0x11 if that fails, then enter the critical section at table[_File * 2].
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_402b(int32_t file, const D2GameBatch2a6dEnv* env) {
    LPCRITICAL_SECTION lock = (LPCRITICAL_SECTION)(g_crtLockTable + (size_t)(uint32_t)file * 8);
    if (((const uint32_t*)lock)[0] == 0) {
        if (env->mtInitLockNum(file) == 0) env->amsgExit(0x11);
    }
    EnterCriticalSection(lock);
}

// 1.13c D2Game+0x40E4 (__SEH_prolog): compiler-injected prologue. It spills the incoming FS:[0], pushes
// __except_handler3 (0x6FC24138), lays the registration record {prev, handler} at EBP-0x10 and points
// FS:[0] there; the EBX/ESI/EDI/return spill that follows is the host ABI's own store. frame is that
// record (an injected operand of the prologue, not an argument); frameSize is the prologue's stack size.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_40e4(uint32_t* frame, uint32_t frameSize) {
    (void)frameSize;
    frame[0] = g_exceptionList;   // the prologue pushes the incoming FS:[0] first
    frame[1] = 0x6FC24138u;       // __except_handler3 scope table
    g_exceptionList = (uint32_t)(uintptr_t)frame;
}

// 1.13c D2Game+0x411F (__SEH_epilog): compiler-injected epilogue. Restore FS:[0] from [EBP-0x10] (the
// same record the prolog built) and pop the saved return address back over [EBP]; only the chain head is
// engine-visible, the return store is the host ABI's.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_411f(const uint32_t* ebp) {
    g_exceptionList = ebp[-4];
}

// 1.13c D2Game+0x8C570: build and append the 7-byte party packet 0x8D -- {0x8D, dwPlayerGUID,
// nPartyGUID} packed, the GUID at byte 1 and the party id at byte 5. The unit arrives in EAX and the
// packet recipient player in ESI (register-only). A NULL unit yields GUID 0xFFFFFFFF and party 0xFFFF;
// the party lookup (0x9BEB0) needs unit->pGame (+0x80) and a player unit (type 0). The packet goes to the
// recipient player's client unless that player is absent or not a player, in which case the client is
// NULL. Party id from 1.10f SUNIT_GetPartyId, sender from D2GAME_PACKETS_SendPacket0x8D_6FC3F960.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_8c570(void* unit, void* player, const D2GameBatch2a6dEnv* env) {
    uint8_t packet[7] = {};
    packet[0] = 0x8D;

    const uint32_t playerGuid = (unit != nullptr) ? rd32(unit, 0xC) : 0xFFFFFFFFu;
    wr32(packet, 1, playerGuid);

    uint16_t partyId = 0xFFFF;
    if (unit != nullptr) {
        void* game = rdptr(unit, 0x80);
        if (game != nullptr && rd32(unit, 0) == 0)
            partyId = env->getPartyId(unit, game);
    }
    wr16(packet, 5, partyId);

    void* client = nullptr;
    if (player != nullptr && rd32(player, 0) == 0) {
        void* playerData = env->getPlayerData(player);
        client = rdptr(playerData, 0x9C);
    }
    env->appendPacket(client, packet, 7);
}

// 1.13c D2Game+0x97420: tear down one pet list (1.10f sub_6FC7CDC0). pGame is register-only (EAX);
// pPlayer is the argument Ghidra hides as unaff_retaddr; pPetList is the list, bHireable the pet-type
// test and bKeep the keep flag (both retail stack arguments). For every pet data node, unlink it from
// the unit list: the non-hireable path frees the pet by GUID (0x977E0); the hireable path finds the
// server unit by GUID in game->units[(GUID & 0x7F)], synchronises clients, releases the dungeon record,
// then either removes the unit (bKeep == 0) or, once per pet, marks it, initialises the mode-change and
// sends packet 0x9B {0x9B, petInfo.wName, resurrection cost}. Every removed node decrements both list
// counters and is freed back to game->pMemoryPool (+0x1C), logged at PlayerPets.cpp:0xE8.
// PetData: +0 flags, +4 nUnitGUID, +0xC petInfo.wName, +0x14 next. PetList: +0 head, +4 nCurrent, +8 nMax.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_97420(void* game, void* player, void* petList,
                                        int32_t bHireable, int32_t bKeep,
                                        const D2GameBatch2a6dEnv* env) {
    void* petData = rdptr(petList, 0);
    while (true) {
        if (petData == nullptr) {
            if (rd32(petList, 4) == 0) wr32(petList, 0, 0);
            return;
        }

        void* next = rdptr(petData, 0x14);
        const uint32_t petGuid = rd32(petData, 4);

        if (bHireable == 0) {
            env->freePet(game, petGuid);
            goto free_node;
        }

        if (game == nullptr) d2game_fatal(0x2D3);

        for (void* pet = rdptr(game, 0x1320 + (petGuid & 0x7F) * 4); pet != nullptr; pet = rdptr(pet, 0xE4)) {
            if (rd32(pet, 0xC) != petGuid) continue;

            // 1.10f SUNIT_IterateLivingPlayers(game, PLAYERPETS_SynchronizeWithClient, &petArg).
            uint8_t petArg[0x20] = {};
            wr32(petArg, 0, petGuid);
            env->iterateLivingPlayers(game, 0x6FCB6810u, petArg);

            // 1.10f DUNGEON_AllocDrlgDelete(UNITS_GetRoom(pet), UNIT_MONSTER, pet->dwUnitId);
            // the 1.13c callsite passes the unit itself in the first slot.
            env->ord10885(env->allocDrlgDelete(pet, 1, rd32(pet, 0xC)));

            if (bKeep == 0) {
                env->removeUnit(game, pet);
                goto free_node;
            }

            if ((rd32(petData, 0) & 1) == 0) {
                wr32(petData, 0, rd32(petData, 0) | 1);
                wrptr(petList, 0, petData);

                if (env->getRoom(pet) != nullptr) {
                    env->modeInit(pet, 0);

                    uint8_t modeChange[0x20] = {};
                    wr32(modeChange, 0x00, 0);
                    wrptr(modeChange, 0x04, pet);
                    wr8(modeChange, 0x15, 0x64);   // MONMODE_DEATH in 1.13c's packed record
                    if (((rd32(pet, 0xC8) >> 10) & 1) != 0)
                        wr32(modeChange, 0x0C,
                             (uint32_t)(uintptr_t)env->getOwner(game, rd32(pet, 0x94), rd32(pet, 0x98)));
                    env->modeChange(game, modeChange, 1);
                }

                void* client = nullptr;
                if (player != nullptr && rd32(player, 0) == 0) {
                    void* playerData = env->getPlayerData(player);
                    client = rdptr(playerData, 0x9C);
                }

                uint8_t packet[7] = {};
                packet[0] = 0x9B;
                wr16(packet, 1, (uint16_t)rd32(petData, 0xC));
                wr32(packet, 3, env->hirelingCost(pet));
                env->appendPacket(client, packet, 7);
            }
            break;
        }

        if (bKeep == 0) {
        free_node:
            wr32(petList, 4, rd32(petList, 4) - 1);
            wr32(petList, 8, rd32(petList, 8) - 1);
            env->freePool(rdptr(game, 0x1C), petData, kPlayerPetsFile, 0xE8, 0);
        }

        petData = next;
    }
}

// 1.13c D2Game+0x9BD00: fastcall lookup in the game's unit-id map -- the map head is game->[0x1D2C]
// (asserted non-NULL, fatal id 0xD2), its first node is [head+4]; each node is {WORD nUnitId @0,
// next @8}. Return the matching node in *pNode, or NULL when the id is absent.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_9bd00(void* game, uint16_t unitId, void** pNode) {
    const uint32_t mapHead = rd32(game, 0x1D2C);
    if (mapHead == 0) d2game_fatal(0xD2);

    void* node = rdptr((void*)(uintptr_t)mapHead, 4);
    while (node != nullptr) {
        if (rd16(node, 0) == unitId) {
            *pNode = node;
            return;
        }
        node = rdptr(node, 8);
    }
    *pNode = nullptr;
}
