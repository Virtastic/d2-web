// The client unit of a joined game's object: creation, the server's mode change, the per-tick clock.
// The rules are the 1.13c client's (D2Client+0x25490 / +0xA75C0 create, +0x25CE0 state dispatch,
// +0x259F0 / +0x25040 mode change, +0x25950 shrine overlays, +0x24B90 / +0x24A70 activation,
// +0x25660 / +0x25400 / +0x251E0 clock); the collision and clock primitives are D2Common's.
#include "d2gs_client_object.h"

#include <D2Collision.h>
#include <D2DataTbls.h>
#include <Units/Object.h>
#include <Units/UnitRoom.h>
#include <Units/Units.h>
#include <cstring>

namespace {
constexpr uint32_t kModes = 8;
constexpr int32_t kOneCel = 0x100;
enum : uint32_t { kNeutral = 0, kOperating = 1, kOpened = 2, kSpecial1 = 3, kSpecial2 = 4, kSpecial3 = 5 };

// The object data with the client's overlay slots behind it (retail hangs overlays off the unit's draw data).
struct ClientObjectData : D2ObjectDataStrc { int32_t overlay[3] = { -1, -1, -1 }; };
ClientObjectData* data_of(const D2UnitStrc* unit) { return static_cast<ClientObjectData*>(unit->pObjectData); }

// The client's object table: 128 chains keyed by the low 7 bits of the unit id, linked through the unit.
constexpr uint32_t kBuckets = 128;
D2UnitStrc* g_table[kBuckets];

// What each shrine kind does on the client: the two overlays an idle shrine of the kind shows, the sound
// played at the player who activates it, and the activation effect. Indexed by the Shrines table's code;
// kinds 0..5 and 16..22 show no overlay. (The client's 23-entry table, read from the 1.13c image.)
constexpr int kShrineKinds = 23;
struct ShrineKind { int16_t overlayA, overlayB; uint16_t sound; uint8_t effect; };
constexpr ShrineKind kShrine[kShrineKinds] = {
    { -1, -1, 0, 0 },
    { -1, -1, 0xA71, 0 }, { -1, -1, 0xA71, 0 }, { -1, -1, 0xA70, 0 }, { -1, -1, 0xA6A, 0 }, { -1, -1, 0xA6A, 0 },
    { 0x3B, 0x39, 0xA68, 0 }, { 0x3C, 0x39, 0xA69, 0 }, { 0x3E, 0x39, 0xA73, 0 }, { 0x3F, 0x3A, 0xA72, 0 },
    { 0x3D, 0x3A, 0xA74, 0 }, { 0x40, 0x3A, 0xA75, 0 }, { 0x41, 0x39, 0xA77, 0 }, { 0x42, 0x3A, 0xA70, 0 },
    { 0x43, 0x3A, 0xA70, 0 }, { 0x44, 0x39, 0xA6B, 0 },
    { -1, -1, 0xA76, kObjectEffectReverseName },
    { -1, -1, 0xA6F, 0 }, { -1, -1, 0xA6D, 0 },
    { -1, -1, 0xA78, kObjectEffectFire },
    { -1, -1, 0xA6F, 0 },
    { -1, -1, 0xA6C, kObjectEffectExploding },
    { -1, -1, 0xA6E, kObjectEffectPoison },
};
// The overlay the seven glowing-shrine classes show while idle when their class has an overlay column set.
constexpr int32_t kIdleClassOverlay = 0x47;
bool idle_overlay_class(int32_t classId) {
    return classId == 0x162 || classId == 0x163 || classId == 0x164 || classId == 0x18D || classId == 0x195 ||
        classId == 0x196 || classId == 0x197;
}

const D2ObjectsTxt* txt_of(const D2UnitStrc* unit) { return unit->pObjectData->pObjectTxt; }
bool is_shrine(const D2UnitStrc* unit) { return (txt_of(unit)->nSubClass & OBJSUBCLASS_SHRINE) != 0; }
// A shrine's two overlays: each is taken off, and put back only while the shrine is in its idle mode. Run when
// a shrine is made and after each of its mode changes.
void refresh_shrine_overlays(D2UnitStrc* unit) {
    const D2ShrinesTxt* shrine = unit->pObjectData->pShrineTxt;
    if (!shrine || shrine->nCode >= kShrineKinds) return;   // retail stops the game on a kind past its table
    const ShrineKind& kind = kShrine[shrine->nCode];
    const bool idle = unit->dwAnimMode == 0;
    if (kind.overlayA != -1) data_of(unit)->overlay[0] = idle ? kind.overlayA : -1;
    if (kind.overlayB != -1) data_of(unit)->overlay[1] = idle ? kind.overlayB : -1;
}
void set_flag(D2UnitStrc* unit, uint32_t flag, bool on) {
    unit->dwFlags = on ? (unit->dwFlags | flag) : (unit->dwFlags & ~flag);
}
// The footprint lives in the room's collision grid; a unit whose room is not active has none to change.
void block(D2UnitStrc* unit) {
    if (D2ActiveRoomStrc* room = UNITS_GetRoom(unit))
        UNITS_BlockCollisionPath(unit, room, unit->pStaticPath->tGameCoords.nX, unit->pStaticPath->tGameCoords.nY);
}
void unblock(D2UnitStrc* unit) {
    if (UNITS_GetRoom(unit)) UNITS_FreeCollisionPath(unit);
}
// D2Common's mode change: the same mode keeps its clock, a new one starts its run of cels.
void change_mode(D2UnitStrc* unit, uint32_t mode) {
    if (unit->dwAnimMode == mode) return;
    unit->dwAnimMode = mode;
    unit->dwFlags |= UNITFLAG_DOUPDATE;
    UNITS_SetAnimStartFrame(unit);
}
bool open_mode(uint32_t mode) { return mode == kOpened || mode == kSpecial2 || mode == kSpecial3; }

// A door. Opening (0 to 2) and closing (2 to 0) with cels to show go through the transit modes 1 and 3 and
// finish on the clock; the footprint is written at once when a door starts to close and cleared only when
// it has finished opening. Every other change is immediate and takes the packet's selectable byte.
void set_door_mode(D2UnitStrc* unit, uint32_t mode, uint8_t selectable) {
    const D2ObjectsTxt* txt = txt_of(unit);
    const uint32_t current = unit->dwAnimMode;
    if (mode == kNeutral && current == kOpened && txt->dwFrameCnt[kSpecial1] != kOneCel) {
        unit->nSeqCurrentFramePrecise = txt->nStart[kNeutral];
        change_mode(unit, kSpecial1);
        set_flag(unit, UNITFLAG_TARGETABLE, false);
        block(unit);
        return;
    }
    if (mode == kOpened && current == kNeutral && txt->dwFrameCnt[kOperating] != kOneCel) {
        change_mode(unit, kOperating);
        set_flag(unit, UNITFLAG_TARGETABLE, false);
        unit->nSeqCurrentFramePrecise = txt->nStart[kOpened];
        return;
    }
    set_flag(unit, UNITFLAG_TARGETABLE, selectable != 0);
    if (mode != current) {
        if (open_mode(mode)) {
            if (!open_mode(current)) unblock(unit);
        } else if (mode == kNeutral) {
            block(unit);
        }
        change_mode(unit, mode);
    }
    unit->nSeqCurrentFramePrecise = txt->nStart[mode];
}

// A door's transit mode has run out: closing ends closed, opening ends open and clears the footprint.
// The mode is written directly, so the clock's count and step stay those of the transit mode.
void finish_door(D2UnitStrc* unit) {
    const D2ObjectsTxt* txt = txt_of(unit);
    if (unit->dwAnimMode == kSpecial1) {
        unit->dwAnimMode = kNeutral;
    } else {
        unblock(unit);
        unit->dwAnimMode = kOpened;
    }
    unit->nSeqCurrentFramePrecise = txt->nStart[unit->dwAnimMode];
    set_flag(unit, UNITFLAG_TARGETABLE, txt->nSelectable[unit->dwAnimMode] != 0);
}

void step_door(D2UnitStrc* unit) {
    const D2ObjectsTxt* txt = txt_of(unit);
    if (unit->dwAnimMode == kOperating) {
        const int32_t last = static_cast<int32_t>(txt->dwFrameCnt[kOperating]) - kOneCel;
        if (unit->nSeqCurrentFramePrecise != last) {
            unit->nSeqCurrentFramePrecise += unit->wAnimSpeed;
            if (unit->nSeqCurrentFramePrecise > last) unit->nSeqCurrentFramePrecise = last;
            return;
        }
    } else if (unit->dwAnimMode == kSpecial1) {
        if (unit->nSeqCurrentFramePrecise > 0) {
            unit->nSeqCurrentFramePrecise = unit->nSeqCurrentFramePrecise < unit->wAnimSpeed ? 0 :
                unit->nSeqCurrentFramePrecise - unit->wAnimSpeed;
            return;
        }
    } else {
        return;   // retail stops the game here: a door holds cels only in its two transit modes
    }
    finish_door(unit);
}
}

