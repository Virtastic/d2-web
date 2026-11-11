// 1.13c D2Client+0xA1860 (with +0xA0750, +0xA1620, +0xA09F0, +0xA0740, +0xA1A50, +0xA1AD0, +0x505F0)
// The client's area scan. The rooms adjacent to the unit's room are visited in list order; a room
// is skipped when filter 0x2000 is set and the room is in town, or when the square of `radius`
// around the point misses the room's rectangle (+0xA0750). In each room the units are taken from
// the room list's head along their room links, the scanning unit excepted; a unit within the
// radius (squared distance) that passes the aura filter (+0xA1620), and is not a no-multishot
// monster when that is asked, is handed to the callback, and the context counts the callbacks that
// returned nonzero.
#include <Units/Units.h>
#include <D2Dungeon.h>
#include <D2Collision.h>
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <Path/Path.h>
#include <Drlg/D2DrlgDrlg.h>
#include "realm_area.h"

// +0xA0750 with the room's rectangle {x0, y0, w, h}: the x test drops a room only when the point
// lies both left of its left edge and right of its right edge, as the retail compare does.
static int room_in_reach(D2ActiveRoomStrc* room, int x, int y, int r) {
    D2DrlgCoordsStrc rect{};
    DUNGEON_GetRoomCoordinates(room, &rect);
    if (x + r < rect.nSubtileX && x - r > rect.nSubtileX + rect.nSubtileWidth) return 0;
    if (y + r >= rect.nSubtileY) return 1;
    return y - r <= rect.nSubtileY + rect.nSubtileHeight;
}

// NOT PORTED: filter flags 4 (undead), 0x4000, 0x10000, 0x40000 and 0x80000 and the object, missile
// and item types (0x10, 8, 0x20); no search the ported skills run sets them, and they pass nothing.
int d2client_realm_a1620(uint32_t flags, D2UnitStrc* c, D2UnitStrc* origin, const RealmSkillDoHost& host) {
    if (!c) return 0;
    const uint32_t mode = c->dwAnimMode;
    if (c->dwUnitType == UNIT_PLAYER) {
        if (!(flags & 1u)) return 0;
        if (flags & 0x1000u) { if (mode != PLRMODE_DEAD) return 0; }
        else if (mode == PLRMODE_DEATH || mode == PLRMODE_DEAD) return 0;
    } else if (c->dwUnitType == UNIT_MONSTER) {
        if (!(flags & 2u)) return 0;
        if (flags & 0x1000u) { if (mode != MONMODE_DEAD) return 0; }
        else if (mode == MONMODE_DEATH || mode == MONMODE_DEAD) return 0;
    } else {
        return 0;
    }
    if ((flags & 0x80u) && !(c->dwFlags & 4u)) return 0;
    if ((flags & 0x400u) && !(c->dwFlags & 8u)) return 0;
    if (flags & 0x100u) {
        D2ActiveRoomStrc* room = UNITS_GetRoom(c);
        if (room && DUNGEON_IsRoomInTown(room)) return 0;
    }
    if ((flags & 0x8000u) && !(host.hostile && host.hostile(c, origin))) return 0;
    if ((flags & 0x20000u) && !UNITS_IsInMeleeRange(origin, c, 0)) return 0;
    if (flags & 0x200u) {
        D2ActiveRoomStrc* from = UNITS_GetRoom(origin);
        if (!from || !UNITS_GetRoom(c)) return 0;
        D2CoordStrc a{UNITS_GetXPosition(origin), UNITS_GetYPosition(origin)};
        D2CoordStrc b{UNITS_GetXPosition(c), UNITS_GetYPosition(c)};
        if (COLLISION_RayTrace(from, &a, &b, 4)) return 0;
    }
    return 1;
}

void d2client_realm_a1860(D2UnitStrc* unit, int x, int y, int radius, uint32_t flags, RealmAreaCallback cb,
        void* value, int noMultishot, const RealmSkillDoHost& host) {
    if (!unit || !cb || !radius) return;
    D2ActiveRoomStrc* room = UNITS_GetRoom(unit);
    if (!room) return;
    if (!x || !y) { x = UNITS_GetXPosition(unit); y = UNITS_GetYPosition(unit); }
    if (!flags) flags = 0x583;
    RealmAreaContext ctx{unit, 0, value};
    const int r2 = radius * radius;
    D2ActiveRoomStrc** list = nullptr;
    int rooms = 0;
    DUNGEON_GetAdjacentRoomsListFromRoom(room, &list, &rooms);
    for (int i = 0; i < rooms; ++i) {
        D2ActiveRoomStrc* near = list[i];
        if (!near) continue;
        if ((flags & 0x2000u) && DUNGEON_IsRoomInTown(near)) continue;
        if (!room_in_reach(near, x, y, radius)) continue;
        for (D2UnitStrc* c = near->pUnitFirst; c;) {
            D2UnitStrc* next = c->pRoomNext;
            if (c != unit &&
                PATH_ComputeSquaredDistance(x, y, UNITS_GetXPosition(c), UNITS_GetYPosition(c)) <= r2 &&
                d2client_realm_a1620(flags, c, unit, host)) {
                bool skip = false;
                if (noMultishot && c->dwUnitType == UNIT_MONSTER) {
                    // +0x1760 with bit 0x1B of the MonStats flags: the no-multishot column.
                    const D2MonStatsTxt* txt = DATATBLS_GetMonStatsTxtRecord(c->dwClassId);
                    skip = txt && (txt->dwMonStatsFlags & MONSTATSFLAG_NOMULTISHOT);
                }
                if (!skip && cb(&ctx, c)) ++ctx.count;
            }
            c = next;
        }
    }
}

int d2client_realm_a09f0(RealmAreaContext* ctx, D2UnitStrc* c) {
    auto* t = static_cast<RealmNextTarget*>(ctx->value);
    const uint32_t g = c ? c->dwUnitId : 0xFFFFFFFFu;
    if (g > t->floor && g < t->bestGuid) { t->bestGuid = g; t->best = c; }
    else if (g <= t->lowGuid) { t->lowGuid = g; t->low = c; }
    ++t->count;
    return 1;
}

int d2client_realm_a0740(RealmAreaContext* ctx, D2UnitStrc*) {
    ++*static_cast<int*>(ctx->value);
    return 1;
}

D2UnitStrc* d2client_realm_a1a50(uint32_t after, uint32_t flags, int range, D2UnitStrc* unit, int* count,
        const RealmSkillDoHost& host) {
    if (!flags) flags = 0x583;
    RealmNextTarget t(after);
    d2client_realm_a1860(unit, 0, 0, range, flags | 0xA280u, d2client_realm_a09f0, &t, 0, host);
    if (count) *count = t.count;
    return t.result();
}

int d2client_realm_a1ad0(uint32_t flags, int range, D2UnitStrc* unit, const RealmSkillDoHost& host) {
    int count = 0;
    d2client_realm_a1860(unit, 0, 0, range, flags | 0xA783u, d2client_realm_a0740, &count, 0, host);
    return count;
}

void d2client_realm_505f0(D2UnitStrc* unit, int percent) {
    if (!unit) return;
    const int32_t frame = unit->nSeqCurrentFramePrecise >> 8;
    unit->nSeqCurrentFramePrecise = static_cast<int32_t>(static_cast<uint32_t>((100 - percent) * frame / 100) << 8);
}
