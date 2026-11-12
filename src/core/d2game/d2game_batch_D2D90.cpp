// src/core/d2game/d2game_batch_D2D90.cpp -- Stage A batch lane (OBJECTIVE v8.3): implementations for
// eleven 1.13c D2Game.dll rows transcribed from the cached Ghidra decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Implementation + marker only: no spec, no vector.
//
// Host model: retail records are opaque host objects addressed by their retail byte offsets
// (rd8/rd16/rd32/rdptr, wr8/wr32/wrptr). Every callee src/ does not own yet is a seam on
// D2GameBatchB64Env in retail argument order; register-only arguments (Ghidra unaff_ESI/unaff_EBX/
// unaff_EDI/in_EAX, and stack slots Ghidra failed to name) are explicit parameters below. The fatal
// path Fog #10265(id) -> #10024(text) -> _exit(-1) is modelled as d2game_fatal(id): only the exit is
// observable. A probe passes the env pointer in r[5], the retail arguments in r[0..3] (or r[6] where
// noted), and reads the result in r[4] (the retail capture tool/fn/README.md).
//
// Prior art (1.10f): D2MOO src/D2Game/ITEMS/ItemMode.cpp for 0xD2D90 (item removal path),
// src/D2Game/MISSILES/Missiles.cpp for 0x9F3B0/0x9FAE0/0x9F9A0, src/D2Game/GAME/Game.cpp and
// src/D2Game/MONSTER/MonsterSpawn.cpp for the 0xEDF60/0xEE260/0xEE480/0xEE7B0 dispatcher family.
// The 1.13c ordinal numbering differs from 1.10f; the names below are the 1.13c imports reached
// through the thunks (src/core/fn/import_thunk.cpp). Unknown ordinals keep their number as the seam
// name rather than a guessed 1.10f name.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees src/ does not own. The retail data tables (sgptDataTables) are a host
// object in env->dataTables: record counts at +0xBA0/+0xA80, record bases at +0xB98/+0xA78.
struct D2GameBatchB64Env {
    void*    dataTables;
    // gdwBitMasks_exref entries used by the monster/missile records: +0x10, +0x14, +0x18.
    uint32_t bitMask10;
    uint32_t bitMask14;
    uint32_t bitMask18;

    // 0x9E330: three table-scan results (bit-shifted by the caller).
    void     (*ord11008)();
    int32_t  (*ord10567)();
    int32_t  (*ord10297)();
    int32_t  (*ord10653)();

    // 0x9F3B0 / 0x9FAE0 / 0x9F9A0: missile helpers.
    int32_t  (*ord10786)(void* pUnit, uint32_t nParam);
    void     (*fn8E900)(int32_t n);
    void     (*fn8E6F0)(void* pNode);
    void     (*fn8EA80)(void* pThis, void* pParam1, int32_t a, int32_t b, int32_t c, int32_t d, uint32_t e);
    int32_t  (*ord10708)(uint32_t nSkillId);
    void     (*ord10306)(void* pUnit, int32_t n, int32_t b);
    int32_t  (*ord10587)(void* pUnit, int32_t n, int32_t b);
    void     (*ord10551)(void* pUnit, int32_t a, int32_t b, int32_t c);
    int32_t  (*ord10008)(void* pUnit);
    void     (*ord10418)(void* pUnit);
    void     (*fn96fa0)(void* pArg, void* pUnit);
    void     (*ord10920)(void);                 // return in EAX; see 0xD2D90 (declared void-returning there)
    void     (*fn8D5F0)();
    int32_t  (*ord10313)();
    void     (*ord10945)(void* pUnit, int32_t n, int32_t b);
    void     (*ord10056)(void* pUnit);
    void     (*ord10302)(int32_t nMissileId);