D2UnitStrc* d2gs_client_object_create(uint32_t guid, uint32_t classId, uint32_t mode, uint8_t interact) {
    D2ObjectsTxt* txt = DATATBLS_GetObjectsTxtRecord(static_cast<int>(classId));
    if (!txt || mode >= kModes) return nullptr;
    D2UnitStrc* unit = UNITS_AllocUnit(nullptr, UNIT_OBJECT);
    if (!unit) return nullptr;
    unit->dwClassId = static_cast<int32_t>(classId);
    unit->dwUnitId = guid;
    unit->dwAnimMode = mode;
    unit->pObjectData = new ClientObjectData{};
    unit->pObjectData->pObjectTxt = txt;
    unit->pObjectData->InteractType = interact;
    UNITS_AllocStaticPath(unit);
    unit->dwFlags |= UNITFLAG_ISCLIENTUNIT;
    set_flag(unit, UNITFLAG_TARGETABLE, txt->nSelectable[mode] != 0);
    set_flag(unit, UNITFLAG_CANBEATTACKED, txt->nIsAttackable0 != 0);
    UNITS_SetAnimStartFrame(unit);
    set_flag(unit, UNITFLAG_DRAWSHADOW, txt->nBlocksLight[mode] == 0);
    if (txt->nOrderFlag[mode] == 1) unit->dwFlags |= UNITFLAG_PREDRAW;
    // The assign packet's interact byte names a shrine's record; the shrine then takes its idle overlays.
    if (is_shrine(unit)) {
        unit->pObjectData->pShrineTxt = DATATBLS_GetShrinesTxtRecord(interact);
        refresh_shrine_overlays(unit);
    }
    D2UnitStrc*& head = g_table[guid & (kBuckets - 1)];
    unit->pListNext = head;
    head = unit;
    return unit;
}

