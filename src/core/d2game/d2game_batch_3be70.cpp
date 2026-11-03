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
BOOL D2Common_10057_DUNGEON_IsRoomInTown(D2ActiveRoomStrc* pRoom);   // D2Common 0x3C390 (d2common_batch_31940.cpp)

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
    void*    (*getClient)(void* game, uint32_t type, uint32_t id); // 0xE03A0 (eax = owner type unit+0x94, edx = owner id unit+0x98)
    int32_t  (*unitX)(void* u);                                 // 0x21100
    int32_t  (*unitY)(void* u);                                 // 0x21130
    int32_t  (*skillDuration)(void* client, uint32_t a, uint32_t id, uint32_t lvl); // Ordinal 10786
    void*    (*createMissile)(void* game, uint32_t* req);       // 0x8F930
    void     (*setLife)(void* unit, int32_t v);                 // Ordinal 10637
    void     (*f3bda0)(void* game, int32_t v);                  // 0x3BDA0 (stack game, class row +0x44)
    void     (*f6cc26e0)(void* game, uint32_t* req, int32_t dur); // 0xA26E0 (ecx = game, esi = request, stack dur)
    void*    (*findTarget)(void* game, void* client, int32_t x, int32_t y, int32_t range, uint32_t filter, int32_t a7, int32_t* a8); // 0xA0F40
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
    int32_t  (*f21b10)(int32_t id, int32_t lvl);                // 0x21B10: (lvl-1)*row[0x14c]+row[0x148]
    int32_t  (*f21ec0)(int32_t id);                             // 0x21EC0: row[0x15c]
    void     (*fd2120)(void* t, void* node, void* game, int32_t v); // 0xB2120: esi = target, eax = node/caster, ecx = game, stack value
    int32_t  (*isDead)(void* t, int32_t s);                     // 0xA68E (eax-free: stack unit, stack index)
    int32_t  (*stat3)(void* a, int32_t b, int32_t c);           // Ordinal 10973 (unit, stat, layer)
    int32_t  (*f21bc0)(int32_t id);                             // 0x21BC0: row[0x14c]
    int32_t  (*f21a90)(int32_t id, int32_t lvl);                // 0x21A90: linear [0x158,0x15c]
    const void* cb59210;                                        // LAB_6fc59210 (callback address only)
    int32_t  (*unitMode)(void* u);                              // Ordinal 10737
    void     (*fcbf340)(uint32_t ecx, uint32_t edx, void* game, int32_t y, void* u, int32_t a, int32_t b, uint32_t cbRva); // 0x9F340; cbRva = D2Game RVA of the callback pushed
    int32_t  (*frame)(void);                                    // Ordinal 11018, no args
    int32_t  (*maxFrames)(void* c, uint32_t id, uint32_t lvl, int32_t v); // Ordinal 10510
    void     (*f6663a0)(void* game, void* client, int32_t elapsed, int32_t flag, uint32_t lvl);   // 0x463A0: eax = flag, esi = level, stack game/client/elapsed
    int32_t  (*dir)(void* u);                                   // Ordinal 10985
    int32_t  (*dirPacked)(void* u);                             // Ordinal 10818
    void     (*r10614)(void);
    void     (*setPos)(void* path, int32_t x, int32_t y);       // Ordinal 10286
    void     (*moveTo)(void* path, void* u, int32_t v);         // Ordinal 10334
    uint32_t (*r10513)(void);
    void     (*setDir)(void* u, int32_t packed);                // Ordinal 10019
    int32_t  (*collide)(void* game, void* unit);                // 0x3F320 MISSMODE_HandleMissileCollision
    const uint8_t* (*skillRow1420)(int32_t id);                 // 0x21420 (eax = id): the skills row; null = the inline table lookup (3D020 only)
};

