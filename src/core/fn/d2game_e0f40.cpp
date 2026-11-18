// 1.13c D2Game+0xE0F40 probe: the game's own SUNIT_Add (D2Game/UNIT/SUnit.cpp).
// regs EAX=pRoom r[0], ECX=nY r[1], EDX=pUnit r[2]; stack nX r[3], pGame r[5], a6 r[6]; aux r[7]. Blobs: unit 0, game 1, room 2, aux 3, log 4.
// aux+8 = the hook log, aux+0x10/0x14 = the level/act the two D2Common hooks return, aux+0x18/0x1C = the DF9A0 clients test and the
// UNITS_GetRoom return, aux+0x120 = the objects.txt nHasCollision bytes (copied into a native row). Unit and game go through the mirror; the
// room is only handed to callees, so it stays an opaque word. Every callee is the recorder logging retail's call.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
#include "fn_tag.h"
#include "mirror_layouts.h"
#include "../include/D2Game/UNIT/SUnit.h"
#include "../include/D2Game/MONSTER/Monster.h"
#include "../include/D2Game/PLAYER/Player.h"
#include "../include/D2DataTbls.h"
#include "../include/D2Dungeon.h"
#include "../include/Path/Path.h"
#include "../include/Units/Units.h"
#include "../include/Units/UnitRoom.h"
#include "../compat/test_seam.h"