void d2gs_client_object_place(D2UnitStrc* unit, D2ActiveRoomStrc* room, int x, int y) {
    if (!unit) return;
    if (UNITS_GetRoom(unit)) UNITROOM_RemoveUnitFromRoom(unit);
    UNITS_InitializeStaticPath(unit, room, x, y);
    if (room) UNITROOM_AddUnitToRoom(unit, room);
    if (txt_of(unit)->nHasCollision[unit->dwAnimMode]) block(unit);
}

void d2gs_client_object_unplace(D2UnitStrc* unit, bool roomAlive) {
    if (!unit) return;
    if (roomAlive && UNITS_GetRoom(unit)) UNITROOM_RemoveUnitFromRoom(unit);
    unit->pStaticPath->pRoom = nullptr;
    unit->pRoomNext = nullptr;
}

void d2gs_client_object_set_mode(D2UnitStrc* unit, uint32_t mode, uint8_t selectable) {
    if (!unit || mode >= kModes) return;
    const D2ObjectsTxt* txt = txt_of(unit);
    if (txt->nIsDoor) {
        set_door_mode(unit, mode, selectable);
    } else {
        change_mode(unit, mode);
        if (txt->nOverlay && idle_overlay_class(unit->dwClassId) && unit->dwAnimMode == 0)
            data_of(unit)->overlay[2] = kIdleClassOverlay;
        set_flag(unit, UNITFLAG_TARGETABLE, txt->nSelectable[mode] != 0);
        if (!txt->nHasCollision[mode]) unblock(unit);
        if (txt->nOrderFlag[mode] == 1) unit->dwFlags |= UNITFLAG_PREDRAW;
    }
    set_flag(unit, UNITFLAG_DRAWSHADOW, txt->nBlocksLight[unit->dwAnimMode] == 0);
}