// Default env: seams whose callee is a DEFINED typed function in src/ are direct calls (D2Common ordinal ->
// ordinal column). Null seams are the documented gaps (no usable definition):
//   createMissile 0x6F930 (template over a hook env only), f3bda0 (39e30 env-typed), f6cc26e0 (0xA26E0 env-typed), findTarget 0xA0F40
//   (arg order unverified), skillDuration/maxFrames/ord10786/10510 (probe or env-ABI definitions), roomCollide #11076,
//   roomAux 0x1280, pathX/pathY #10867/#10750 (register-array probes), fd2120 0xB2120,
//   isDead #10494 (probe), stat3 #10973, fcbf340 0x9F340 (stack-arg thunk), r10614, r10513, cb58e60 (LAB_6fc58e60 has no row).
// getClient is SUNIT_GetServerUnit(game, owner type unit+0x94, owner id unit+0x98) (E03A0: eax = type, edx = id).
static const uint8_t* g3beSkillRow(int32_t i) {
    if (i < 0 || i >= i32(sgptDataTables, 0xba0)) return nullptr;
    return (const uint8_t*)ptr(sgptDataTables, 0xb98) + (size_t)i * 0x23c;
}
static const D2GameBatch3beEnv g3beDirect = [] {
    D2GameBatch3beEnv e{};
    e.unitClass = [](void* u) -> uint32_t { return (uint32_t)MISSILE_GetSkill((D2UnitStrc*)u); };
    e.unitLevel = [](void* u) -> uint32_t { return MISSILE_GetLevel((D2UnitStrc*)u); };
    e.unitGuid  = [](void* u) -> uint32_t { return (uint32_t)MISSILE_GetTargetX((D2UnitStrc*)u); };
    e.getClient = [](void* g, uint32_t ty, uint32_t id) -> void* { return SUNIT_GetServerUnit((D2GameStrc*)g, (D2C_UnitTypes)ty, (int32_t)id); };
    e.unitX = [](void* u) -> int32_t { return UNITS_GetXPosition((D2UnitStrc*)u); };
    e.unitY = [](void* u) -> int32_t { return UNITS_GetYPosition((D2UnitStrc*)u); };
    // 0x21B10 / 0x21BC0 / 0x21EC0: skills-row formulas (row +0x148/0x14C/0x15C).
    e.f21b10 = [](int32_t id, int32_t lvl) -> int32_t { const uint8_t* r = g3beSkillRow(id); return r ? (lvl - 1) * i32(r, 0x14c) + i32(r, 0x148) : 0; };
    e.f21bc0 = [](int32_t id) -> int32_t { const uint8_t* r = g3beSkillRow(id); return r ? i32(r, 0x14c) : 0; };
    e.f21ec0 = [](int32_t id) -> int32_t { const uint8_t* r = g3beSkillRow(id); return r ? i32(r, 0x15c) : 0; };
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
    const uint8_t* row;
    if (u && (row = classRow(e, u)) && flag == 0 && e->unitGuid((void*)u) == 0)
        e->f3bda0(game, i32(row, 0x44));
    return 0;
}

// 1.13c D2Game+0x3C5B0 -- ecx = game, edx = unit. Builds the type-3 missile request (callback 0x39210 in req[21]) and
// hands it to 0xA26E0 (ecx = game, esi = request, stack = duration).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3c5b0(void* game, int32_t* u, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u) return 1;
    const uint8_t* row = classRow(e, u);
    if (!row || i16(row, 0x24) < 0) return 1;
    if (!isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x25], u[0x26]);
    if (!client) return 1;
    uint32_t req[23] = {};
    req[0] = 3; req[1] = (uint32_t)(uintptr_t)client;
    req[4] = (uint32_t)(int32_t)i16(row, 0x24);
    req[5] = e->unitX(u); req[6] = e->unitY(u);
    const uint32_t id = e->unitClass(u), lvl = e->unitLevel(u);
    req[11] = id; req[12] = lvl;
    req[21] = (uint32_t)(uintptr_t)e->cb59210;
    int32_t dur = (int32_t)(lvl - 1) * i32(row, 0x50) + i32(row, 0x4c);
    if (dur < 1) {
        const uint8_t* srow = skillRow(e, (int32_t)id);
        if (!srow) return 1;
        dur = e->skillDuration(client, u32(srow, 0x144), id, lvl);
        if (dur < 1) dur = 1;
    }
    e->f6cc26e0(game, req, dur);
    return 1;
}

