// Host test: tools/realm-tests.sh src/core/realm/d2gs_client_object.test.cpp
// A joined game's object unit: the state it is made with, the server's mode changes, the client clock,
// and the walk-collision footprint each of them leaves in the room.
#include "d2gs_client_object.cpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>

// Fake D2Common: three classes, and a footprint log in place of the room's collision grid.
static uint8_t g_txtMem[3 * sizeof(D2ObjectsTxt)];   // zeroed rows; the test fills the columns it reads
static D2ObjectsTxt* const g_txt = reinterpret_cast<D2ObjectsTxt*>(g_txtMem);
enum { DOOR, CHEST, TORCH };
static std::string g_log;   // 'B' for a footprint written, 'F' for one cleared
D2ObjectsTxt* __stdcall DATATBLS_GetObjectsTxtRecord(int id) { return id >= 0 && id < 3 ? &g_txt[id] : nullptr; }
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
    const D2ObjectsTxt* txt = &g_txt[unit->dwClassId];
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
    return 0;
}