void d2gs_client_object_step(D2UnitStrc* unit) {
    if (!unit) return;
    const D2ObjectsTxt* txt = txt_of(unit);
    const uint32_t mode = unit->dwAnimMode;
    const int32_t count = static_cast<int32_t>(txt->dwFrameCnt[mode]);
    if (count == kOneCel) return;
    const bool cycles = txt->nCycleAnim[mode] != 0;
    const bool ladder = unit->dwClassId == 0xBD;   // the one class whose opened mode runs on into two more
    if (!cycles) {
        if (txt->nIsDoor) { step_door(unit); return; }
        if (unit->nSeqCurrentFramePrecise >= count - kOneCel) {
            if (ladder && (mode == kOpened || mode == kSpecial1)) {
                unit->dwAnimMode = mode + 1;
                unit->nSeqCurrentFramePrecise = txt->nStart[mode + 1] << 8;
            } else if (mode == kOperating) {
                // Operating runs out into opened; the clock's step stays the operating mode's.
                unit->dwAnimMode = kOpened;
                if (txt->nOverlay && idle_overlay_class(unit->dwClassId)) data_of(unit)->overlay[2] = -1;
                if (txt->nOrderFlag[kOpened] == 1) unit->dwFlags |= UNITFLAG_PREDRAW;
                unit->nSeqCurrentFramePrecise = txt->nStart[kOpened] << 8;
                set_flag(unit, UNITFLAG_TARGETABLE, txt->nSelectable[kOpened] != 0);
                if (!txt->nHasCollision[kOpened] && txt->nHasCollision[kOperating]) unblock(unit);
            }
            return;
        }
    }
    if (unit->dwClassId == 0x0C) {                 // this class runs its cels backwards
        unit->nSeqCurrentFramePrecise -= unit->wAnimSpeed;
        if (unit->nSeqCurrentFramePrecise < 0) unit->nSeqCurrentFramePrecise += count;
    } else if (ladder && mode == kSpecial1) {
        unit->nSeqCurrentFramePrecise -= unit->wAnimSpeed;
        if (unit->nSeqCurrentFramePrecise < 0) {
            unit->dwAnimMode = kSpecial2;
            unit->nSeqCurrentFramePrecise = txt->nStart[kSpecial2] << 8;
        }
    } else {
        unit->nSeqCurrentFramePrecise += unit->wAnimSpeed;
        if (unit->nSeqCurrentFramePrecise >= count)
            unit->nSeqCurrentFramePrecise = cycles ? (txt->nStart[mode] << 8) - count + unit->nSeqCurrentFramePrecise
                                                   : count - kOneCel;
    }
}

void d2gs_client_object_destroy(D2UnitStrc* unit) {
    if (!unit) return;
    for (D2UnitStrc** link = &g_table[unit->dwUnitId & (kBuckets - 1)]; *link; link = &(*link)->pListNext)
        if (*link == unit) { *link = unit->pListNext; break; }
    unit->pListNext = nullptr;
    if (txt_of(unit)->nHasCollision[unit->dwAnimMode]) unblock(unit);
    if (UNITS_GetRoom(unit)) UNITROOM_RemoveUnitFromRoom(unit);
    UNITS_FreeStaticPath(unit);
    unit->pStaticPath = nullptr;
    delete data_of(unit);
    unit->pObjectData = nullptr;
    UNITS_FreeUnit(unit);
}

D2UnitStrc* d2gs_client_object_find(uint32_t guid) {
    for (D2UnitStrc* unit = g_table[guid & (kBuckets - 1)]; unit; unit = unit->pListNext)
        if (unit->dwUnitId == guid) return unit;
    return nullptr;
}

D2UnitStrc* d2gs_client_object_next(const D2UnitStrc* previous) {
    uint32_t bucket = 0;
    if (previous) {
        if (previous->pListNext) return previous->pListNext;
        bucket = (previous->dwUnitId & (kBuckets - 1)) + 1;
    }
    for (; bucket < kBuckets; ++bucket)
        if (g_table[bucket]) return g_table[bucket];
    return nullptr;
}

// GUESS: the three slots are reported as overlay rows alike. The slots differ in how the client's overlay list
// treats them (the 0x47 glow goes in its own slot); the list's draw is not ported, and nothing draws these yet.
int d2gs_client_object_overlays(const D2UnitStrc* unit, int32_t out[3]) {
    int n = 0;
    if (unit && out)
        for (int32_t overlay : data_of(unit)->overlay)
            if (overlay != -1) out[n++] = overlay;
    return n;
}

