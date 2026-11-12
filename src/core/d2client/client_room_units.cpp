// Client room unit lists: the per-room unit chain D2Client walks in its room callback (1.13c D2Client+0x7A5D0) through
// D2Common #10667 (D2Common+0x3C640, DUNGEON_GetFirstUnitInRoom), which runs D2Common+0x664B0 over the room's list
// before returning its head. The chain is retail's own layout: room +0x74 the first unit, unit +0xE8 the next one,
// unit +0x2C the path whose client Y (static path +8 for objects/items/tiles, dynamic path +0xC otherwise) is the
// 664B0 sort key. A unit joins its room at the HEAD (D2Common+0x66900, UNITROOM_AddUnitToRoomEx); a room change is
// an unlink from the old room and a head insert into the new one.
// Guest pointers are 32-bit (the 664B0 port reads them as uint32_t): the records live in static storage.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include <D2DataTbls.h>
#include <DataTbls/ObjectsTbls.h>
#include <Units/Units.h>

extern "C" void d2common_sub_664b0(uint32_t pStruct);

namespace {
constexpr uint32_t kMaxUnits = 2048, kMaxRooms = 512;
struct alignas(8) CUnit { uint8_t b[0xF0]; };
struct alignas(8) CPath { uint8_t b[0x20]; };
struct alignas(8) CRoom { uint8_t b[0x80]; };
CUnit g_unit[kMaxUnits]; CPath g_path[kMaxUnits]; CRoom g_room[kMaxRooms];
struct UnitMeta { uint8_t used, seen; int32_t room; uint32_t type, id; int32_t x, y; uint32_t aux; };
struct RoomMeta { uint8_t used; int32_t level, tileX, tileY; };
UnitMeta g_um[kMaxUnits]; RoomMeta g_rm[kMaxRooms];

inline uint32_t A(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }
inline uint32_t& W(void* b, int o) { return *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(b) + o); }
inline int32_t slotOf(uint32_t p) {
    if (!p) return -1;
    uintptr_t d = static_cast<uintptr_t>(p) - reinterpret_cast<uintptr_t>(&g_unit[0]);
    return static_cast<int32_t>(d / sizeof(CUnit));
}
// UNITROOM_RemoveUnitFromRoom: unlink the unit from its room's +0x74 / +0xE8 chain.
void unlink(int32_t s) {
    int32_t r = g_um[s].room;
    if (r < 0) return;
    uint32_t self = A(&g_unit[s]), *pp = &W(&g_room[r], 0x74);
    while (*pp && *pp != self) pp = &W(&g_unit[slotOf(*pp)], 0xE8);
    if (*pp) *pp = W(&g_unit[s], 0xE8);
    W(&g_unit[s], 0xE8) = 0;
    g_um[s].room = -1;
}
// D2Common+0x66900: head insert.
void link(int32_t s, int32_t r) {
    W(&g_unit[s], 0xE8) = W(&g_room[r], 0x74);
    W(&g_room[r], 0x74) = A(&g_unit[s]);
    g_um[s].room = r;
}
int32_t roomFind(int32_t level, int32_t tx, int32_t ty, bool make) {
    int32_t freeSlot = -1;
    for (uint32_t i = 0; i < kMaxRooms; i++) {
        if (g_rm[i].used && g_rm[i].level == level && g_rm[i].tileX == tx && g_rm[i].tileY == ty) return (int32_t)i;
        if (freeSlot < 0 && (!g_rm[i].used || !W(&g_room[i], 0x74))) freeSlot = (int32_t)i;
    }
    if (!make || freeSlot < 0) return -1;
    memset(&g_room[freeSlot], 0, sizeof(CRoom));
    g_rm[freeSlot] = {1, level, tx, ty};
    return freeSlot;
}
}  // namespace

extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_cru_reset() {
    memset(g_unit, 0, sizeof g_unit); memset(g_path, 0, sizeof g_path); memset(g_room, 0, sizeof g_room);
    memset(g_um, 0, sizeof g_um); memset(g_rm, 0, sizeof g_rm);
    for (auto& m : g_um) m.room = -1;
}
// A frame's unit set begins: every unit not set again before d2c_cru_end leaves its room (the client freed it).
EMSCRIPTEN_KEEPALIVE void d2c_cru_begin() { for (auto& m : g_um) m.seen = 0; }
// One client unit: type, id, class, subtile x/y, mode, the fixed-point fraction of a dynamic path's position (0x8000 = the
// subtile centre PATH_ToFP16Center gives a freshly placed unit), unit +0xC4 / +0xC8, and the room it stands in (level, room tile
// origin; level < 0 = no room). Returns the unit's slot, -1 when the table is full.
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_unit(uint32_t type, uint32_t id, int32_t cls, int32_t x, int32_t y, uint32_t mode,
                                          uint32_t fx, uint32_t fy, uint32_t flags, uint32_t flagsEx, int32_t level, int32_t rtx, int32_t rty) {
    int32_t s = -1, freeSlot = -1;
    for (uint32_t i = 0; i < kMaxUnits; i++) {
        if (g_um[i].used && g_um[i].type == type && g_um[i].id == id) { s = (int32_t)i; break; }
        if (freeSlot < 0 && !g_um[i].used) freeSlot = (int32_t)i;
    }
    if (s < 0) {
        if (freeSlot < 0) return -1;
        s = freeSlot;
        memset(&g_unit[s], 0, sizeof(CUnit)); memset(&g_path[s], 0, sizeof(CPath));
        g_um[s] = {1, 0, -1, type, id, 0, 0, 0};
    }
    CUnit& u = g_unit[s];
    W(&u, 0x00) = type; W(&u, 0x04) = (uint32_t)cls; W(&u, 0x0C) = id; W(&u, 0x10) = mode;
    W(&u, 0x2C) = A(&g_path[s]); W(&u, 0xC4) = flags; W(&u, 0xC8) = flagsEx;
    // Client Y: static path +8 = DUNGEON_GameSubtileToClientCoords (8*(x+y)); dynamic path +0xC = DUNGEON_GameToClientCoords
    // over the fp16 position >> 11 (PATH_UpdateClientCoords).
    if (type == 2 || type == 4 || type == 5) { W(&g_path[s], 0x04) = (uint32_t)(16 * (x - y)); W(&g_path[s], 0x08) = (uint32_t)(8 * (x + y)); }
    else {
        int32_t px = (int32_t)(((uint32_t)x << 16) + (fx & 0xFFFF)) >> 11, py = (int32_t)(((uint32_t)y << 16) + (fy & 0xFFFF)) >> 11;
        W(&g_path[s], 0x08) = (uint32_t)((px - py) >> 1); W(&g_path[s], 0x0C) = (uint32_t)((px + py) >> 2);
    }
    g_um[s].x = x; g_um[s].y = y; g_um[s].seen = 1;
    int32_t r = level < 0 ? -1 : roomFind(level, rtx, rty, true);
    if (r != g_um[s].room) { unlink(s); if (r >= 0) link(s, r); }
    return s;
}
// End of the frame's unit set: units not seen leave their room and free their slot.
EMSCRIPTEN_KEEPALIVE void d2c_cru_end() {
    for (uint32_t i = 0; i < kMaxUnits; i++) if (g_um[i].used && !g_um[i].seen) { unlink((int32_t)i); g_um[i].used = 0; }
}
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_room(int32_t level, int32_t tx, int32_t ty) { return roomFind(level, tx, ty, false); }

