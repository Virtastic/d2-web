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

    // 0xEE7B0: quest-side callbacks and the retail a2q8 static table (see the function comment).
    void     (*fn33F80)();                      // 0x33F80
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
static void d2game_D2D90_113c(void* pGame, void* pEaxUnit, void* pEsiUnit,
                              const D2GameBatchB64Env& env)
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

    const uint8_t nByte = env.ord10020(pEsiUnit);
    const uint32_t uArg = env.ord10584(pEaxUnit, nByte, rd32(pGame, 0x70));
    const uint32_t y = env.unitY(pEsiUnit);                            // 0x1130
    const uint32_t x = env.unitX(pEsiUnit);                            // 0x1100
    // 1.13c passes the caller's file/line text here; it is retail content and is not reproduced.
    env.ord10869("", 0xF7C, pItemRec, x, y, uArg, 0, nByte);
    env.ord10012(pEsiUnit, 0xFF);

    // The disassembly tests the game argument here (always non-null after the 0xf70 assert); the
    // value handed to 0x7A540 is the player-data field +0x9C for player units, else zero.
    uint32_t nValue = 0;
    if (rd32(pEaxUnit, 0x00) == 0) {
        void* pPlayerData = env.ord10920p(pEaxUnit);
        nValue = pPlayerData ? rd32(pPlayerData, 0x9C) : 0;
    }
    env.fn7a540(pEsiUnit, nValue);

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
static uint32_t d2game_EDF60_113c(void* pGame, uint32_t nModulus, uint32_t* pOutX, uint32_t* pOutY,
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

// 1.13c D2Game+0xee260
// EAX = the unit, EDI = the same unit (the 0x232B0 target). Take the unit's class id (+0xC); when it
// indexes the monster-stats table (stride 0x1A8, count +0xA80, base +0xA78) replace it with the
// record's int16 at +0x2. Then classify the resulting id and assign the boss UMod flag through
// 0x232B0(unit, flag): 0x60/0x18/0x5B/0x146..0x14A/0x154..0x157/0x162/0x1B4/0x1CD/0x1F5 -> 0, and
// 0x21C/0x21D/0x21E -> 1; 0xD3 gets both calls in order (0 then 1).
static void d2game_EE260_113c(void* pEaxUnit, void* pEdiUnit, const D2GameBatchB64Env& env)
{
    const int32_t nMonCount = (int32_t)rd32(env.dataTables, 0xA80);
    int32_t nId = (int32_t)rd32(pEaxUnit, 0x0C);
    if (nId >= 0 && nId < nMonCount) {
        void* pRec = (uint8_t*)rdptr(env.dataTables, 0xA78) + (uint32_t)nId * 0x1A8;
        if (pRec != nullptr) nId = (int16_t)rd16(pRec, 0x02);
    }
    if (nId < 0 || nId >= nMonCount) return;

    if (nId > 0x157) {
        switch (nId) {
        case 0x162: case 0x1B4: case 0x1CD: case 0x1F5:
            env.fn232B0(pEdiUnit, 0);
            return;
        case 0x21C: case 0x21D: case 0x21E:
            env.fn232B0(pEdiUnit, 1);
            return;
        default:
            return;
        }
    }
    if (nId > 0x153) { env.fn232B0(pEdiUnit, 0); return; }             // 0x154..0x157
    if (nId < 0x61) {
        if (nId == 0x60 || nId == 0x18 || nId == 0x5B) env.fn232B0(pEdiUnit, 0);
        return;
    }
    if (nId == 0xD3) {
        env.fn232B0(pEdiUnit, 0);
        env.fn232B0(pEdiUnit, 1);
    } else if (nId > 0x145 && nId < 0x14B) {                           // 0x146..0x14A
        env.fn232B0(pEdiUnit, 0);
    }
}

// 1.13c D2Game+0xEE480
// EAX = the unit, param_1 = the owner unit. Look up the unit's monster record (class id at +0xC,
// stride 0x1A8) and read it through #10035. If the record's int16 +0x26 names a valid monster and
// 0x1870() is 0x105: for a player owner, stamp the monster data (+0x28, flags byte +0x08 bit 1 and
// bit 0, owner +0x2C, +0x30=1, unit id +0x28), roll 0x11D0 when +0x2C < +0x2D, and call 0xEE0A0
// once; otherwise assign the owner through 0x20D90 when bit 0x10 is set, compute the spawn count
// (roll 0x11D0 when +0x2C < +0x2D) and repeat: 0xEDDB0 spawns a unit, bit 0x10 re-stamps it through
// 0x20CF0, and the loop index wraps at the +0x28 validity marker.
static void d2game_EE480_113c(void* pEaxUnit, void* pParam1, const D2GameBatchB64Env& env)
{
    const int32_t nMonCount = (int32_t)rd32(env.dataTables, 0xA80);
    const int32_t nClassId = (int32_t)rd32(pEaxUnit, 0x0C);
    if (nClassId < 0 || nClassId >= nMonCount) return;
    void* pRec = (uint8_t*)rdptr(env.dataTables, 0xA78) + (uint32_t)nClassId * 0x1A8;
    if (pRec == nullptr) return;

    uint8_t local4[4] = { 0, 0, 0, 0 };
    env.ord10035(nClassId, 0, local4);
    const uint32_t uUnitId = rd32(pEaxUnit, 0x00);

    const int16_t nRecA = (int16_t)rd16(pRec, 0x26);
    if (nRecA < 0 || nRecA >= nMonCount) return;

    const int32_t nVariant = env.fn1870();
    if (nVariant >= 0 && nVariant < nMonCount && nVariant == 0x105) {
        const int32_t nOwner = pParam1 ? (int32_t)rd32(pParam1, 0x0C) : -1;
        if (pParam1 != nullptr && rd32(pParam1, 0x00) == 1) {
            void* pMonData = rd32(pParam1, 0x14) != 0 ? rdptr((void*)(uintptr_t)rd32(pParam1, 0x14), 0x28) : nullptr;
            if ((env.bitMask14 & rd8(pRec, 0x0C)) != 0) wr8(pMonData, 0x08, rd8(pMonData, 0x08) | 2);
            wr16(pMonData, 0x08, (uint16_t)(rd16(pMonData, 0x08) | 1));
            wr32(pMonData, 0x2C, (uint32_t)nOwner);
            wr32(pMonData, 0x30, 1);
            wr32(pMonData, 0x28, uUnitId);
        }
        if (rd8(pRec, 0x2C) < rd8(pRec, 0x2D)) {
            env.fn11D0((uint8_t*)pParam1 + 0x20);
        }
        env.fnEE0A0(uUnitId, pParam1, (int32_t)(int16_t)rd16(pRec, 0x26), 1);
        return;
    }

    if ((env.bitMask10 & rd8(pRec, 0x0C)) != 0) {
        const int32_t nOwner = pParam1 ? (int32_t)rd32(pParam1, 0x0C) : -1;
        env.fn20D90((uint32_t)nOwner, 1, uUnitId, (uint32_t)nOwner, 1,
                    env.bitMask14 & rd8(pRec, 0x0C));
    }

    int32_t nIndex = 0;
    int32_t nWrap = 0;
    const int16_t nRec28 = (int16_t)rd16(pRec, 0x28);
    if (nRec28 >= 0 && nRec28 < nMonCount) nWrap = 1;

    uint32_t nSpawns = rd8(pRec, 0x2C);
    if (nSpawns < rd8(pRec, 0x2D)) nSpawns += env.fn11D0((uint8_t*)pParam1 + 0x20);

    while ((int32_t)nSpawns > 0) {
        void* pSpawned = env.fnEDDB0(1, pParam1);
        if (pSpawned != nullptr && (env.bitMask10 & rd8(pRec, 0x0C)) != 0) {
            const int32_t nOwner = pParam1 ? (int32_t)rd32(pParam1, 0x0C) : -1;
            if (rd32(pSpawned, 0x00) == 1) {
                void* pMonData = rd32(pSpawned, 0x14) != 0 ? rdptr((void*)(uintptr_t)rd32(pSpawned, 0x14), 0x28) : nullptr;
                wr32(pMonData, 0x2C, (uint32_t)nOwner);
                wr32(pMonData, 0x30, 1);
                wr32(pMonData, 0x28, uUnitId);
            }
            env.fn20CF0();
        }
        nIndex = nIndex + 1;
        if (nWrap < nIndex) nIndex = 0;
        nSpawns = nSpawns - 1;
    }
}

// 1.13c D2Game+0x140B0
// Random item-type selection for a vendor/cube action. param_1 is the unit; #10295 gives the table
// of item-type slots and #10695(param_1 ? param_1[1] : -1) the candidate record. Ghidra left two
// stack slots unnamed; from the frame they are the second and third stack arguments, taken here as
// param_2 (passed to #10536/#10996 and re-assigned over param_1 in 1.13c) and param_3 (masked with
// 0xffff0000 for #11094). The item-type count is *table, forced to 4 when #10711(param_1) is
// non-zero or record+0x113 is set. A random index (power-of-two mask when possible, else modulo) is
// drawn from #10411's RNG state (seed*0x6AC690C5 + high word); visited indices are retried, #11007
// maps index -> item type, and #11094 must accept it before #10536/#10996 commit the choice and
// 0x13DE0 runs. Returns 0 when every index has been exhausted.
static uint32_t d2game_140B0_113c(uint32_t nParam1, uint32_t nParam2, uint32_t nParam3, void* pEdi,
                                  const D2GameBatchB64Env& env)
{
    uint32_t* pSlots = env.ord10295();
    const int32_t nKey = nParam1 ? (int32_t)rd32((void*)(uintptr_t)nParam1, 0x04) : -1;
    void* pRecord = env.ord10695(nKey);
    if (pSlots == nullptr || pRecord == nullptr) return 0;

    uint32_t nCount = *pSlots;
    uint32_t nMasked = nParam3 & 0xFFFF0000u;
    if (env.ord10711((void*)(uintptr_t)nParam1) != 0 || rd8(pRecord, 0x113) != 0) nCount = 4;

    // The retail stack array is bounded by the item-type count; 1024 covers the 1.13c table.
    uint8_t visited[1024] = { 0 };

    for (;;) {
        uint32_t nIndex;
        do {
            uint64_t* pRng = (uint64_t*)env.ord10411((void*)(uintptr_t)nParam1);
            if ((int32_t)nCount < 1) {
                nIndex = 0;
            } else {
                const uint32_t* p = (const uint32_t*)pRng;
                uint64_t u = (uint64_t)p[0] * 0x6AC690C5ull + (uint64_t)p[1];
                *pRng = u;
                if ((nCount & (nCount - 1)) == 0) {
                    nIndex = (nCount - 1) & (uint32_t)u;
                } else {
                    nIndex = (uint32_t)(u % (uint64_t)nCount);
                }
            }
        } while (nIndex < sizeof visited && visited[nIndex] != 0);

        int32_t nItemType = env.ord11007(nIndex);
        if (nItemType == 0) break;
        if (env.ord11094(nMasked, nItemType) != 0) {
            env.ord10536(nParam2, nIndex);
            env.ord10996(1, 0, nParam2, nItemType, 0, 0);
            env.fn13DE0(pEdi);
            return 1;
        }
        if (nIndex < sizeof visited) visited[nIndex] = 1;

        int32_t nScan = 0;
        if ((int32_t)nCount < 1) return 0;
        for (;;) {
            nParam1 = nParam2;  // retail's loop condition reloads param_1 from the second stack arg
            if (!(nScan < (int32_t)sizeof visited && visited[nScan] != 0)) break;
            nScan = nScan + 1;
            if ((int32_t)nCount <= nScan) return 0;
        }
    }
    return 0;
}

// 1.13c D2Game+0x9e330
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9e330(uint32_t* r)
{
    d2game_9E330_113c((int32_t*)(uintptr_t)r[0], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0x9F9A0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9f9a0(uint32_t* r)
{
    d2game_9F9A0_113c((void*)(uintptr_t)r[0], (int32_t)r[1], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0xCA5B0 (r[0]=this, r[1]=param_2, r[2]=unit A, r[3]=level-record table)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ca5b0(uint32_t* r)
{
    d2game_CA5B0_113c((void*)(uintptr_t)r[0], r[1], (void*)(uintptr_t)r[2],
                      (void**)(uintptr_t)r[3], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0x109B0 (r[0]=param_1, r[1]=ESI)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_109b0(uint32_t* r)
{
    d2game_109B0_113c((int32_t*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0xD2D90 (r[0]=game, r[1]=EAX unit, r[2]=ESI unit)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_d2d90(uint32_t* r)
{
    d2game_D2D90_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], (void*)(uintptr_t)r[2],
                      batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0x9F3B0 (r[0]=this, r[1]=param_1, r[2]=missile id, r[3]=EBX, r[6]=EBP, r[4]=result)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9f3b0(uint32_t* r)
{
    r[4] = d2game_9F3B0_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], (int32_t)r[2], r[3],
                             r[6], batchEnv(r));
}

// 1.13c D2Game+0x9FAE0 (r[0]=unit, r[1]=missile id, r[4]=result)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9fae0(uint32_t* r)
{
    r[4] = d2game_9FAE0_113c((void*)(uintptr_t)r[0], (int32_t)r[1], batchEnv(r));
}

// 1.13c D2Game+0xEDF60 (r[0]=game, r[1]=modulus, r[2]=out X, r[3]=out Y, r[4]=result)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_edf60(uint32_t* r)
{
    r[4] = d2game_EDF60_113c((void*)(uintptr_t)r[0], r[1], (uint32_t*)(uintptr_t)r[2],
                             (uint32_t*)(uintptr_t)r[3], batchEnv(r));
}

// 1.13c D2Game+0xEE260 (r[0]=EAX unit, r[1]=EDI unit)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ee260(uint32_t* r)
{
    d2game_EE260_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0xEE480 (r[0]=EAX unit, r[1]=param_1 owner)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ee480(uint32_t* r)
{
    d2game_EE480_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], batchEnv(r));
    r[4] = 0;
}

// 1.13c D2Game+0x140B0 (r[0]=param_1, r[1]=param_2, r[2]=param_3, r[3]=EDI, r[4]=result)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_140b0(uint32_t* r)
{
    r[4] = d2game_140B0_113c(r[0], r[1], r[2], (void*)(uintptr_t)r[3], batchEnv(r));
}

// 1.13c D2Game+0xee7b0
// __fastcall: ECX = param_1 (the unit handed to the callbacks), EAX = a second unit whose id (+0x00)
// and class id (+0x0C) drive the dispatch. The class id is replaced by the monster record's int16
// +0x2 when it indexes the monster-stats table (stride 0x1A8). The switch below reproduces the
// retail boss/quest UMod assignments:
//   - game+0x10F4 is a pointer to the head of a unit-linked list; nodes are matched by their dword
//     +0x00 and walked through +0xF4 (0x3D dwords).
//   - 0x232B0(param_1, flag) assigns a UMod; flags in param_1's dword +0xC4 select quest-side
//     effects (+0x20000, +0x800) and the monster-data byte +0x5C keeps bit 1.
//   - 0x33F80 and 0x2A3C-class callbacks run when a matching node is found; 0x10945 clears the
//     skill; 0x21C (act 2 quest 8) walks a 4-entry table (retail static data, supplied through
//     env->a2q8Table) calling 0x10450 and 0xCCFCE0 per entry.
static void* batchFindUnitOfType(void* pGame, int32_t nType)
{
    void* pList = rdptr(pGame, 0x10F4);
    if (pList == nullptr) return nullptr;
    void* pNode = rdptr(pList, 0x00);
    while (pNode != nullptr) {
        if ((int32_t)rd32(pNode, 0x00) == nType) return pNode;
        pNode = rdptr(pNode, 0xF4);
    }
    return nullptr;
}

static void d2game_EE7B0_113c(void* pEaxUnit, void* pParam1, const D2GameBatchB64Env& env)
{
    const int32_t nMonCount = (int32_t)rd32(env.dataTables, 0xA80);
    // The event record in EAX carries the game at +0x00 and the class id at +0x0C (disassembly);
    // ECX (pParam1) is the unit. The game drives the quest-list at +0x10F4.
    void* pGame = rdptr(pEaxUnit, 0x00);
    const int32_t nClassId = (int32_t)rd32(pEaxUnit, 0x0C);
    int32_t nId = nClassId;
    if (nClassId >= 0 && nClassId < nMonCount) {
        void* pRec = (uint8_t*)rdptr(env.dataTables, 0xA78) + (uint32_t)nClassId * 0x1A8;
        if (pRec != nullptr) nId = (int16_t)rd16(pRec, 0x02);
    }

    // LAB_6fd0e80d: the switch reached for out-of-range ids and for ids below 0x125.
    auto dispatchLow = [&](int32_t id) {
        switch (id) {
        case 0x9C:
            if (nClassId == 0x2C3) {
                env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
                return;
            }
            env.fn232B0(pParam1, 1);
            if (batchFindUnitOfType(pGame, 6) != nullptr) env.fn33F80();
            return;
        case 0xAF:
            env.unitY(pParam1);
            env.unitX(pParam1);
            return;
        case 0xD3:
            if (nClassId == 0x2C4) {
                env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
                return;
            }
            if (batchFindUnitOfType(pGame, 0x0D) != nullptr) env.fn33F80();
            if (batchFindUnitOfType(pGame, 9) != nullptr) env.fn33F80();
            return;
        case 0xE5:
            if (batchFindUnitOfType(pGame, 8) != nullptr) env.fn33F80();
            return;
        case 0xF2:
            if (nClassId == 0x2C0) {
                env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
                env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
                return;
            }
            if (batchFindUnitOfType(pGame, 0x14) != nullptr) env.fn33F80();
            env.fn232B0(pParam1, 1);
            return;
        case 0xF3:
            if (nClassId == 0x2C1) {
                env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
                env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
                return;
            }
            env.fn232B0(pParam1, 1);
            if (batchFindUnitOfType(pGame, 0x17) != nullptr) env.fn33F80();
            return;
        case 0xFA:
            if (batchFindUnitOfType(pGame, 0x0C) != nullptr) env.fn33F80();
            if (pParam1 == nullptr) return;
            wr32(pParam1, 0xC4, rd32(pParam1, 0xC4) | 0x800);
            if (rd32(pParam1, 0x00) != 1) return;
            if (rd32(pParam1, 0x14) == 0) return;
            wr8((void*)(uintptr_t)rd32(pParam1, 0x14), 0x5C,
                (uint8_t)(rd8((void*)(uintptr_t)rd32(pParam1, 0x14), 0x5C) | 1));
            return;
        case 0x100:
            if (nClassId == 0x2C2) {
                env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
                return;
            }
            env.fn232B0(pParam1, 1);
            if (batchFindUnitOfType(pGame, 0x16) != nullptr) env.fn33F80();
            return;
        case 0x10B:
            env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
            if (batchFindUnitOfType(pGame, 2) != nullptr) env.fn33F80();
            // The decompiler shows a single argument on this #10945 call; the seam keeps the
            // three-slot retail signature and passes zeros for the rest.
            env.ord10945(pParam1, 0, 0);
            return;
        case 0x11C:
            env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
            return;
        default:
            return;
        }
    };

    if (nId < 0 || nId >= nMonCount) {
        dispatchLow(-1);
        return;
    }
    if (nId < 0x125) {
        if (nId == 0x124) {
            env.fn232B0(pParam1, 0);
            return;
        }
        dispatchLow(nId);
        return;
    }

    // The 0x125..0x157 range and the larger quest ids.
    switch (nId) {
    case 0x154: case 0x155: case 0x156: case 0x157:
        if (pParam1 != nullptr) wr32(pParam1, 0xC4, rd32(pParam1, 0xC4) | 0x20000);
        return;
    case 0x16E:
        if (batchFindUnitOfType(pGame, 0x13) != nullptr) env.fn33F80();
        if (pParam1 != nullptr) {
            wr32(pParam1, 0xC4, rd32(pParam1, 0xC4) | 0x20000);
            env.fn232B0(pParam1, 1);
            return;
        }
        break;
    case 0x192:
        break;
    case 0x197:
        if (batchFindUnitOfType(pGame, 0x11) != nullptr) env.fn33F80();
        return;
    case 0x199:
        if (batchFindUnitOfType(pGame, 0x18) != nullptr) env.fn33F80();
        return;
    case 0x1B2:
        if (batchFindUnitOfType(pGame, 0x20) != nullptr) env.fn33F80();
        return;
    case 0x20E:
        if (batchFindUnitOfType(pGame, 0x22) != nullptr) {
            env.fn33F80();
            env.fn232B0(pParam1, 1);
            return;
        }
        if (rdptr(pGame, 0x10F4) == nullptr) break;              // no list: fall to 0x232B0
        // A list exists but no matching node: locate it and assign, otherwise fall through.
        {
            void* pList = rdptr(pGame, 0x10F4);
            if (pList == nullptr || rdptr(pList, 0x00) == nullptr) break;
            env.fn232B0(pParam1, 1);
            return;
        }
    case 0x21C: {
        if (pParam1 == nullptr) d2game_fatal(0xA2A);
        const uint32_t nSel = rd32(pParam1, 0x04);
        const uint32_t nArg = rd32(pParam1, 0x80);
        env.ord10973(pParam1, 0x0C, 0);
        // Retail static table addressed past the a2q8 source-path literal, selected by param_1[1]
        // and walked in 0x18-byte strides. The table itself is retail content in env.
        const uint8_t* pEntry = (const uint8_t*)env.a2q8Table + 0x20 + (size_t)nSel * 8;  // a2q8Table = retail literal base
        for (int k = 0; k < 4; ++k) {
            uint32_t local = 0;
            env.ord10450(*(const uint32_t*)(pEntry + (size_t)k * 0x18), &local);
            env.fnCCFCE0((void*)(uintptr_t)nArg);
        }
        return;
    }
    case 0x220:
        if (nClassId == 0x2C5) {
            env.fn232B0(pParam1, 1);
            env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
            env.fn232B0(pParam1, 1); env.fn232B0(pParam1, 1);
            return;
        }
        if (batchFindUnitOfType(pGame, 0x24) != nullptr) {
            env.fn33F80();
            env.fn232B0(pParam1, 1);
            return;
        }
        if (rdptr(pGame, 0x10F4) == nullptr) break;
        {
            void* pList = rdptr(pGame, 0x10F4);
            if (pList == nullptr || rdptr(pList, 0x00) == nullptr) break;
            env.fn232B0(pParam1, 1);
            return;
        }
    default:
        return;
    }
    env.fn232B0(pParam1, 1);
}

// 1.13c D2Game+0xEE7B0 (r[0]=EAX unit, r[1]=param_1)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ee7b0(uint32_t* r)
{
    d2game_EE7B0_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], batchEnv(r));
    r[4] = 0;
}