int d2gs_client_object_effect_missiles(const D2UnitStrc* object, uint32_t effect, int playerLevel,
        D2gsClientObjectMissile* out, int cap) {
    if (!object || !out) return 0;
    const int x = object->pStaticPath->tGameCoords.nX, y = object->pStaticPath->tGameCoords.nY;
    int level = playerLevel / 5;
    level = level < 1 ? 1 : level > 8 ? 8 : level;
    if (effect == kObjectEffectFire) {
        // Four steps each way, alternating sides and growing by 5: 5, -10, 15, -20.
        // GUESS: the two steps are handed on as the target as read, not added to the object's position; the
        // missile creator's use of that pair (a point or a direction) is not ported. Porting it settles this.
        if (cap < 16) return 0;
        static const int kStep[4] = { 5, -10, 15, -20 };
        int n = 0;
        for (int dx : kStep)
            for (int dy : kStep) out[n++] = { 0x3E, 3u, x, y, dx, dy, level };
        return n;
    }
    if (effect == kObjectEffectExploding || effect == kObjectEffectPoison) {
        if (cap < 6) return 0;
        static const int kDx[6] = { -6, -6, 0, 0, 6, 6 }, kDy[6] = { 6, -6, 6, -6, 6, -6 };
        const int missile = effect == kObjectEffectExploding ? 0x2D : 0x30;
        for (int i = 0; i < 6; ++i) out[i] = { missile, 0x520u, x, y, x + kDx[i], y + kDy[i], level };
        return 6;
    }
    return 0;
}

void d2gs_client_object_reverse_name(char name[16]) {
    char copy[16];
    std::memcpy(copy, name, sizeof copy);
    copy[15] = 0;
    const size_t length = std::strlen(copy);
    for (size_t i = 0; i < length; ++i) name[i] = copy[length - 1 - i];
}

namespace {
// A player activated the object. The kind is the shrine record's code for a shrine class and the class's
// shrine-function column for any other; the sound is always chosen by the object's shrine record.
int activate(D2UnitStrc* unit, uint32_t playerGuid, D2UnitStrc* (*player)(uint32_t), D2gsClientObjectEvent* event) {
    const D2ShrinesTxt* shrine = unit->pObjectData->pShrineTxt;
    const uint32_t kind = is_shrine(unit) ? (shrine ? shrine->nCode : 0) : txt_of(unit)->nShrineFunction;
    D2UnitStrc* target = player ? player(playerGuid) : nullptr;
    if (!target) return 0;                                   // an unknown player: nothing happens
    if (target->dwUnitType != UNIT_PLAYER) return 0;         // retail stops the game here
    // Retail stops the game on an object with no shrine record or a record past the table; nothing is applied here.
    if (!shrine || shrine->nCode >= kShrineKinds) return 0;
    uint32_t effect = kObjectEffectNone;
    if (kind > 0 && kind < kShrineKinds) effect = kShrine[kind].effect;
    if (effect == kObjectEffectReverseName && target->pPlayerData)
        d2gs_client_object_reverse_name(target->pPlayerData->szName);
    if (event) *event = { unit->dwUnitId, playerGuid, kind, kShrine[shrine->nCode].sound, effect };
    return 2;
}
}

int d2gs_client_object_packet(const uint8_t* packet, int length, D2UnitStrc* (*player)(uint32_t guid),
        D2gsClientObjectEvent* event) {
    if (!packet || length < 1) return -1;
    if (packet[0] != 0x0e && packet[0] != 0x4d) return 0;
    if (length != (packet[0] == 0x0e ? 12 : 17)) return -1;
    if (packet[1] != UNIT_OBJECT) return 0;
    uint32_t guid = 0, value = 0;
    std::memcpy(&guid, packet + 2, sizeof guid);
    D2UnitStrc* unit = d2gs_client_object_find(guid);
    if (!unit) return 0;
    // GUESS: which of the two packets a server sends when a shrine is used. Both reach the same activation in
    // the client, so both are applied; the recorded joined-game streams hold neither. A stream recorded while a
    // shrine is used settles it.
    if (packet[0] == 0x4d) {
        // The unit-action packet always asks for action 0x15; for an object its dword at 6 is the player's id.
        std::memcpy(&value, packet + 6, sizeof value);
        return activate(unit, value, player, event);
    }
    // The object-state packet: byte 6 is the action, byte 7 and the dword at 8 its two arguments.
    std::memcpy(&value, packet + 8, sizeof value);
    switch (packet[6]) {
    case 3:
        d2gs_client_object_set_mode(unit, value, packet[7]);
        if (is_shrine(unit)) refresh_shrine_overlays(unit);
        return 1;
    case 0x15:
        return activate(unit, packet[7], player, event);     // the player's id is the packet's one byte
    default:
        return 0;   // an object takes no other action: retail stops the game on any of them
    }
}