static HookEnv* E; static mirror::Mirror* M; static uint32_t g_aux, g_pool;
static uint32_t rw(const void* p) { return (uint32_t)(uintptr_t)p; }
static uint32_t C(uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) { return E->h(rva, 0, 0, 0, 0, 0, s0, s1, s2, s3); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_e0f40(uint32_t* r)
{
    const uint32_t aux = r[7]; g_aux = aux; g_pool = HookEnv::rd32(r[5] + 0x1C);
    const uint32_t sc[11] = { 0xA556, HookEnv::rd32(aux + 0x10), 0xA56E, HookEnv::rd32(aux + 0x14), 0xA520, aux,
                              0xDF9A0, HookEnv::rd32(aux + 0x18), 0xA478, HookEnv::rd32(aux + 0x1C), 0 };
    HookEnv he((uint32_t*)(uintptr_t)HookEnv::rd32(aux + 8), sc); E = &he;
    mirror::Mirror m(mirror::types, mirror::ntypes); M = &m;
    m.allow(r[2], fnBlobLen(r, 0));
    m.allow(r[5], fnBlobLen(r, 1));
    auto pUnit = m.get<D2UnitStrc>(r[2]);
    auto pGame = m.get<D2GameStrc>(r[5]);
    auto pRoom = (D2ActiveRoomStrc*)(uintptr_t)r[0];

    D2Seam<&DUNGEON_GetLevelIdFromRoom>::hook = [](D2ActiveRoomStrc* room) -> int { return (int)C(0xA556, rw(room)); };
    D2Seam<&DRLG_GetActNoFromLevelId>::hook = [](int lvl) -> uint8_t { return (uint8_t)C(0xA56E, (uint32_t)lvl); };
    D2Seam<&PATH_AllocDynamicPath>::hook = [](void*, D2ActiveRoomStrc* room, int x, int y, D2UnitStrc*, BOOL) { C(0xA66A, g_pool, rw(room), (uint32_t)x, (uint32_t)y); };
    D2Seam<&sub_6FC7B7A0>::hook = [](D2GameStrc*, D2UnitStrc*, int32_t x, int32_t y) { C(0x37E20, (uint32_t)x, (uint32_t)y); };
    D2Seam<&MONSTER_SetVelocityAndPosition>::hook = [](D2GameStrc* g, D2UnitStrc*, int32_t x, int32_t y, int32_t) -> int32_t { return (int32_t)C(0xAFAB0, M->ra(g), (uint32_t)x, (uint32_t)y); };
    D2Seam<&SUNIT_RoomHasClients_DF9A0>::hook = [](D2UnitStrc*) -> int32_t { return (int32_t)C(0xDF9A0); };
    D2Seam<&MONSTER_UpdateAiCallbackEvent>::hook = [](D2GameStrc*, D2UnitStrc*) { C(0xAF920); };
    D2Seam<&sub_6FCBC300>::hook = [](D2GameStrc* g, D2UnitStrc*) { C(0xE0860, M->ra(g)); };
    D2Seam<&DATATBLS_GetObjectsTxtRecord>::hook = [](int id) -> D2ObjectsTxt* {
        static D2ObjectsTxt row; C(0xA520, (uint32_t)id);
        std::memset(&row, 0, sizeof row);
        for (size_t i = 0; i < sizeof row.nHasCollision; i++) row.nHasCollision[i] = *(const uint8_t*)(uintptr_t)(g_aux + 0x120 + i);
        return &row; };
    D2Seam<&UNITS_InitializeStaticPath>::hook = [](D2UnitStrc* u, D2ActiveRoomStrc* room, int x, int y) { C(0xA5F2, M->ra(u), rw(room), (uint32_t)x, (uint32_t)y); };
    D2Seam<&UNITS_BlockCollisionPath>::hook = [](D2UnitStrc* u, D2ActiveRoomStrc* room, int x, int y) { C(0xA69A, M->ra(u), rw(room), (uint32_t)x, (uint32_t)y); };
    D2Seam<&UNITROOM_AddUnitToRoom>::hook = [](D2UnitStrc* u, D2ActiveRoomStrc* room) -> int { C(0xA6FA, M->ra(u), rw(room)); return 1; };
    D2Seam<&UNITROOM_AddUnitToRoomEx>::hook = [](D2UnitStrc* u, D2ActiveRoomStrc* room, int z) -> int { C(0xA62E, M->ra(u), rw(room), (uint32_t)z); return 1; };
    D2Seam<&SUNIT_AddUnitToList>::hook = [](D2GameStrc*, D2UnitStrc*) -> D2UnitStrc* { C(0xE0310); return nullptr; };
    D2Seam<&UNITS_GetRoom>::hook = [](D2UnitStrc* u) -> D2ActiveRoomStrc* { return (D2ActiveRoomStrc*)(uintptr_t)C(0xA478, M->ra(u)); };
    D2Seam<&UNITROOM_RefreshUnit>::hook = [](D2UnitStrc* u) { C(0xA4DE, M->ra(u)); };
    D2Seam<&UNITS_SetCurrentRoomInvalid>::hook = [](D2UnitStrc* u, int a) { C(0xA6E8, M->ra(u), (uint32_t)a); };

    SUNIT_Add(pUnit, (int32_t)r[3], (int32_t)r[1], pGame, pRoom, (int32_t)r[6]);

    D2Seam<&DUNGEON_GetLevelIdFromRoom>::hook = nullptr; D2Seam<&DRLG_GetActNoFromLevelId>::hook = nullptr; D2Seam<&PATH_AllocDynamicPath>::hook = nullptr;
    D2Seam<&sub_6FC7B7A0>::hook = nullptr; D2Seam<&MONSTER_SetVelocityAndPosition>::hook = nullptr; D2Seam<&SUNIT_RoomHasClients_DF9A0>::hook = nullptr;
    D2Seam<&MONSTER_UpdateAiCallbackEvent>::hook = nullptr; D2Seam<&sub_6FCBC300>::hook = nullptr; D2Seam<&DATATBLS_GetObjectsTxtRecord>::hook = nullptr;
    D2Seam<&UNITS_InitializeStaticPath>::hook = nullptr; D2Seam<&UNITS_BlockCollisionPath>::hook = nullptr; D2Seam<&UNITROOM_AddUnitToRoom>::hook = nullptr;
    D2Seam<&UNITROOM_AddUnitToRoomEx>::hook = nullptr; D2Seam<&SUNIT_AddUnitToList>::hook = nullptr; D2Seam<&UNITS_GetRoom>::hook = nullptr;
    D2Seam<&UNITROOM_RefreshUnit>::hook = nullptr; D2Seam<&UNITS_SetCurrentRoomInvalid>::hook = nullptr;
    m.commit();
    r[4] = r[6];   // retail leaves a6 in eax
    fnTagLog(r, 4, 4);
}
