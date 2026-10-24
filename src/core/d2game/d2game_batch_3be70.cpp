// src/core/d2game/d2game_batch_3be70.cpp -- twelve 1.13c D2Game.dll missile-callback / skill-start
// functions (RVA 0x3BE70-0x3F890) transcribed from the cached Ghidra decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Units are opaque host records addressed by retail byte
// offsets; the Fog/D2Common imports and not-yet-mapped D2Game callees are host seams on D2GameBatch3beEnv
// in retail argument order. Register-only arguments the decompile shows as unaff_*/extraout_* are
// explicit trailing parameters. Where Ghidra's stack-struct base is ambiguous the struct starts at the
// first zeroed/typed dword (the 23-dword missile-create request; word 0 is the request type).
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Game/src/MISSILES (MissMode.cpp).
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include "MISSILES/MissMode.h"
#include "GAME/Event.h"
#include "UNIT/SUnit.h"
#include "D2StatList.h"
#include "D2States.h"
#include "D2Inventory.h"
#include "D2Dungeon.h"
#include "../fn/hookenv.h"
BOOL D2Common_10057_DUNGEON_IsRoomInTown(D2ActiveRoomStrc* pRoom);   // 1.13c D2Common+0x3C390 (d2common_batch_31940.cpp)

namespace {
inline int32_t i32(const void* p, uint32_t o) { int32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline uint32_t u32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void* ptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
inline int16_t i16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
[[noreturn]] void fatal(uint32_t) { _exit(-1); }
}

struct D2GameBatch3beEnv {
    const uint8_t* dt;                                          // *sgptDataTables
    uint32_t (*unitClass)(void* u);                             // Ordinal 10240 (skill/class id)
    uint32_t (*unitLevel)(void* u);                             // Ordinal 11029
    uint32_t (*unitGuid)(void* u);                              // Ordinal 11018
    void*    (*getClient)(void* game, uint32_t clientId);       // 0x400003A0-ish FUN_6fd003a0
    int32_t  (*unitX)(void* u);                                 // 0x21100
    int32_t  (*unitY)(void* u);                                 // 0x21130
    int32_t  (*skillDuration)(void* client, uint32_t a, uint32_t id, uint32_t lvl); // Ordinal 10786
    void*    (*statsRow)(void* u);                              // 0x21420
    void*    (*createMissile)(void* game, uint32_t* req);       // 0x8F930
    void     (*setLife)(void* unit, int32_t v);                 // Ordinal 10637
    void     (*f3bda0)(void* game);                             // 0x3BDA0
    void     (*f6cc26e0)(void* edi, int32_t dur);               // 0xCC26E0
    void*    (*findTarget)(uint32_t a, int32_t b, uint32_t c, void* client, int32_t x, int32_t y); // 0xCC0F40
    void*    (*getRoom)(void* u);                               // Ordinal 10331
    int32_t  (*pathX)(void* path);                              // Ordinal 10867
    int32_t  (*pathY)(void* path);                              // Ordinal 10750
    int32_t  (*roomCollide)(void* room, const int32_t* rect, const int32_t* rect2, int32_t mask); // 11076
    void*    (*roomAux)(void* room);                            // 0x21280
    int32_t  (*auxTest)(void* aux);                             // Ordinal 10057
    void*    (*createUnit)(void* gameField, int32_t type, uint32_t id, uint32_t a, uint32_t b); // 11013
    void     (*f6caea80)(void* game, void* unit, int32_t a, uint32_t b, int32_t c, int32_t d, int32_t e);
    void     (*setMode)(void* u, int32_t v);                    // 10785
    void     (*setCallback)(void* u, const void* cb);           // 10911
    void     (*attach)(void* a, void* b, int32_t v);            // 10807
    void     (*setFlag)(void* u, int32_t a, int32_t b);         // 10945
    const void* cb58e60;                                        // LAB_6fc58e60
    int32_t  (*dist2)(int32_t x1, int32_t y1, int32_t x2, int32_t y2); // Ordinal 10769
    int32_t  (*f21b10)(uint32_t ecx, uint32_t edi);
    void     (*f21ec0)(void);
    void     (*fd2120)(void* esi);                              // 0xCD2120
    int32_t  (*isDead)(void* t);                                // Ordinal 10494
    int32_t  (*stat1)(void* a);                                 // Ordinal 10973 (1 arg)
    int32_t  (*stat3)(void* a, int32_t b, int32_t c);           // Ordinal 10973 (3 args)
    int32_t  (*f21bc0)(void);
    void     (*f21a90)(uint32_t ecx, int32_t v);
    int32_t  (*unitMode)(void* u);                              // Ordinal 10737
    void     (*fcbf340)(void);
    int32_t  (*frame)(void);                                    // Ordinal 11018, no args
    int32_t  (*maxFrames)(void* c, uint32_t id, uint32_t lvl, int32_t v); // Ordinal 10510
    void     (*f6663a0)(void* unit, void* client, int32_t v);
    int32_t  (*dir)(void* u);                                   // Ordinal 10985
    int32_t  (*dirPacked)(void* u);                             // Ordinal 10818
    void     (*r10614)(void);
    void     (*setPos)(void* path, int32_t x, int32_t y);       // Ordinal 10286
    void     (*moveTo)(void* path, void* u, int32_t v);         // Ordinal 10334
    uint32_t (*r10513)(void);
    void     (*setDir)(void* u, int32_t packed);                // Ordinal 10019
    int32_t  (*collide)(void* game, void* unit);                // 0x3F320 MISSMODE_HandleMissileCollision
};

// Default env: seams whose callee is a DEFINED typed function in src/ are direct calls (D2Common ordinal ->
// ordinal column). Null seams are the documented gaps (no usable definition):
//   createMissile 0x6F930 (template over a hook env only), f3bda0 (39e30 env-typed), f6cc26e0 (0xA26E0 env-typed), findTarget 0xA0F40
//   (arg order unverified), statsRow/skillDuration/maxFrames/ord10786/10510 (probe or env-ABI definitions), roomCollide #11076,
//   roomAux 0x1280, pathX/pathY #10867/#10750 (register-array probes), f21b10/f21ec0/f21bc0/f21a90 (probes), fd2120 0xB2120,
//   isDead #10494 (probe), stat1/stat3 #10973 (2-arg row probe), fcbf340 0x9F340 (stack-arg thunk), r10614, r10513, cb58e60 (LAB_6fc58e60 has no row).
// getClient is SUNIT_GetServerUnit(game, UNIT_PLAYER, id): the decompile drops the type register (player owner id, unit+0x98 / +0x26 dword).
static const D2GameBatch3beEnv g3beDirect = [] {
    D2GameBatch3beEnv e{};
    e.unitClass = [](void* u) -> uint32_t { return (uint32_t)MISSILE_GetSkill((D2UnitStrc*)u); };
    e.unitLevel = [](void* u) -> uint32_t { return MISSILE_GetLevel((D2UnitStrc*)u); };
    e.unitGuid  = [](void* u) -> uint32_t { return (uint32_t)MISSILE_GetTargetX((D2UnitStrc*)u); };
    e.getClient = [](void* g, uint32_t id) -> void* { return SUNIT_GetServerUnit((D2GameStrc*)g, UNIT_PLAYER, (int32_t)id); };
    e.unitX = [](void* u) -> int32_t { return UNITS_GetXPosition((D2UnitStrc*)u); };
    e.unitY = [](void* u) -> int32_t { return UNITS_GetYPosition((D2UnitStrc*)u); };
    e.setLife = [](void* u, int32_t v) { MISSILE_SetTargetX((D2UnitStrc*)u, v); };
    e.getRoom = [](void* u) -> void* { return UNITS_GetRoom((D2UnitStrc*)u); };
    e.auxTest = [](void* r) -> int32_t { return D2Common_10057_DUNGEON_IsRoomInTown((D2ActiveRoomStrc*)r); };
    e.createUnit = [](void* pool, int32_t t, uint32_t id, uint32_t a, uint32_t b) -> void* {
        return STATLIST_AllocStatList(pool, (uint32_t)t, id, (int)a, (D2UnitGUID)b); };
    e.f6caea80 = [](void* g, void* u, int32_t a, uint32_t b, int32_t c, int32_t d, int32_t x) {
        D2GAME_InitTimer_6FC351D0((D2GameStrc*)g, (D2UnitStrc*)u, (D2C_EventTypes)a, (int32_t)b, (EventTimerCallback)(intptr_t)c, d, x); };
    e.setMode = [](void* s, int32_t v) { STATLIST_SetState((D2StatListStrc*)s, v); };
    e.setCallback = [](void* s, const void* cb) { STATLIST_SetStatRemoveCallback((D2StatListStrc*)s, (StatListRemoveCallback)cb); };
    e.attach = [](void* u, void* s, int32_t v) { D2COMMON_10475_PostStatToStatList((D2UnitStrc*)u, (D2StatListStrc*)s, v); };
    e.setFlag = [](void* u, int32_t st, int32_t v) { STATES_ToggleState((D2UnitStrc*)u, st, v); };
    e.dist2 = [](int32_t a, int32_t b, int32_t c, int32_t d) -> int32_t { return PATH_ComputeSquaredDistance(a, b, c, d); };
    e.unitMode = [](void* u) -> int32_t { return MISSILE_GetCurrentFrame((D2UnitStrc*)u); };
    e.dir = [](void* u) -> int32_t { return MISSILE_GetRemainingFrames((D2UnitStrc*)u); };
    e.dirPacked = [](void* u) -> int32_t { return MISSILE_GetTargetY((D2UnitStrc*)u); };
    e.setDir = [](void* u, int32_t v) { MISSILE_SetTargetY((D2UnitStrc*)u, v); };
    e.collide = [](void* g, void* u) -> int32_t { return MISSMODE_HandleMissileCollision((D2GameStrc*)g, (D2UnitStrc*)u); };
    return e;
}();

namespace {
const uint8_t* dtab(const D2GameBatch3beEnv* e) { return e->dt ? e->dt : (const uint8_t*)sgptDataTables; }
// class table row (sgptDataTables+0xB64, count +0xB6C, stride 0x1A4) or null.
const uint8_t* classRow(const D2GameBatch3beEnv* e, const int32_t* u) {
    int32_t i = u[1];
    if (i < 0 || i >= i32(dtab(e), 0xb6c)) return nullptr;
    return (const uint8_t*)ptr(dtab(e), 0xb64) + (size_t)i * 0x1a4;
}
// skills table row (+0xB98, count +0xBA0, stride 0x23C) or null.
const uint8_t* skillRow(const D2GameBatch3beEnv* e, int32_t i) {
    if (i < 0 || i >= i32(dtab(e), 0xba0)) return nullptr;
    return (const uint8_t*)ptr(dtab(e), 0xb98) + (size_t)i * 0x23c;
}
bool isPlayerBit(const int32_t* u) { return ((uint32_t)u[0x32] >> 10 & 1) != 0; }
bool hasPath(const int32_t* u) { int t = u[0]; return t != 2 && (t < 4 || t > 5) && u[0xb] != 0; }
}

// 1.13c D2Game+0x3BE70
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3be70(void* game, const int32_t* u, int32_t flag, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (u && classRow(e, u) && flag == 0 && e->unitGuid((void*)u) == 0)
        e->f3bda0(game);
    return 0;
}

// 1.13c D2Game+0x3C5B0 -- `edi` is the hidden first argument of 0xCC26E0. The request struct the
// decompile builds is never passed anywhere in this function and is omitted.
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3c5b0(void* game, int32_t* u, void* edi, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u) return 1;
    const uint8_t* row = classRow(e, u);
    if (!row || i16(row, 0x24) < 0) return 1;
    if (!isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x26]);
    if (!client) return 1;
    int32_t dur = (int32_t)(e->unitLevel(u) - 1) * i32(row, 0x50) + i32(row, 0x4c);
    if (dur < 1) {
        uint32_t id = e->unitClass(u), lvl = e->unitLevel(u);
        void* st = e->statsRow(u);   // hidden argument of 0x21420 assumed to be the unit
        if (!st) return 1;
        dur = e->skillDuration(client, u32(st, 0x144), id, lvl);
        if (dur < 1) dur = 1;
    }
    e->f6cc26e0(edi, dur);
    return 1;
}