    // 0xD2D90: item removal.
    void*    (*ord10646)(void* pA, void* pB);
    uint8_t  (*ord10020)(void* pUnit);
    uint32_t (*ord10584)(void* pA, uint8_t b, uint32_t c);
    void     (*ord10869)(const char* file, uint32_t line, void* pUnit, uint32_t x, uint32_t y,
                         uint32_t a, uint32_t b, uint32_t c);
    void     (*ord10012)(void* pUnit, uint32_t n);
    void*    (*ord10920p)(void* pUnit);         // UNITS_GetPlayerData
    void     (*fn7a540)(void* pUnit, uint32_t value);
    void     (*fnDFC10)(void* pParam, void* pGame, uint32_t nCallback);
    void     (*fnE20A0)(void* pGame, void* pUnit);
    uint32_t (*unitX)(void* pUnit);             // 0x1100
    uint32_t (*unitY)(void* pUnit);             // 0x1130

    // 0xCA5B0.
    int32_t  (*ord10826)(void* param2);

    // 0x109B0.
    void     (*fnE210)();
    void     (*ord10414)(int32_t v);
    void*    (*ord10411)(void* pRng);
    void     (*ord10614)();
    void     (*fnEE90)(int32_t v);
    void     (*ord10875)(int32_t v, int32_t n);
    int16_t  (*ord10718)(int32_t v);
    void     (*fn10600)(int32_t v);
    void     (*fn10550)();

    // 0xEDF60.
    int      (*ord10544)();
    uint32_t (*fn11D0)(void* pRng);
    void     (*ord10561)(void* pGame, int32_t* pCoords);
    uint32_t (*ord10051)(void* pRoom);
    int16_t  (*ord10482)(void* pGame, int32_t x, int32_t y, uint32_t n);
    int16_t  (*ord10573)(void* pGame, int32_t x, int32_t y, int32_t n, uint32_t id);
    // Four (dx,dy) pairs scanned around the candidate position (retail .text table 0x6FD1C278).
    const int32_t (*roomOffsets)[2];

    // 0xEE260 / 0xEE480.
    void     (*fn232B0)(void* pUnit, int32_t nFlag);
    void     (*ord10035)(int32_t nClassId, int32_t n, void* pOut);
    int32_t  (*fn1870)();
    void     (*fnEE0A0)(uint32_t u, void* pParam, int32_t n, int32_t b);
    void     (*fn20D90)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
    void     (*fn20CF0)();
    void*    (*fnEDDB0)(int32_t n, void* pParam);

    // 0xEE7B0 slots (unused: that row lives in d2game_ee7b0.cpp).
    uint32_t (*fn33F80)(void* pUnitEdi, void* pNodeEsi);  // 0x33F80: EDI=unit, ESI=found list node
    void     (*ord10973)(void* pUnit, int32_t a, int32_t b);
    void     (*ord10450)(uint32_t nEntry, void* pOut);
    void     (*fnCCFCE0)(void* pGame);
    const void* a2q8Table;

    // 0x140B0.
    uint32_t* (*ord10295)();
    void*    (*ord10695)(int32_t n);
    int      (*ord10711)(void* p);
    int32_t  (*ord11007)(uint32_t idx);
    int      (*ord11094)(uint32_t a, int32_t b);
    void     (*ord10536)(uint32_t a, uint32_t idx);
    void     (*ord10996)(int, int, uint32_t a, int32_t b, int, int);
    void     (*fn13DE0)(void* pEdi);
};

static const D2GameBatchB64Env kNoEnv{};
static const D2GameBatchB64Env& batchEnv(const uint32_t* r) {
    return r[5] ? *(const D2GameBatchB64Env*)(uintptr_t)r[5] : kNoEnv;
}

// 1.13c D2Game+0x9e330
// Three table lookups; the first two are shifted right by 8 before being stored. param_1 is the
// output array (three int32 at +0x00/+0x04/+0x08).
static void d2game_9E330_113c(int32_t* pOut, const D2GameBatchB64Env& env)
{
    env.ord11008();
    pOut[0] = env.ord10567() >> 8;
    pOut[1] = env.ord10297() >> 8;
    pOut[2] = env.ord10653();
}