// 1.13c D2Game+0x3C990 -- ecx = game, edx = unit; the class row (unit+4) also supplies the duration and request word 4.
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3c990(void* game, int32_t* u, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u) return 1;
    const uint8_t* row = classRow(e, u);
    if (!row || i16(row, 0x24) < 0 || !isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x25], u[0x26]);
    if (!client) return 1;
    int32_t id = (int32_t)e->unitClass(u);
    uint32_t lvl = e->unitLevel(u);
    const uint8_t* srow = skillRow(e, id);
    if (!srow) return 1;
    int32_t dur = i32(row, 0x4c);
    if (dur < 1) {
        dur = e->skillDuration(client, u32(srow, 0x138), id, lvl);
        if (dur < 5) dur = 5;
    }
    uint32_t req[23] = {};
    req[0] = 0x8001; req[1] = (uint32_t)(uintptr_t)client;
    req[4] = (uint32_t)(int32_t)i16(row, 0x24);
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
    void* client = e->getClient(game, u[0x25], u[0x26]);
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

// 1.13c D2Game+0x3D020 -- ecx = game, edx = unit, stack ctx (ret 4; ctx+0x0C feeds the target search 0xA0F40).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3d020(void* game, int32_t* u, int32_t* ctx, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u) return 1;
    const uint8_t* row = classRow(e, u);
    if (!row) return 1;
    if (!isPlayerBit(u)) return 3;
    void* client = e->getClient(game, u[0x25], u[0x26]);
    if (!client || !ctx) return 3;
    const uint32_t guid = e->unitGuid(u);
    if ((int32_t)guid <= 1) return 3;
    int32_t x = e->unitX(u), y = e->unitY(u);
    int32_t dur = i32(row, 0x4c);
    if (dur <= 0) {
        uint32_t id = e->unitClass(u);
        uint32_t lvl = e->unitLevel(u);
        const uint8_t* srow = e->skillRow1420 ? e->skillRow1420((int32_t)id) : skillRow(e, (int32_t)id);
        if (!srow) return 3;
        dur = e->skillDuration(client, u32(srow, 100), id, lvl);
        if (dur < 1) dur = 1;
    }
    int32_t* t = (int32_t*)e->findTarget(game, client, x, y, dur, 0x88583, ctx[3], nullptr);
    if (t && t != ctx) {
        uint32_t req[23] = {};
        req[0] = 0x21; req[1] = (uint32_t)(uintptr_t)client; req[3] = (uint32_t)(uintptr_t)t;
        req[4] = (uint32_t)u[1]; req[5] = x; req[6] = y;
        req[7] = e->unitX(t); req[8] = e->unitY(t);
        req[11] = e->unitClass(u); req[12] = e->unitLevel(u);
        void* m = e->createMissile(game, req);
        if (m) e->setLife(m, (int32_t)guid - 1);
    }
    return 3;
}

