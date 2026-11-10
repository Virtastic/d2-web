// Host test: tools/realm-tests.sh src/core/realm/d2gs_client_object.test.cpp
// A joined game's object unit: the state it is made with, the server's mode changes, the client clock,
// and the walk-collision footprint each of them leaves in the room.
#include "d2gs_client_object.cpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>

// Fake D2Common: three classes, and a footprint log in place of the room's collision grid.
static uint8_t g_txtMem[5 * sizeof(D2ObjectsTxt)];   // zeroed rows; the test fills the columns it reads
static D2ObjectsTxt* const g_txt = reinterpret_cast<D2ObjectsTxt*>(g_txtMem);
enum { DOOR, CHEST, TORCH, SHRINE, GLOW = 0x162 };   // GLOW is one of the seven classes with an idle glow
static std::string g_log;   // 'B' for a footprint written, 'F' for one cleared
D2ObjectsTxt* __stdcall DATATBLS_GetObjectsTxtRecord(int id) {
    return id == GLOW ? &g_txt[4] : id >= 0 && id < 4 ? &g_txt[id] : nullptr;
}
// The shrine records: the record's index is the assign packet's interact byte, its code the kind.
static D2ShrinesTxt g_shrines[23];
D2ShrinesTxt* __stdcall DATATBLS_GetShrinesTxtRecord(int id) { return id >= 0 && id < 23 ? &g_shrines[id] : nullptr; }
static int g_roomUnits;     // units on the room's unit list
int __stdcall UNITROOM_AddUnitToRoom(D2UnitStrc* unit, D2ActiveRoomStrc* room) { assert(room && !unit->pRoomNext); unit->pRoomNext = unit; ++g_roomUnits; return 1; }
void __stdcall UNITROOM_RemoveUnitFromRoom(D2UnitStrc* unit) { assert(unit->pRoomNext); unit->pRoomNext = nullptr; --g_roomUnits; }
D2UnitStrc* __stdcall UNITS_AllocUnit(void*, int type) {
    auto* unit = static_cast<D2UnitStrc*>(std::calloc(1, sizeof(D2UnitStrc)));
    unit->dwUnitType = type;
    return unit;
}
void __stdcall UNITS_FreeUnit(D2UnitStrc* unit) { std::free(unit); }
void __stdcall UNITS_AllocStaticPath(D2UnitStrc* unit) { unit->pStaticPath = static_cast<D2StaticPathStrc*>(std::calloc(1, sizeof(D2StaticPathStrc))); }
void __stdcall UNITS_FreeStaticPath(D2UnitStrc* unit) { std::free(unit->pStaticPath); }
void __stdcall UNITS_InitializeStaticPath(D2UnitStrc* unit, D2ActiveRoomStrc* room, int x, int y) {
    unit->pStaticPath->pRoom = room; unit->pStaticPath->tGameCoords.nX = x; unit->pStaticPath->tGameCoords.nY = y;
}
D2ActiveRoomStrc* __stdcall UNITS_GetRoom(D2UnitStrc* unit) { return unit->pStaticPath->pRoom; }
void __stdcall UNITS_SetAnimStartFrame(D2UnitStrc* unit) {
    const D2ObjectsTxt* txt = unit->pObjectData->pObjectTxt;
    unit->dwFrameCountPrecise = txt->dwFrameCnt[unit->dwAnimMode];
    unit->nSeqCurrentFramePrecise = txt->nStart[unit->dwAnimMode] << 8;
    unit->wAnimSpeed = txt->wFrameDelta[unit->dwAnimMode];
}
void __stdcall UNITS_BlockCollisionPath(D2UnitStrc*, D2ActiveRoomStrc* room, int x, int y) {
    assert(room && x == 100 && y == 200);
    g_log += 'B';
}
void __stdcall UNITS_FreeCollisionPath(D2UnitStrc*) { g_log += 'F'; }