// 1.13c D2Game+0x9F9A0
// Missile cleanup: the missile id in EAX must be a valid record index; then #10302(id), #10418(unit),
// and for a missile unit (type 0) the 0x96FA0 hook with the unit's owner field (+0x80).
static void d2game_9F9A0_113c(void* pUnit, int32_t nMissileId, const D2GameBatchB64Env& env)
{
    const int32_t nCount = (int32_t)rd32(env.dataTables, 0xBA0);
    if (nMissileId < 0 || nMissileId >= nCount) return;
    env.ord10302(nMissileId);
    env.ord10418(pUnit);
    if (pUnit != nullptr && rd32(pUnit, 0x00) == 0) {
        env.fn96fa0(rdptr(pUnit, 0x80), pUnit);
    }
}

// 1.13c D2Game+0xca5b0
// Dispatcher entry from the 0xEEF10 table: `this` (ECX) selects an increment of the level record's
// +0x2CC counter through a table indexed by the level id from #10826(param_2); otherwise, for a
// monster unit (type 1), set flag 0x02 in its monster data at +0x5C. The level-record table comes
// from the caller's return-address slot and is passed explicitly here as pLevelRooms.
static void d2game_CA5B0_113c(void* pThis, uint32_t param2, void* pUnit, void** pLevelRooms,
                              const D2GameBatchB64Env& env)
{
    if (pThis == nullptr) {
        const int32_t nLevel = env.ord10826((void*)(uintptr_t)param2);
        int32_t* pCounter = (int32_t*)((uint8_t*)pLevelRooms[nLevel] + 0x2CC);
        *pCounter = *pCounter + 1;
    } else if (pUnit != nullptr && rd32(pUnit, 0x00) == 1 && rdptr(pUnit, 0x14) != nullptr) {
        uint8_t* pFlags = (uint8_t*)rdptr(pUnit, 0x14) + 0x5C;
        *pFlags = (uint8_t)(*pFlags | 2);
    }
}

// 1.13c D2Game+0x109B0
// Skill/state teardown for a unit: run the activation/statlist/state helpers, set +0x30 to 2, then
// branch on #10718's 16-bit result (0 -> 0x10600, else 0x10550). ECX is param_1 (the unit-id slot),
// ESI a second object whose +0x30 is written.
static void d2game_109B0_113c(int32_t* pParam1, void* pEsi, const D2GameBatchB64Env& env)
{
    env.fnE210();
    env.ord10414(*pParam1);
    env.ord10411((void*)(uintptr_t)(uint32_t)*pParam1);
    env.ord10614();
    env.fnEE90(*pParam1);
    env.ord10875(*pParam1, 2);
    wr32(pEsi, 0x30, 2);
    const int32_t n = *pParam1;
    if (env.ord10718(n) == 0) {
        env.fn10600(n);
    } else {
        env.fn10550();
    }
}