// 1.13c D2Game+0x3C990 -- the request's first argument is the game (Ghidra shows a stale register).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3c990(void* game, int32_t* u, const uint8_t* urow, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u) return 1;
    const uint8_t* row = classRow(e, u);
    if (!row || i16(row, 0x24) < 0 || !isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x26]);
    if (!client) return 1;
    int32_t id = (int32_t)e->unitClass(u);
    uint32_t lvl = e->unitLevel(u);
    const uint8_t* srow = skillRow(e, id);
    if (!srow) return 1;
    int32_t dur = i32(urow, 0x4c);   // urow = unaff_EDI
    if (dur < 1) {
        dur = e->skillDuration(client, u32(srow, 0x138), id, lvl);
        if (dur < 5) dur = 5;
    }
    uint32_t req[23] = {};
    req[0] = 0x8001; req[1] = (uint32_t)(uintptr_t)client;
    req[4] = (uint32_t)(int32_t)i16(urow, 0x24);
    req[5] = e->unitX(u); req[6] = e->unitY(u);
    req[11] = id; req[12] = lvl; req[19] = dur;
    e->createMissile(game, req);
    return 1;
}

// 1.13c D2Game+0x3CDD0
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3cdd0(void* game, int32_t* u, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u) return 1;
    const uint8_t* row = classRow(e, u);
    if (!row || i16(row, 0x24) < 0 || !isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x26]);
    if (!client) return 1;
    uint32_t req[23] = {};
    req[0] = 1; req[1] = (uint32_t)(uintptr_t)client;
    req[5] = e->unitX(u); req[6] = e->unitY(u);
    e->unitClass(u);   // #10240 is called (retail 0x3CE5x) and its result overwritten by #11029
    req[10] = e->unitLevel(u);
    req[2] = (uint32_t)(int32_t)i16(row, 0x24);
    e->createMissile(game, req);
    return 0;
}