static std::string take() { std::string s = g_log; g_log.clear(); return s; }
static bool targetable(const D2UnitStrc* u) { return (u->dwFlags & UNITFLAG_TARGETABLE) != 0; }
static int steps_until(D2UnitStrc* u, uint32_t mode) {
    int n = 0;
    while (u->dwAnimMode != mode) { d2gs_client_object_step(u); assert(++n < 1000); }
    return n;
}

static D2UnitStrc* g_players[3];
static D2UnitStrc* player_by_id(uint32_t id) { return id < 3 ? g_players[id] : nullptr; }
static int overlays(const D2UnitStrc* u, int32_t a = -1, int32_t b = -1) {
    int32_t got[3] = { -1, -1, -1 };
    const int n = d2gs_client_object_overlays(u, got);
    assert(got[0] == a && got[1] == b);
    return n;
}

int main() {
    alignas(16) static uint8_t roomMem[0x100];
    auto* room = reinterpret_cast<D2ActiveRoomStrc*>(roomMem);
    // A door: one cel closed and open, 4 cels opening at a full cel a tick, 4 cels closing from the last.
    D2ObjectsTxt& door = g_txt[DOOR];
    door.nIsDoor = 1;
    for (int m = 0; m < 8; ++m) { door.dwFrameCnt[m] = 0x100; door.wFrameDelta[m] = 0x100; }
    door.dwFrameCnt[1] = door.dwFrameCnt[3] = 0x400; door.nStart[3] = 3;
    door.nSelectable[0] = door.nSelectable[2] = 1;
    door.nHasCollision[0] = door.nHasCollision[1] = door.nHasCollision[3] = 1;
    door.nBlocksLight[0] = 1;
    // A chest: selectable closed, 3 cels operating, one cel opened; it never blocks.
    D2ObjectsTxt& chest = g_txt[CHEST];
    for (int m = 0; m < 8; ++m) { chest.dwFrameCnt[m] = 0x100; chest.wFrameDelta[m] = 0x80; }
    chest.dwFrameCnt[1] = 0x300; chest.nSelectable[0] = 1; chest.nHasCollision[0] = chest.nHasCollision[1] = 1;
    // A torch: 5 looping cels in its lit mode, stepping 0x180.
    D2ObjectsTxt& torch = g_txt[TORCH];
    for (int m = 0; m < 8; ++m) { torch.dwFrameCnt[m] = 0x100; torch.wFrameDelta[m] = 0x180; }
    torch.dwFrameCnt[2] = 0x500; torch.nCycleAnim[2] = 1; torch.nHasCollision[2] = 1;

    assert(!d2gs_client_object_create(1, 9, 0, 0) && !d2gs_client_object_create(1, DOOR, 8, 0));

    // The door arrives closed: selectable, casting no light through, its footprint written when placed.
    D2UnitStrc* d = d2gs_client_object_create(7, DOOR, 0, 0x80);
    assert(d && d->dwUnitId == 7 && d->dwAnimMode == 0 && targetable(d) && d->pObjectData->InteractType == 0x80);
    assert(!(d->dwFlags & UNITFLAG_DRAWSHADOW) && take() == "");
    d2gs_client_object_place(d, room, 100, 200);
    assert(take() == "B");

    // The server opens it: the client plays the opening cels, and only then clears the footprint.
    d2gs_client_object_set_mode(d, 2, 1);
    assert(d->dwAnimMode == 1 && !targetable(d) && d->nSeqCurrentFramePrecise == 0 && take() == "");
    d2gs_client_object_step(d); assert(d->dwAnimMode == 1 && d->nSeqCurrentFramePrecise == 0x100);
    d2gs_client_object_step(d); d2gs_client_object_step(d);
    assert(d->dwAnimMode == 1 && d->nSeqCurrentFramePrecise == 0x300 && take() == "");
    d2gs_client_object_step(d);
    assert(d->dwAnimMode == 2 && d->nSeqCurrentFramePrecise == 0 && targetable(d) && take() == "F");
    for (int i = 0; i < 5; ++i) d2gs_client_object_step(d);
    assert(d->dwAnimMode == 2 && take() == "");
    // The same mode again takes the packet's selectable byte and changes nothing else.
    d2gs_client_object_set_mode(d, 2, 0);
    assert(d->dwAnimMode == 2 && !targetable(d) && take() == "");
    d2gs_client_object_set_mode(d, 2, 1);
    assert(targetable(d) && (d->dwFlags & UNITFLAG_DRAWSHADOW));

    // The server closes it: the footprint is back at once, the closing cels run from the last to the first.
    d2gs_client_object_set_mode(d, 0, 1);
    assert(d->dwAnimMode == 3 && !targetable(d) && d->nSeqCurrentFramePrecise == 0x300 && take() == "B");
    assert(steps_until(d, 0) == 4 && d->nSeqCurrentFramePrecise == 0 && targetable(d) && take() == "");
    assert(d->dwFlags & UNITFLAG_DRAWSHADOW);   // the shadow flag is the mode-change packet's; the clock leaves it

    // A direct change (a door the server sends already open, then a blocked mode) moves the footprint at once.
    d2gs_client_object_set_mode(d, 4, 0);
    assert(d->dwAnimMode == 4 && !targetable(d) && take() == "F");
    d2gs_client_object_set_mode(d, 5, 1);
    assert(d->dwAnimMode == 5 && take() == "");
    d2gs_client_object_set_mode(d, 0, 1);
    assert(d->dwAnimMode == 0 && targetable(d) && take() == "B");
    d2gs_client_object_destroy(d);
    assert(take() == "F");

    // A door in a room that is not active changes mode and touches no grid; placing it later writes the footprint.
    d = d2gs_client_object_create(8, DOOR, 2, 0);
    d2gs_client_object_place(d, nullptr, 100, 200);
    d2gs_client_object_set_mode(d, 0, 1);
    assert(d->dwAnimMode == 3 && take() == "");
    steps_until(d, 0);
    d2gs_client_object_place(d, room, 100, 200);
    assert(take() == "B");
    d2gs_client_object_unplace(d);
    d2gs_client_object_destroy(d);
    assert(take() == "");

    // A chest: byte 7 is not read; operating runs out into opened on the clock, which drops the footprint.
    D2UnitStrc* c = d2gs_client_object_create(9, CHEST, 0, 0);
    d2gs_client_object_place(c, room, 100, 200);
    assert(targetable(c) && take() == "B");
    d2gs_client_object_set_mode(c, 1, 1);
    assert(c->dwAnimMode == 1 && !targetable(c) && take() == "");
    d2gs_client_object_step(c);
    assert(c->nSeqCurrentFramePrecise == 0x80);
    d2gs_client_object_set_mode(c, 1, 0);                      // the same mode again: the clock is not restarted
    assert(c->nSeqCurrentFramePrecise == 0x80);
    assert(steps_until(c, 2) == 4 && c->nSeqCurrentFramePrecise == 0 && !targetable(c) && take() == "F");
    d2gs_client_object_destroy(c);
    assert(take() == "");

    // A torch loops: 0x180 a tick over 5 cels wraps past the end back to the start.
    D2UnitStrc* t = d2gs_client_object_create(10, TORCH, 0, 0);
    d2gs_client_object_place(t, room, 100, 200);
    d2gs_client_object_set_mode(t, 2, 0);
    assert(t->dwAnimMode == 2 && take() == "");                // a mode change never writes a footprint, it only clears one
    const int32_t expect[] = { 0x180, 0x300, 0x480, 0x100, 0x280 };
    for (int32_t frame : expect) { d2gs_client_object_step(t); assert(t->nSeqCurrentFramePrecise == frame); }
    d2gs_client_object_set_mode(t, 2, 0);                      // repeated by the server: the loop goes on
    assert(t->nSeqCurrentFramePrecise == 0x280);
    d2gs_client_object_destroy(t);
    assert(g_roomUnits == 0 && !d2gs_client_object_next(nullptr));

    // ---- Shrines and the object table, on the server's packet bytes ----
    for (int i = 0; i < 23; ++i) g_shrines[i].nCode = uint8_t(i);
    D2ObjectsTxt& shrineTxt = g_txt[SHRINE];
    shrineTxt.nSubClass = 1;
    for (int m = 0; m < 8; ++m) { shrineTxt.dwFrameCnt[m] = 0x100; shrineTxt.wFrameDelta[m] = 0x100; }
    shrineTxt.dwFrameCnt[1] = 0x200; shrineTxt.nSelectable[0] = 1;
    D2PlayerDataStrc names[2] = {};
    std::strcpy(names[0].szName, "Sorc"); std::strcpy(names[1].szName, "Abcdefghijklmno");
    D2UnitStrc players[2] = {};
    for (int i = 0; i < 2; ++i) { players[i].dwUnitType = UNIT_PLAYER; players[i].dwUnitId = i + 1; players[i].pPlayerData = &names[i]; g_players[i + 1] = &players[i]; }

    // The table finds a unit by id; two ids sharing the low 7 bits share a chain, and the walk visits each once.
    D2UnitStrc* s6 = d2gs_client_object_create(0x20, SHRINE, 0, 6);      // an armor shrine (kind 6), idle
    D2UnitStrc* s16 = d2gs_client_object_create(0xA0, SHRINE, 0, 16);    // same chain as 0x20
    D2UnitStrc* s2 = d2gs_client_object_create(0x21, SHRINE, 2, 2);      // a kind with no overlay, made opened
    assert(d2gs_client_object_find(0x20) == s6 && d2gs_client_object_find(0xA0) == s16 && !d2gs_client_object_find(0x120));
    int walked = 0;
    for (D2UnitStrc* u = d2gs_client_object_next(nullptr); u; u = d2gs_client_object_next(u)) ++walked;
    assert(walked == 3);
    d2gs_client_object_place(s6, room, 100, 200);
    d2gs_client_object_place(s16, room, 100, 200);
    assert(g_roomUnits == 2);                                   // a placed object is on its room's unit list
    d2gs_client_object_place(s6, room, 100, 200);
    assert(g_roomUnits == 2);                                   // placing again does not list it twice
    // An idle shrine shows its kind's two overlays; kinds without them show none.
    assert(s6->pObjectData->pShrineTxt == &g_shrines[6] && overlays(s6, 0x3B, 0x39) == 2);
    assert(overlays(s16) == 0 && overlays(s2) == 0);

    D2gsClientObjectEvent ev = {};
    // 0x0E action 0x15: player 1 activates the armor shrine. Its sound, no effect; the mode is the server's to change.
    const uint8_t act6[12] = { 0x0e, 2, 0x20, 0, 0, 0, 0x15, 1, 0, 0, 0, 0 };
    assert(d2gs_client_object_packet(act6, 12, player_by_id, &ev) == 2);
    assert(ev.objectGuid == 0x20 && ev.playerGuid == 1 && ev.shrine == 6 && ev.sound == 0xA68 && ev.effect == kObjectEffectNone);
    assert(s6->dwAnimMode == 0 && overlays(s6, 0x3B, 0x39) == 2);
    // 0x0E action 3: the server moves it to operating; a shrine that is not idle drops both overlays.
    const uint8_t op6[12] = { 0x0e, 2, 0x20, 0, 0, 0, 3, 0, 1, 0, 0, 0 };
    assert(d2gs_client_object_packet(op6, 12, player_by_id, &ev) == 1 && s6->dwAnimMode == 1 && overlays(s6) == 0);
    assert(steps_until(s6, 2) == 2 && overlays(s6) == 0);       // and the clock does not bring them back
    // Back to idle (the shrine recharged): both return.
    const uint8_t idle6[12] = { 0x0e, 2, 0x20, 0, 0, 0, 3, 1, 0, 0, 0, 0 };
    assert(d2gs_client_object_packet(idle6, 12, player_by_id, &ev) == 1 && overlays(s6, 0x3B, 0x39) == 2 && targetable(s6));

    // The unit-action packet aimed at an object: its dword at 6 is the player. Kind 16 turns the name around.
    const uint8_t act16[17] = { 0x4d, 2, 0xA0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    assert(d2gs_client_object_packet(act16, 17, player_by_id, &ev) == 2);
    assert(ev.objectGuid == 0xA0 && ev.sound == 0xA76 && ev.effect == kObjectEffectReverseName);
    assert(!std::strcmp(names[0].szName, "croS"));
    uint8_t act16b[17]; std::memcpy(act16b, act16, 17); act16b[6] = 2;
    assert(d2gs_client_object_packet(act16b, 17, player_by_id, &ev) == 2 && !std::strcmp(names[1].szName, "onmlkjihgfedcbA"));

    // Nothing happens for: a player the client does not know, an object it does not know, another unit type,
    // an action objects do not take, and a packet of the wrong size.
    ev = {};
    const uint8_t noPlayer[12] = { 0x0e, 2, 0x20, 0, 0, 0, 0x15, 9, 0, 0, 0, 0 };
    const uint8_t noObject[12] = { 0x0e, 2, 0x77, 0, 0, 0, 0x15, 1, 0, 0, 0, 0 };
    const uint8_t monster[12] = { 0x0e, 1, 0x20, 0, 0, 0, 0x15, 1, 0, 0, 0, 0 };
    const uint8_t action7[12] = { 0x0e, 2, 0x20, 0, 0, 0, 7, 1, 2, 0, 0, 0 };
    assert(d2gs_client_object_packet(noPlayer, 12, player_by_id, &ev) == 0 && d2gs_client_object_packet(noObject, 12, player_by_id, &ev) == 0);
    assert(d2gs_client_object_packet(monster, 12, player_by_id, &ev) == 0 && d2gs_client_object_packet(action7, 12, player_by_id, &ev) == 0);
    assert(d2gs_client_object_packet(act6, 11, player_by_id, &ev) == -1 && d2gs_client_object_packet(act16, 12, player_by_id, &ev) == -1);
    assert(ev.sound == 0 && s6->dwAnimMode == 0);
    // An object that is not a shrine has no shrine record: activation does nothing.
    D2UnitStrc* plain = d2gs_client_object_create(0x30, CHEST, 0, 0);
    const uint8_t actChest[12] = { 0x0e, 2, 0x30, 0, 0, 0, 0x15, 1, 0, 0, 0, 0 };
    assert(d2gs_client_object_packet(actChest, 12, player_by_id, &ev) == 0);
    d2gs_client_object_destroy(plain);

    // Every kind's sound and effect, by the assign packet's interact byte.
    static const uint16_t kSound[23] = { 0, 0xA71, 0xA71, 0xA70, 0xA6A, 0xA6A, 0xA68, 0xA69, 0xA73, 0xA72, 0xA74, 0xA75,
        0xA77, 0xA70, 0xA70, 0xA6B, 0xA76, 0xA6F, 0xA6D, 0xA78, 0xA6F, 0xA6C, 0xA6E };
    static const int16_t kOverlay[23][2] = { {-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{0x3B,0x39},{0x3C,0x39},
        {0x3E,0x39},{0x3F,0x3A},{0x3D,0x3A},{0x40,0x3A},{0x41,0x39},{0x42,0x3A},{0x43,0x3A},{0x44,0x39},
        {-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1} };
    for (int kind = 0; kind < 23; ++kind) {
        D2UnitStrc* u = d2gs_client_object_create(0x40, SHRINE, 0, uint8_t(kind));
        overlays(u, kOverlay[kind][0], kOverlay[kind][1]);
        const uint8_t act[12] = { 0x0e, 2, 0x40, 0, 0, 0, 0x15, 2, 0, 0, 0, 0 };
        std::strcpy(names[1].szName, "ab");
        assert(d2gs_client_object_packet(act, 12, player_by_id, &ev) == 2 && ev.shrine == uint32_t(kind) && ev.sound == kSound[kind]);
        const uint32_t effect = kind == 16 ? kObjectEffectReverseName : kind == 19 ? kObjectEffectFire :
            kind == 21 ? kObjectEffectExploding : kind == 22 ? kObjectEffectPoison : kObjectEffectNone;
        assert(ev.effect == effect && !std::strcmp(names[1].szName, kind == 16 ? "ba" : "ab"));
        d2gs_client_object_destroy(u);
    }

    // The missiles of the three missile kinds, for a level-23 player (missile level 23 / 5 = 4) at (100, 200).
    D2gsClientObjectMissile m[16];
    assert(d2gs_client_object_effect_missiles(s6, kObjectEffectFire, 23, m, 16) == 16);
    assert(m[0].missile == 0x3E && m[0].flags == 3 && m[0].x == 100 && m[0].y == 200 && m[0].targetX == 5 && m[0].targetY == 5 && m[0].level == 4);
    assert(m[1].targetX == 5 && m[1].targetY == -10 && m[4].targetX == -10 && m[4].targetY == 5 && m[15].targetX == -20 && m[15].targetY == -20);
    assert(d2gs_client_object_effect_missiles(s6, kObjectEffectExploding, 1, m, 16) == 6);
    assert(m[0].missile == 0x2D && m[0].flags == 0x520 && m[0].targetX == 94 && m[0].targetY == 206 && m[0].level == 1);
    assert(m[5].targetX == 106 && m[5].targetY == 194);
    assert(d2gs_client_object_effect_missiles(s6, kObjectEffectPoison, 99, m, 16) == 6 && m[3].missile == 0x30 && m[3].targetX == 100 && m[3].targetY == 194 && m[3].level == 8);
    assert(d2gs_client_object_effect_missiles(s6, kObjectEffectReverseName, 1, m, 16) == 0 && d2gs_client_object_effect_missiles(s6, kObjectEffectFire, 1, m, 15) == 0);

    // A hidden room takes the unit off its list; one whose room is already freed is only forgotten.
    d2gs_client_object_unplace(s6);
    assert(g_roomUnits == 1 && !UNITS_GetRoom(s6) && d2gs_client_object_find(0x20) == s6);
    d2gs_client_object_unplace(s16, false);
    assert(g_roomUnits == 1 && !UNITS_GetRoom(s16));
    g_roomUnits = 0;
    d2gs_client_object_destroy(s6); d2gs_client_object_destroy(s16); d2gs_client_object_destroy(s2);
    assert(!d2gs_client_object_next(nullptr) && g_roomUnits == 0);

    // The glowing classes: the glow goes on with a server change to idle and off when the clock opens the object.
    D2ObjectsTxt& glow = g_txt[4];
    glow.nOverlay = 1;
    for (int md = 0; md < 8; ++md) { glow.dwFrameCnt[md] = 0x100; glow.wFrameDelta[md] = 0x100; }
    glow.dwFrameCnt[1] = 0x200;
    D2UnitStrc* g = d2gs_client_object_create(0x50, GLOW, 2, 0);
    assert(overlays(g) == 0);
    const uint8_t idleG[12] = { 0x0e, 2, 0x50, 0, 0, 0, 3, 1, 0, 0, 0, 0 }, opG[12] = { 0x0e, 2, 0x50, 0, 0, 0, 3, 0, 1, 0, 0, 0 };
    assert(d2gs_client_object_packet(idleG, 12, player_by_id, &ev) == 1 && overlays(g, 0x47) == 1);
    assert(d2gs_client_object_packet(opG, 12, player_by_id, &ev) == 1 && overlays(g, 0x47) == 1);
    steps_until(g, 2);
    assert(overlays(g) == 0);
    d2gs_client_object_destroy(g);
    return 0;
}