// 1.13c D2Game+0xD2D90
// Item-mode removal helper (source line 0xf7c of ITEMS/ItemMode.cpp in 1.13c). Arguments, settled
// against the retail disassembly because the decompiler mis-attributed several registers:
//   game      = first stack argument (EDI)          -- asserts 0xf70
//   eaxUnit   = EAX                                 -- asserts 0xf71
//   esiUnit   = ESI
// Clear UNITFLAG_TARGETABLE (0x2) on esiUnit, resolve the item record (eaxUnit+0x60) through
// #10646 and require it to be esiUnit itself (asserts 0xf77/0xf78). #10584 prepares the item's
// sound/event argument from #10020(esiUnit) and game->0x70, then #10869 traces the item-mode change
// with the unit's X/Y. #10012(esiUnit, 0xFF) despawns the item; for a player-owned unit the player
// data field +0x9C is handed to 0x7A540 (item->mode update), followed by the living-player iteration
// 0xDFC10 with callback 0xE7E0 and SUNIT_RemoveUnit(game, esiUnit).
// Seam table of the 0xD2D90 entry (the FN probe supplies it). The flag (second stack argument) gates
// the 0x7A540 call; ECX is only saved and restored by the function. 0x1130/0x1100 are thiscall on
// the item (ECX), 0x1100 taking the 0x1130 result; 0xA5E6 is handed only the first four arguments.
struct D2GameD2D90Env {
    void*    (*ord10646)(void* pA, void* pB);
    uint32_t (*ord10020)(void* pUnit);
    uint32_t (*ord10584)(void* pA, uint32_t b, uint32_t c);
    uint32_t (*unitY)(void* pUnit, uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*unitX)(void* pUnit, uint32_t y);
    void     (*ord10869)(const char* file, uint32_t line, void* pRec, uint32_t x, uint32_t y, uint32_t a, uint32_t b, uint32_t c);
    void     (*ord10012)(void* pUnit, uint32_t n);
    void*    (*ord10920p)(void* pUnit);
    void     (*fn7a540)(void* pUnit, uint32_t value);
    void     (*fnDFC10)(void* pUnit, void* pGame, uint32_t cb);
    void     (*fnE20A0)(void* pGame, void* pUnit);
};
void d2game_D2D90_113c(void* pGame, void* pEaxUnit, void* pEsiUnit, uint32_t nEcx, uint32_t nFlag,
                       const D2GameD2D90Env& env)
{
    if (pGame == nullptr) d2game_fatal(0xF70);
    if (pEaxUnit == nullptr) d2game_fatal(0xF71);

    if (pEsiUnit != nullptr) {
        wr32(pEsiUnit, 0xC4, rd32(pEsiUnit, 0xC4) & 0xFFFFFFFD);       // &= ~UNITFLAG_TARGETABLE
    }

    void* pItemRec = rdptr(pEaxUnit, 0x60);
    void* pResolved = env.ord10646(pItemRec, pEsiUnit);
    if (pResolved == nullptr) d2game_fatal(0xF77);
    if (pEsiUnit != pResolved) d2game_fatal(0xF78);

    const uint8_t nByte = (uint8_t)env.ord10020(pEsiUnit);
    // retail does `mov cl, al` and pushes ECX: the caller's upper ECX bytes ride along with the byte (0xA634 arg 2, 0x1130 arg 3)
    const uint32_t nMerged = (nEcx & 0xFFFFFF00u) | nByte;
    const uint32_t uArg = env.ord10584(pEaxUnit, nMerged, rd32(pGame, 0x70));
    const uint32_t y = env.unitY(pEsiUnit, uArg, 0, nMerged);          // 0x1130
    const uint32_t x = env.unitX(pEsiUnit, y);                         // 0x1100 takes the 0x1130 result
    // 1.13c passes the caller's file/line text here; it is retail content and is not reproduced.
    env.ord10869("", 0xF7C, pItemRec, x, 0, 0, 0, 0);
    env.ord10012(pEsiUnit, 0xFF);

    // The second stack argument gates the 0x7A540 call; the value is the player-data field +0x9C
    // for player units, else zero (retail does not null-check the player data).
    if (nFlag != 0) {
        uint32_t nValue = 0;
        if (rd32(pEaxUnit, 0x00) == 0) nValue = rd32(env.ord10920p(pEaxUnit), 0x9C);
        env.fn7a540(pEsiUnit, nValue);
    }

    env.fnDFC10(pEsiUnit, pGame, 0xE7E0);                              // EDI=game, EBX=callback RVA
    env.fnE20A0(pGame, pEsiUnit);
}