// 1.13c D2Game+0x3D020 -- `ctx` is the hidden stack argument (its +0x0C feeds the target search).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3d020(void* game, int32_t* u, int32_t flag, int32_t* ctx, uint32_t seed, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u) return 1;
    const uint8_t* row = classRow(e, u);
    if (!row) return 1;
    if (!isPlayerBit(u)) return 3;
    void* client = e->getClient(game, u[0x26]);
    if (!client || !flag || e->unitGuid(u) <= 1) return 3;
    int32_t x = e->unitX(u), y = e->unitY(u);
    int32_t dur = i32(row, 0x4c);
    uint32_t lvl = 0;
    if (dur < 1) {
        uint32_t id = e->unitClass(u); lvl = e->unitLevel(u);
        void* st = e->statsRow(u);
        if (!st) return 3;
        dur = e->skillDuration(client, u32(st, 100), id, lvl);
        if (dur < 1) dur = 1;
    }
    int32_t* t = (int32_t*)e->findTarget((uint32_t)ctx[3], dur, seed, client, x, y);
    if (t && t != ctx) {
        uint32_t req[23] = {};
        req[0] = 0x21; req[1] = (uint32_t)(uintptr_t)client; req[3] = (uint32_t)(uintptr_t)t;
        req[4] = (uint32_t)u[1]; req[5] = x; req[6] = y;
        req[7] = e->unitX(t); req[8] = e->unitY(t);
        req[10] = e->unitLevel(u);
        void* m = e->createMissile(game, req);
        if (m) e->setLife(m, y - 1);
    }
    return 3;
}