// The frame's client rooms as subtile rectangles (the browser's room records, cleared by d2c_cru_frame_begin), so a unit is
// placed in the room its subtile falls in without the caller choosing one.
struct RoomRect { int32_t level, x, y, w, h; };
static RoomRect g_rect[kMaxRooms]; static uint32_t g_rectN;
EMSCRIPTEN_KEEPALIVE void d2c_cru_frame_begin() { g_rectN = 0; d2c_cru_begin(); }
EMSCRIPTEN_KEEPALIVE void d2c_cru_room_rect(int32_t level, int32_t sx, int32_t sy, int32_t w, int32_t h) {
    if (g_rectN < kMaxRooms) g_rect[g_rectN++] = {level, sx, sy, w, h};
}
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_unit_at(uint32_t type, uint32_t id, int32_t cls, int32_t x, int32_t y, uint32_t mode,
                                             uint32_t fx, uint32_t fy, uint32_t flags, uint32_t flagsEx) {
    int32_t level = -1, rx = 0, ry = 0;
    for (uint32_t i = 0; i < g_rectN; i++) {
        const RoomRect& r = g_rect[i];
        if (x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h) { level = r.level; rx = r.x / 5; ry = r.y / 5; break; }
    }
    return d2c_cru_unit(type, id, cls, x, y, mode, fx, fy, flags, flagsEx, level, rx, ry);
}
// The frame's unit set is in: units not set leave their rooms, and every room holding a unit gets the one #10667 sort
// pass D2Client 7A5D0 gives it when it walks the room.
EMSCRIPTEN_KEEPALIVE void d2c_cru_frame_end() {
    d2c_cru_end();
    for (uint32_t r = 0; r < kMaxRooms; r++) if (g_rm[r].used && W(&g_room[r], 0x74)) d2common_sub_664b0(A(&g_room[r]));
}
// #10667 as D2Client 7A5D0 calls it: sort the room's list (664B0), return its first unit's slot.
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_first_sorted(int32_t r) {
    if (r < 0 || r >= (int32_t)kMaxRooms) return -1;
    d2common_sub_664b0(A(&g_room[r]));
    return slotOf(W(&g_room[r], 0x74));
}
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_first(int32_t r) { return r < 0 || r >= (int32_t)kMaxRooms ? -1 : slotOf(W(&g_room[r], 0x74)); }
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_next(int32_t s) { return s < 0 || s >= (int32_t)kMaxUnits ? -1 : slotOf(W(&g_unit[s], 0xE8)); }
EMSCRIPTEN_KEEPALIVE uint32_t d2c_cru_aux(int32_t s) { return s < 0 || s >= (int32_t)kMaxUnits ? 0 : g_um[s].aux; }
EMSCRIPTEN_KEEPALIVE void d2c_cru_set_aux(int32_t s, uint32_t v) { if (s >= 0 && s < (int32_t)kMaxUnits) g_um[s].aux = v; }
// Position of the slot in its room's list (0 = head), -1 when it is in no room.
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_rank(int32_t s) {
    if (s < 0 || s >= (int32_t)kMaxUnits || g_um[s].room < 0) return -1;
    int32_t k = 0;
    for (int32_t t = slotOf(W(&g_room[g_um[s].room], 0x74)); t >= 0 && k < (int32_t)kMaxUnits; t = d2c_cru_next(t), k++) if (t == s) return k;
    return -1;
}
// 1.13c D2Client+0xA6180 over the slot's own fields, with the two table reads it makes: D2Client+0x1510 (the monster's
// MonStats2 row via MonStats +0x18; flags +4 bit 20, UNFLATDEAD) and Ordinal_10688 (objects.txt +0x1B7, DrawUnder).
EMSCRIPTEN_KEEPALIVE int32_t d2c_cru_a6180(int32_t s) {
    if (s < 0 || s >= (int32_t)kMaxUnits || !g_um[s].used) return 0;
    CUnit& u = g_unit[s];
    uint32_t type = W(&u, 0), mode = W(&u, 0x10), cls = W(&u, 4);
    switch (type) {
    case 0: return mode == 0x11;
    case 1: {
        if (mode != 0xC) return 0;
        D2MonStats2Txt* m2 = UNITS_GetMonStats2TxtRecordFromMonsterId((int)cls);
        return !(m2 && (m2->dwFlags >> 20) & 1);
    }
    case 2: {
        D2ObjectsTxt* o = DATATBLS_GetObjectsTxtRecord((int)cls);
        uint32_t f = o ? o->nDrawUnder : 0;
        return (f & 2) || (mode == 2 && (f & 1));
    }
    case 3: return (W(&u, 0xC4) & 0x10000) != 0;
    case 4: return mode == 3;
    }
    return 0;
}
}