// 1.13c D2Game+0x9f3b0
// __thiscall: this = the game, param_1 = the caster. EAX = missile id, EBX/EBP preserved as
// arguments below (EBX selects the chain-walk branch; EBP becomes its high argument).
// Reject an invalid missile record unless one of the record's two update bitmasks (0x18/0x14)
// matches byte 4. #10786 clamps the count to at least 5; then either 0x8E900(8) or walk the
// caster's +0xDC node chain (type 8, field +0x14 == -1) through 0x8E6F0, clearing EAX/EBP. Finally
// 0x8EA80 schedules the update with the rounded count.
static uint32_t d2game_9F3B0_113c(void* pThis, void* pParam1, int32_t nMissileId, uint32_t nEbx,
                                  uint32_t nEbp, const D2GameBatchB64Env& env)
{
    const int32_t nCount = (int32_t)rd32(env.dataTables, 0xBA0);
    if (nMissileId < 0 || nMissileId >= nCount) return 0;
    void* pRec = (uint8_t*)rdptr(env.dataTables, 0xB98) + (uint32_t)nMissileId * 0x23C;
    if (pRec == nullptr) return 0;

    const uint8_t nRecFlags = rd8(pRec, 0x04);
    if ((env.bitMask18 & nRecFlags) == 0 && (env.bitMask14 & nRecFlags) == 0) return 0;

    int32_t nCountArg = env.ord10786(pParam1, rd32(pRec, 0x128));
    if (nCountArg < 6) nCountArg = 5;
    const int32_t nSpan = (int32_t)rd32(pThis, 0xA8);

    int32_t nEax = nMissileId;
    uint32_t nHigh = nEbp;
    if (nEbx == 0) {
        env.fn8E900(8);
    } else {
        if (pParam1 != nullptr) {
            void* pNode = rdptr(pParam1, 0xDC);
            while (pNode != nullptr) {
                void* pNext = rdptr(pNode, 0x24);
                if (rd8(pNode, 0x00) == 8 && rd32(pNode, 0x14) == 0xFFFFFFFF) env.fn8E6F0(pNode);
                pNode = pNext;
            }
        }
        nHigh = 0;
        nEax = -1;
    }
    env.fn8EA80(pThis, pParam1, 8, ((nSpan - 1 + nCountArg) / nCountArg) * nCountArg + 1, 0,
                nEax, nHigh);
    return 1;
}

// 1.13c D2Game+0x9fae0
// __fastcall: ECX = param_1 (the unit), EAX = the missile id. Valid missile record required. If the
// record names a skill (+0x170), remove/learn it through #10708/#10306 and convert the missile
// count with #10786; otherwise the count starts at 1. Only when the unit's stat 5 (mana) is at
// least that count: spend it (#10551), #10008/#10418 clean up the skill and the 0x96FA0 hook runs
// for missile units, then #10306 restores the saved skill, #10920 refreshes player data, 0x8D5F0
// and #10313/#10945 release the charge, and #10056 finalises. Otherwise refund (#10551) and fail.
static uint32_t d2game_9FAE0_113c(void* pUnit, int32_t nMissileId, const D2GameBatchB64Env& env)
{
    const int32_t nCount = (int32_t)rd32(env.dataTables, 0xBA0);
    if (nMissileId < 0 || nMissileId >= nCount) return 0;
    void* pRec = (uint8_t*)rdptr(env.dataTables, 0xB98) + (uint32_t)nMissileId * 0x23C;
    if (pRec == nullptr) return 0;

    int32_t nCost = 1;
    if (rd32(pRec, 0x170) != 0xFFFFFFFF) {
        nCost = env.ord10708(0xFFFFFFFF);
        if (nCost != 0) env.ord10306(pUnit, nCost, 1);
        nCost = env.ord10786(pUnit, rd32(pRec, 0x170));
    }

    const int32_t nMana = env.ord10587(pUnit, 5, 0);
    if (nCost <= nMana) {
        env.ord10551(pUnit, 5, -nCost, 0);
        const int32_t nSaved = env.ord10008(pUnit);
        env.ord10418(pUnit);
        if (pUnit != nullptr && rd32(pUnit, 0x00) == 0) env.fn96fa0(rdptr(pUnit, 0x80), pUnit);
        if (nSaved != 0) {
            env.ord10306(pUnit, nSaved, 0);
            if (pUnit != nullptr && rd32(pUnit, 0x00) == 0) env.ord10920p(pUnit);
            env.fn8D5F0();
            const int32_t n = env.ord10313();
            if (n != 0) env.ord10945(pUnit, n, 1);
            env.ord10056(pUnit);
            return 1;
        }
        // The decompiler shows only three arguments on this refund call; the seam keeps the
        // four-slot retail signature and passes 0 in the fourth.
        env.ord10551(pUnit, 5, nCost, 0);
    }
    return 0;
}