// 1.13c D2Game+0x3D450 -- `radius` and `game` are the hidden arguments (Ghidra's unaff_retaddr / uStack_8).
EMSCRIPTEN_KEEPALIVE void d2_game_3d450(uint32_t p1, int32_t* u, int32_t radius, void* game, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!e->getRoom(u)) return;
    uint32_t req[25] = {};
    req[2] = 1;
    uint32_t cid = 0;
    void* client = nullptr;
    if (u && isPlayerBit(u)) client = e->getClient(game, u[0x26]);
    cid = (uint32_t)(uintptr_t)client;
    req[3] = cid; req[4] = p1; req[12] = e->unitLevel(u);
    if (u) req[0] = 0x8001;
    const bool direct = u[0] == 2 || (u[0] > 3 && u[0] < 6);
    void* path = ptr(u, 0x2c);
    int32_t cx = direct ? i32(path, 0xc) : path ? e->pathX(path) : 0;
    int32_t cy = direct ? i32(path, 0x10) : path ? e->pathY(path) : 0;
    for (int32_t dx = -radius; dx <= radius; dx++) {
        for (int32_t dy = -radius; dy <= radius; dy++) {
            void* room = e->getRoom(u);
            if (!room || dx * dx + dy * dy > radius * radius) continue;
            const int32_t x1 = cx + dx, y1 = cy + dy;
            const int32_t p1[2] = {x1, y1};
            const int32_t p2[2] = {x1 + dx, y1 + dy};   // literal: iVar1+iVar11, iVar10+iVar8
            if (e->roomCollide(room, p1, p2, 4) != 0) continue;
            void* aux = e->roomAux(e->getRoom(u));
            if (aux && e->auxTest(aux) == 0) {
                req[19] = (uint32_t)(uintptr_t)u;
                req[5] = x1; req[6] = y1;
                e->createMissile(game, req);
            }
        }
    }
}