// 1.13c D2Game+0x3D450 -- five stack arguments (ret 0x14): game, unit, radius, request word 4, duration (0 = no word 19).
EMSCRIPTEN_KEEPALIVE void d2_game_3d450(void* game, int32_t* u, int32_t radius, uint32_t a4, int32_t a5, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (a5 < 0) a5 = 0;
    if (!e->getRoom(u)) return;
    uint32_t req[23] = {};
    void* client = nullptr;
    if (u && isPlayerBit(u)) client = e->getClient(game, u[0x25], u[0x26]);
    req[0] = a5 != 0 ? 0x8001 : 1;
    req[1] = (uint32_t)(uintptr_t)client;
    req[4] = a4;
    req[11] = e->unitClass(u); req[12] = e->unitLevel(u);
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
                if (a5 != 0) req[19] = (uint32_t)a5;
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

// 1.13c D2Game+0x3E740 -- ecx = game, edx = unit, stack t (ret 4).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3e740(void* game, int32_t* u, int32_t* t, const D2GameBatch3beEnv* e = &g3beDirect)
{
    int32_t id = (int32_t)e->unitClass(u);
    int32_t lvl = (int32_t)e->unitLevel(u);
    if (!skillRow(e, id) || !u || !isPlayerBit(u)) return 1;
    if (!e->getClient(game, u[0x25], u[0x26])) return 1;
    if (!t || t[0] != 1) return 1;
    uint32_t guid = e->unitGuid(u);
    if (!game) fatal(0x2d3);
    int32_t* n = (int32_t*)ptr(game, 0x1920 + (guid & 0x7f) * 4);
    if (!n) return 1;
    while ((uint32_t)n[3] != guid) {
        n = (int32_t*)ptr(n, 0x39 * 4);
        if (!n) return 1;
    }
    // 0x1B10 (eax = id, edx = level): 0 for a bad row or level <= 0, else (lvl-1)*row[0x14c] + row[0x148]; read through the data-table seam (e.dt)
    const uint8_t* srow = skillRow(e, id);
    int32_t r = (srow && lvl > 0) ? (lvl - 1) * i32(srow, 0x14c) + i32(srow, 0x148) : 0;
    // retail order: 0x1130 (y) then 0x1100 (x) of the target, then of the node
    int32_t ty = e->unitY(t), tx = e->unitX(t), ny = e->unitY(n), nx = e->unitX(n);
    if (e->dist2(nx, ny, tx, ty) < r * r) {
        int32_t v = srow ? i32(srow, 0x15c) : 0;   // 0x1EC0 (eax = id)
        e->fd2120(t, n, game, v);
    }
    return 1;
}

// 1.13c D2Game+0x3EAE0 -- ecx = game, edx = unit, stack t (ret 4).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3eae0(void* game, int32_t* u, int32_t* t, const D2GameBatch3beEnv* e = &g3beDirect)
{
    int32_t id = (int32_t)e->unitClass(u);
    int32_t lvl = (int32_t)e->unitLevel(u);
    const uint8_t* srow = skillRow(e, id);
    if (!srow || !u || !isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x25], u[0x26]);
    if (!client) return 1;
    if (t && t[0] == 1) {
        int16_t s = i16(srow, 0x82);
        if (s < 0 || s >= i32(dtab(e), 0xc4)) return 1;
        if (e->isDead(t, s) == 0) {
            int32_t a = e->stat3(client, 0xc, 0);
            int32_t b = e->f21bc0(id);
            int32_t sum = b + (a + lvl);
            int32_t c = e->stat3(t, 0xc, 0);
            if (sum > c) e->fd2120(t, client, game, e->f21a90(id, lvl));
        }
    }
    return 0;
}

// 1.13c D2Game+0x3F6D0 -- ecx = game, edx = unit. Position (y first, then x) as the 3BC00 pair; callback RVA 0x39930
// for modes 2..0x18, 0x39910 for mode 1; anything else skips 0x9F340. Always ends in the collision handler.
EMSCRIPTEN_KEEPALIVE void d2_game_3f6d0(void* game, int32_t* u, const D2GameBatch3beEnv* e = &g3beDirect)
{
    int32_t m = e->unitMode(u);
    if (m >= 1 && m <= 0x18) {
        const uint32_t t = (uint32_t)u[0];
        void* sub = ptr(u, 0x2c);
        const bool direct = t == 2 || (t > 3 && t <= 5);
        const int32_t y = direct ? i32(sub, 0x10) : sub ? e->pathY(sub) : 0;
        const int32_t x = direct ? i32(sub, 0xc) : sub ? e->pathX(sub) : 0;
        e->fcbf340(0x3002, (uint32_t)x, game, y, u, 0x7c, 1, m == 1 ? 0x39910u : 0x39930u);
    }
    e->collide(game, u);
}