// 1.13c D2Game+0xedf60
// Random room probe around a dungeon room: after the #10544 gate, seed the RNG at game+0x6C through
// 0x11D0 (0 -> 1), read the room coordinates with #10561 and walk the room slots (stride 0x30, room
// pointer at +0x18) starting at a random index, wrapping modulo ESI. A room whose #10051 result has
// bit 2 set yields a candidate position (5*(room coords + room->+0x8/+0xC) + 3); #10482 must accept
// it, and the four (dx,dy) pairs around it must each fail #10573 with id 0x1C09. The first accepted
// pair is written to outX/outY.
[[maybe_unused]] static uint32_t d2game_EDF60_113c(void* pGame, uint32_t nModulus, uint32_t* pOutX, uint32_t* pOutY,
                                  const D2GameBatchB64Env& env)
{
    if (env.ord10544() == 0) return 0;

    void* pSeed = pGame ? (uint8_t*)pGame + 0x6C : nullptr;
    uint32_t uVar4 = env.fn11D0(pSeed);
    if (uVar4 == 0) uVar4 = 1;

    int32_t coords[4] = { 0, 0, 0, 0 };
    env.ord10561(pGame, coords);

    const uint32_t uStartMinus1 = uVar4 - 1;
    if (uVar4 != uStartMinus1) {
        do {
            const uint32_t nSlot = uVar4;
            void* pEntry = (uint8_t*)pGame + uVar4 * 0x30;
            void* pRoom = rdptr(pEntry, 0x18);
            if (pRoom != nullptr && (env.ord10051(pRoom) & 2) != 0) {
                const int32_t y = (int32_t)(rd32(pEntry, 0x0C) + (uint32_t)coords[3]) * 5 + 3;
                const int32_t x = (int32_t)(rd32(pEntry, 0x08) + (uint32_t)coords[2]) * 5 + 3;
                if (env.ord10482(pGame, x, y, 0x100) == 0) {
                    for (int k = 0; k < 4; ++k) {
                        const int32_t dx = env.roomOffsets[k][0];
                        const int32_t dy = env.roomOffsets[k][1];
                        if (env.ord10573(pGame, dx + x, dy + y, 2, 0x1C09) == 0) {
                            *pOutX = (uint32_t)x;
                            *pOutY = (uint32_t)y;
                            return 1;
                        }
                    }
                }
            }
            uVar4 = (nSlot + 1) % nModulus;
        } while (uVar4 != uStartMinus1);
    }
    return 0;
}

// 1.13c D2Game+0x9e330
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_old_9e330(uint32_t* r)
{
    d2game_9E330_113c((int32_t*)(uintptr_t)r[0], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0x9F9A0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_old_9f9a0(uint32_t* r)
{
    d2game_9F9A0_113c((void*)(uintptr_t)r[0], (int32_t)r[1], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0xCA5B0 (r[0]=this, r[1]=param_2, r[2]=unit A, r[3]=level-record table)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_old_ca5b0(uint32_t* r)
{
    d2game_CA5B0_113c((void*)(uintptr_t)r[0], r[1], (void*)(uintptr_t)r[2],
                      (void**)(uintptr_t)r[3], batchEnv(r));
    r[4] = 0;
}


// 1.13c D2Game+0x9F3B0 (r[0]=this, r[1]=param_1, r[2]=missile id, r[3]=EBX, r[6]=EBP, r[4]=result)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_old_9f3b0(uint32_t* r)
{
    r[4] = d2game_9F3B0_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], (int32_t)r[2], r[3],
                             r[6], batchEnv(r));
}

// 1.13c D2Game+0x9FAE0 (r[0]=unit, r[1]=missile id, r[4]=result)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_old_9fae0(uint32_t* r)
{
    r[4] = d2game_9FAE0_113c((void*)(uintptr_t)r[0], (int32_t)r[1], batchEnv(r));
}

// The 0xEDF60 entry point is d2g_edf60 in d2game_edf60.cpp.