// 1.13c D2Game+0x3E590 -- a null row (retail dereferences it) returns.
EMSCRIPTEN_KEEPALIVE void d2_game_3e590(int32_t* u, int32_t* t, const D2GameBatch3beEnv* e = &g3beDirect)
{
    const uint8_t* row = u ? classRow(e, u) : nullptr;
    if (!row) { if (!u) fatal(0xa2a); return; }
    if (row[0x188] == 0 || !t) return;
    void* game = ptr(u, 0x80);
    uint32_t id = row[0x189] + u32(game, 0xa8);
    void* nu = e->createUnit(ptr(game, 0x1c), 2, id, t[0], t[3]);
    if (!nu) return;
    e->f6caea80(game, t, 0xc, id, 0, 0, 0);
    e->setMode(nu, 0x56);
    e->setCallback(nu, e->cb58e60);
    e->attach(t, nu, 1);
    e->setFlag(t, 0x56, 1);
}

// 1.13c D2Game+0x3E740 -- t = stack argument, ecx/edi = hidden args of 0x21B10, esi of 0xCD2120.
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3e740(void* game, int32_t* u, int32_t* t, uint32_t ecx, uint32_t edi, void* esi, const D2GameBatch3beEnv* e = &g3beDirect)
{
    int32_t id = (int32_t)e->unitClass(u);
    e->unitLevel(u);
    if (!skillRow(e, id) || !u || !isPlayerBit(u)) return 1;
    if (!e->getClient(game, u[0x26])) return 1;
    if (!t || t[0] != 1) return 1;
    uint32_t guid = e->unitGuid(u);
    if (!game) fatal(0x2d3);
    int32_t* n = (int32_t*)ptr(game, 0x1920 + (guid & 0x7f) * 4);
    if (!n) return 1;
    while ((uint32_t)n[3] != guid) {
        n = (int32_t*)ptr(n, 0x39 * 4);
        if (!n) return 1;
    }
    int32_t r = e->f21b10(ecx, edi);
    if (e->dist2(e->unitX(n), e->unitY(n), e->unitX(t), e->unitY(t)) < r * r) {
        e->f21ec0();
        e->fd2120(esi);
    }
    return 1;
}