// 1.13c D2Game+0x3F7D0 -- esi = game (frame counter at +0xA8).
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3f7d0(void* game, int32_t* u, int32_t flag, void* esi, const D2GameBatch3beEnv* e = &g3beDirect)
{
    if (!u || !isPlayerBit(u)) return 1;
    void* client = e->getClient(game, u[0x25], u[0x26]);
    if (!client || !flag) return 1;
    uint32_t id = e->unitClass(u), lvl = e->unitLevel(u);
    int32_t f = e->frame() - i32(esi, 0xa8);
    if (f > 9 && f <= e->maxFrames(client, id, lvl, 1)) {
        e->f6663a0(esi, client, f, flag, lvl);
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
    int32_t py = e->unitY(u), px = e->unitX(u);   // retail reads 0x21130 (y) before 0x21100 (x)
    e->setPos(path, px + sx, py + sy);
    e->moveTo(path, u, 0);
    e->setLife(u, (int32_t)e->r10513());
    e->setDir(u, sy * 0x10000 + (sx & 0xffff));
    // the decompile passes the unit where the first call passes the game (stale register); game is used.
    return (uint32_t)e->collide(game, u);
}

// function probe seam (src/core/fn/d2game_3cdd0.cpp): callees are HookEnv recorders; dt is the descriptor's data-table blob.
static HookEnv* g3cH; static int32_t* g3cU;
uint32_t D2Game_3CDD0_probe(void* game, int32_t* u, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g3cH = &h; g3cU = u;
    D2GameBatch3beEnv e = g3beDirect;
    e.dt = dt;
    e.getClient = [](void* g, uint32_t ty, uint32_t id) -> void* { return (void*)(uintptr_t)g3cH->h(0xE03A0, ty, (uint32_t)(uintptr_t)g, id); };
    e.unitX = [](void* u) -> int32_t { return (int32_t)g3cH->h(0x1100, 0, (uint32_t)(uintptr_t)u); };
    e.unitY = [](void* u) -> int32_t { return (int32_t)g3cH->h(0x1130, 0, (uint32_t)(uintptr_t)u); };
    e.unitClass = [](void* u) -> uint32_t { return g3cH->h(0xB26A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.unitLevel = [](void* u) -> uint32_t { return g3cH->h(0xB0FC, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    // hookblk 0x6F930 d 23 (the 23-dword request in EDX, mirrored from the capture): the block rides after the call's own record
    e.createMissile = [](void* g, uint32_t* req) -> void* { void* r = (void*)(uintptr_t)g3cH->h(0x6F930, 0, (uint32_t)(uintptr_t)g, g3cH->stk(req)); g3cH->blk(0x6F930, req, 23); return r; };
    return d2_game_3cdd0(game, u, &e);
}

// function probe seams for the missile rows (src/core/fn/d2game_3c990.cpp, _3d020, _3d450): retail's callees are HookEnv recorders.
static HookEnv* g3mH;
static void g3mCommon(D2GameBatch3beEnv& e)
{
    e.getClient = [](void* g, uint32_t ty, uint32_t id) -> void* { return (void*)(uintptr_t)g3mH->h(0xE03A0, ty, (uint32_t)(uintptr_t)g, id); };
    e.unitX = [](void* u) -> int32_t { return (int32_t)g3mH->h(0x1100, 0, (uint32_t)(uintptr_t)u); };
    e.unitY = [](void* u) -> int32_t { return (int32_t)g3mH->h(0x1130, 0, (uint32_t)(uintptr_t)u); };
    e.unitClass = [](void* u) -> uint32_t { return g3mH->h(0xB26A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.unitLevel = [](void* u) -> uint32_t { return g3mH->h(0xB0FC, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.skillDuration = [](void* c, uint32_t a, uint32_t id, uint32_t lvl) -> int32_t { return (int32_t)g3mH->h(0xAC9A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)c, a, id, lvl); };
    e.createMissile = [](void* g, uint32_t* req) -> void* { void* r = (void*)(uintptr_t)g3mH->h(0x6F930, 0, (uint32_t)(uintptr_t)g, g3mH->stk(req)); g3mH->blk(0x6F930, req, 23); return r; };
}
uint32_t D2Game_3C990_probe(void* game, int32_t* u, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g3mH = &h;
    D2GameBatch3beEnv e = g3beDirect; e.dt = dt; g3mCommon(e);
    return d2_game_3c990(game, u, &e);
}
uint32_t D2Game_3D020_probe(void* game, int32_t* u, int32_t* ctx, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g3mH = &h;
    D2GameBatch3beEnv e = g3beDirect; e.dt = dt; g3mCommon(e);
    e.unitGuid = [](void* u) -> uint32_t { return g3mH->h(0xB24C, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.skillRow1420 = [](int32_t id) -> const uint8_t* { return (const uint8_t*)(uintptr_t)g3mH->h(0x1420, (uint32_t)id); };
    e.findTarget = [](void* g, void* c, int32_t x, int32_t y, int32_t range, uint32_t filter, int32_t a7, int32_t*) -> void* {
        return (void*)(uintptr_t)g3mH->h(0xA0F40, filter, (uint32_t)a7, (uint32_t)range, 0, 0, (uint32_t)(uintptr_t)g, (uint32_t)(uintptr_t)c, (uint32_t)x, (uint32_t)y); };
    e.setLife = [](void* m, int32_t v) { g3mH->h(0xA50E, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)m, (uint32_t)v); };
    return d2_game_3d020(game, u, ctx, &e);
}
uint32_t D2Game_3E740_probe(void* game, int32_t* u, int32_t* t, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{   // src/core/fn/d2game_3e740.cpp: callees are HookEnv recorders (0xB2120: eax = node, ecx = game, esi = target, stack value)
    HookEnv h(lg, sc); g3mH = &h;
    D2GameBatch3beEnv e = g3beDirect; e.dt = dt; g3mCommon(e);
    e.unitGuid = [](void* u) -> uint32_t { return g3mH->h(0xB24C, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.dist2 = [](int32_t a, int32_t b, int32_t c, int32_t d) -> int32_t { return (int32_t)g3mH->h(0xAE26, 0, 0, 0, 0, 0, (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d); };
    e.fd2120 = [](void* tg, void* node, void* g, int32_t v) { g3mH->h(0xB2120, (uint32_t)(uintptr_t)node, (uint32_t)(uintptr_t)g, 0, 0, (uint32_t)(uintptr_t)tg, (uint32_t)v); };
    return d2_game_3e740(game, u, t, &e);
}
void D2Game_3D450_probe(void* game, int32_t* u, int32_t radius, uint32_t a4, int32_t a5, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g3mH = &h;
    D2GameBatch3beEnv e = g3beDirect; g3mCommon(e);
    e.getRoom = [](void* u) -> void* { return (void*)(uintptr_t)g3mH->h(0xA478, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.pathX = [](void* p) -> int32_t { return (int32_t)g3mH->h(0xA454, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)p); };
    e.pathY = [](void* p) -> int32_t { return (int32_t)g3mH->h(0xA3B8, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)p); };
    e.roomCollide = [](void* r, const int32_t* a, const int32_t* b, int32_t m) -> int32_t {
        return (int32_t)g3mH->h(0xAEB0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)r, g3mH->stk(a), g3mH->stk(b), (uint32_t)m); };
    e.roomAux = [](void* r) -> void* { return (void*)(uintptr_t)g3mH->h(0x1280, (uint32_t)(uintptr_t)r); };
    e.auxTest = [](void* a) -> int32_t { return (int32_t)g3mH->h(0xA808, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)a); };
    d2_game_3d450(game, u, radius, a4, a5, &e);
}

// function probe seam (src/core/fn/d2game_3f7d0.cpp): retail's callees are HookEnv recorders (E03A0, B26A, B0FC, B24C, ACE2, 463A0).
static HookEnv* g3fH; static int32_t* g3fU;
uint32_t D2Game_3F7D0_probe(void* game, int32_t* u, int32_t flag, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g3fH = &h; g3fU = u;
    D2GameBatch3beEnv e = g3beDirect;
    e.getClient = [](void* g, uint32_t ty, uint32_t id) -> void* { return (void*)(uintptr_t)g3fH->h(0xE03A0, ty, (uint32_t)(uintptr_t)g, id); };
    e.unitClass = [](void* u) -> uint32_t { return g3fH->h(0xB26A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.unitLevel = [](void* u) -> uint32_t { return g3fH->h(0xB0FC, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.frame = []() -> int32_t { return (int32_t)g3fH->h(0xB24C, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)g3fU); };
    e.maxFrames = [](void* c, uint32_t id, uint32_t lvl, int32_t v) -> int32_t { return (int32_t)g3fH->h(0xACE2, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)c, id, lvl, (uint32_t)v); };
    e.f6663a0 = [](void* g, void* c, int32_t el, int32_t fl, uint32_t lv) { g3fH->h(0x463A0, (uint32_t)fl, 0, 0, (uint32_t)el, lv, (uint32_t)(uintptr_t)g, (uint32_t)(uintptr_t)c, (uint32_t)el); };
    return d2_game_3f7d0(game, u, flag, game, &e);
}

// function probe seam (src/core/fn/d2game_3f890.cpp): hooks as in the descriptor; the seed pointer is unit+0x20.
static int32_t g3gFrame;
uint32_t D2Game_3F890_probe(void* game, int32_t* u, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g3fH = &h; g3fU = u;
    D2GameBatch3beEnv e = g3beDirect;
    e.dt = dt;
    e.dir = [](void* x) -> int32_t { return (int32_t)g3fH->h(0xB28E, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)x); };
    e.unitGuid = [](void* x) -> uint32_t { g3gFrame = (int32_t)g3fH->h(0xB24C, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)x); return (uint32_t)g3gFrame; };
    e.r10614 = []() { g3fH->h(0xA628, 0, (uint32_t)(uintptr_t)(g3fU + 8), (uint32_t)g3gFrame); };
    e.dirPacked = [](void* x) -> int32_t { return (int32_t)g3fH->h(0xB288, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)x); };
    e.unitX = [](void* x) -> int32_t { return (int32_t)g3fH->h(0x1100, 0, (uint32_t)(uintptr_t)x); };
    e.unitY = [](void* x) -> int32_t { return (int32_t)g3fH->h(0x1130, 0, (uint32_t)(uintptr_t)x); };
    e.setPos = [](void* p, int32_t x, int32_t y) { g3fH->h(0xAB2C, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)p, (uint32_t)x, (uint32_t)y); };
    e.moveTo = [](void* p, void* x, int32_t v) { g3fH->h(0xAAF0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)p, (uint32_t)(uintptr_t)x, (uint32_t)v); };
    e.r10513 = []() -> uint32_t { return g3fH->h(0xAFB2, 0, (uint32_t)(uintptr_t)(g3fU + 8)); };
    e.setLife = [](void* x, int32_t v) { g3fH->h(0xA50E, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)x, (uint32_t)v); };
    e.setDir = [](void* x, int32_t v) { g3fH->h(0xAF5E, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)x, (uint32_t)v); };
    e.collide = [](void* g, void* x) -> int32_t { (void)x; return (int32_t)g3fH->h(0x3F320, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)g); };
    return d2_game_3f890(game, u, &e);
}

// function probe seam (src/core/fn/d2game_3be70.cpp): dt = table blob, lg = hook log, cfg[0] = what #11018 (0xB24C) returns.
static const uint32_t* g_pbe_cfg; static uint32_t* g_pbe_lg;
static void pbe_log(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3)
{
    uint32_t* w = g_pbe_lg + 1 + g_pbe_lg[0]++ * 10;
    w[0] = rva; w[1] = eax; w[2] = ecx; w[3] = edx; w[4] = 0; w[5] = esi; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = s3;
    for (int i = 1; i < 10; i++) if (HookEnv::isStk(w[i])) w[i] = 0xB3000000u;
}
static const uint8_t* g_pbe_dt;
uint32_t D2Game_3BE70_probe(void* game, const int32_t* u, int32_t flag, uint32_t* lg, const uint8_t* dt, const uint32_t* cfg)
{
    g_pbe_cfg = cfg; g_pbe_lg = lg; g_pbe_dt = dt; lg[0] = 0;
    D2GameBatch3beEnv e{};
    e.dt = dt;
    e.unitGuid = [](void* u) -> uint32_t { pbe_log(0xB24C, 0, 0, 0, 0, (uint32_t)(uintptr_t)u, 0, 0, 0); return g_pbe_cfg[0]; };
    e.f3bda0 = [](void* g, int32_t v) { pbe_log(0x3BDA0, 0, 0, 0, 0, (uint32_t)(uintptr_t)g, (uint32_t)v, 0, 0); };
    return d2_game_3be70(game, u, flag, &e);
}

void D2Game_3E590_probe(int32_t* u, int32_t* t, uint32_t* lg, const uint8_t* dt, const uint32_t* cfg)
{   // cfg[0]: the unit createUnit (#11013 / 0xA6A0) returns
    g_pbe_cfg = cfg; g_pbe_lg = lg; g_pbe_dt = dt; lg[0] = 0;
    D2GameBatch3beEnv e{};
    e.dt = dt;
    auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    (void)P;
    e.createUnit = [](void* pool, int32_t ty, uint32_t id, uint32_t a, uint32_t b) -> void* {
        pbe_log(0xA6A0, 0, 0, 0, 0, (uint32_t)(uintptr_t)pool, (uint32_t)ty, id, a); (void)b; return (void*)(uintptr_t)g_pbe_cfg[0]; };
    e.f6caea80 = [](void* g, void* u, int32_t a, uint32_t b, int32_t c, int32_t d, int32_t x) {
        (void)d; (void)x; pbe_log(0x8EA80, 0, (uint32_t)(uintptr_t)g, (uint32_t)(uintptr_t)u, 0, (uint32_t)a, b, (uint32_t)c, 0); };
    e.setMode = [](void* s, int32_t v) { pbe_log(0xA5B6, 0, 0, 0, 0, (uint32_t)(uintptr_t)s, (uint32_t)v, 0, 0); };
    e.setCallback = [](void* s, const void* cb) { pbe_log(0xA75A, 0, 0, 0, 0, (uint32_t)(uintptr_t)s, (uint32_t)(uintptr_t)cb, 0, 0); };
    e.attach = [](void* u, void* s, int32_t v) { pbe_log(0xA60A, 0, 0, 0, 0, (uint32_t)(uintptr_t)u, (uint32_t)(uintptr_t)s, (uint32_t)v, 0); };
    e.setFlag = [](void* u, int32_t st, int32_t v) { pbe_log(0xA3CA, 0, 0, 0, 0, (uint32_t)(uintptr_t)u, (uint32_t)st, (uint32_t)v, 0); };
    e.cb58e60 = (const void*)(uintptr_t)0xB2038E60u;   // the vector stores a retail code address as 0xB2<RVA> (callback RVA 0x38E60)
    d2_game_3e590(u, t, &e);
}

void D2Game_3F6D0_probe(void* game, int32_t* u, uint32_t* lg, const uint32_t* cfg)
{   // cfg: mode (#10737 / 0xB102), y (0xA3B8), x (0xA454)
    g_pbe_cfg = cfg; g_pbe_lg = lg; lg[0] = 0;
    D2GameBatch3beEnv e{};
    e.unitMode = [](void* u) -> int32_t { pbe_log(0xB102, 0, 0, 0, 0, (uint32_t)(uintptr_t)u, 0, 0, 0); return (int32_t)g_pbe_cfg[0]; };
    e.pathY = [](void* p) -> int32_t { pbe_log(0xA3B8, 0, 0, 0, 0, (uint32_t)(uintptr_t)p, 0, 0, 0); return (int32_t)g_pbe_cfg[1]; };
    e.pathX = [](void* p) -> int32_t { pbe_log(0xA454, 0, 0, 0, 0, (uint32_t)(uintptr_t)p, 0, 0, 0); return (int32_t)g_pbe_cfg[2]; };
    e.fcbf340 = [](uint32_t c, uint32_t d, void* g, int32_t y, void* u, int32_t a, int32_t b, uint32_t cb) {
        (void)b; (void)cb; pbe_log(0x9F340, 0, c, d, 0, (uint32_t)(uintptr_t)g, (uint32_t)y, (uint32_t)(uintptr_t)u, (uint32_t)a); };
    e.collide = [](void* g, void* u) -> int32_t { (void)u; pbe_log(0x3F320, 0, 0, 0, 0, (uint32_t)(uintptr_t)g, 0, 0, 0); return 0; };
    d2_game_3f6d0(game, u, &e);
}