// 1.13c D2Game+0x3EAE0 -- t = stack argument, ecx = hidden argument of 0x21A90.
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3eae0(void* game, int32_t* u, int32_t* t, uint32_t ecx, const D2GameBatch3beEnv* e = &g3beDirect)
{
    int32_t id = (int32_t)e->unitClass(u);
    e->unitLevel(u);
    const uint8_t* srow = skillRow(e, id);
    if (!srow || !u || !isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x26]);
    if (!client) return 1;
    if (t && t[0] == 1) {
        int16_t s = i16(srow, 0x82);
        if (s < 0 || s >= i32(dtab(e), 0xc4)) return 1;
        if (e->isDead(t) == 0) {
            int32_t a = e->stat1(client);
            int32_t b = e->f21bc0();
            int32_t c = e->stat3(t, 0xc, 0);
            if (c < b + a + s) {
                e->f21a90(ecx, 0xc);
                e->fd2120(nullptr);
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0x3F6D0 -- the 10750/10867 results are discarded in the decompile (register inputs of 0xBF340).
EMSCRIPTEN_KEEPALIVE void d2_game_3f6d0(void* game, int32_t* u, const D2GameBatch3beEnv* e = &g3beDirect)
{
    int32_t m = e->unitMode(u);
    if (m >= 2 && m <= 0x18) {
        if (hasPath(u)) e->pathY(ptr(u, 0x2c));
        if (hasPath(u)) e->pathX(ptr(u, 0x2c));
    } else {
        if (m != 1) { e->collide(game, u); return; }
        if (hasPath(u)) e->pathX(ptr(u, 0x2c));
        if (hasPath(u)) e->pathY(ptr(u, 0x2c));
    }
    e->fcbf340();
    e->collide(game, u);
}

// 1.13c D2Game+0x3F7D0 -- esi = game (frame counter at +0xA8).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3f7d0(void* game, int32_t* u, int32_t flag, void* esi, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u || !isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x26]);
    if (!client || !flag) return 1;
    uint32_t id = e->unitClass(u), lvl = e->unitLevel(u);
    int32_t f = e->frame() - i32(esi, 0xa8);
    if (f > 9 && f <= e->maxFrames(client, id, lvl, 1)) {
        e->f6663a0(u, client, f);
        return 2;
    }
    return 1;
}

// 1.13c D2Game+0x3F890
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3f890(void* game, int32_t* u, const D2GameBatch3beEnv* e = &g3beDirect)
{
    const uint8_t* row = u ? classRow(e, u) : nullptr;
    if (!row) return 2;
    int32_t step = i32(row, 0x38);
    if (step < 1) step = 1;
    if (e->dir(u) % step != 0) return (uint32_t)e->collide(game, u);
    uint64_t seed;
    std::memcpy(&seed, &u[8], 8);
    e->unitGuid(u);
    e->r10614();
    uint32_t v = (uint32_t)e->dirPacked(u);
    int32_t a = (int16_t)v, b = (int16_t)(v >> 16);
    seed = (uint64_t)(uint32_t)seed * 0x6ac690c5ull + (uint32_t)u[9];
    std::memcpy(&u[8], &seed, 8);
    int32_t c = (seed & 1) == 0 ? b : -b;
    int32_t d = (seed & 1) == 0 ? -a : a;
    c += a * 4; d += b * 4;
    int32_t sx = (c + ((c >> 31) & 3)) >> 2, sy = (d + ((d >> 31) & 3)) >> 2;
    if (sx == 0) sx = 1;
    if (sy == 0) sy = 1;
    void* path = ptr(u, 0x2c);
    e->setPos(path, e->unitX(u) + sx, e->unitY(u) + sy);
    e->moveTo(path, u, 0);
    e->setLife(u, (int32_t)e->r10513());
    e->setDir(u, sy * 0x10000 + (sx & 0xffff));
    // the decompile passes the unit where the first call passes the game (stale register); game is used.
    return (uint32_t)e->collide(game, u);
}

// function probe seam (src/core/fn/d2game_3cdd0.cpp): callees are HookEnv recorders; dt is the descriptor's data-table blob.
// Retail calls E03A0 with eax = unit+0x94 (owner type), edx = unit+0x98; the vendored getClient fixes the type to player.
static HookEnv* g3cH; static int32_t* g3cU;
uint32_t D2Game_3CDD0_probe(void* game, int32_t* u, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g3cH = &h; g3cU = u;
    D2GameBatch3beEnv e = g3beDirect;
    e.dt = dt;
    e.getClient = [](void* g, uint32_t id) -> void* { return (void*)(uintptr_t)g3cH->h(0xE03A0, (uint32_t)g3cU[0x25], (uint32_t)(uintptr_t)g, id); };
    e.unitX = [](void* u) -> int32_t { return (int32_t)g3cH->h(0x1100, 0, (uint32_t)(uintptr_t)u); };
    e.unitY = [](void* u) -> int32_t { return (int32_t)g3cH->h(0x1130, 0, (uint32_t)(uintptr_t)u); };
    e.unitClass = [](void* u) -> uint32_t { return g3cH->h(0xB26A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.unitLevel = [](void* u) -> uint32_t { return g3cH->h(0xB0FC, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.createMissile = [](void* g, uint32_t* req) -> void* { return (void*)(uintptr_t)g3cH->h(0x6F930, 0, (uint32_t)(uintptr_t)g, g3cH->stk(req)); };
    return d2_game_3cdd0(game, u, &e);
}
