// The client unit of a joined game's object: creation, the server's mode change, the per-tick clock.
// The rules are the 1.13c client's (D2Client+0x25490 create, +0x259F0 / +0x25040 mode change,
// +0x25660 / +0x25400 / +0x251E0 clock); the collision and clock primitives are D2Common's.
#include "d2gs_client_object.h"

#include <D2Collision.h>
#include <D2DataTbls.h>
#include <Units/Object.h>
#include <Units/Units.h>

namespace {
constexpr uint32_t kModes = 8;
constexpr int32_t kOneCel = 0x100;
enum : uint32_t { kNeutral = 0, kOperating = 1, kOpened = 2, kSpecial1 = 3, kSpecial2 = 4, kSpecial3 = 5 };

const D2ObjectsTxt* txt_of(const D2UnitStrc* unit) { return unit->pObjectData->pObjectTxt; }
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
    unit->pObjectData = new D2ObjectDataStrc{};
    unit->pObjectData->pObjectTxt = txt;
    unit->pObjectData->InteractType = interact;
    UNITS_AllocStaticPath(unit);
    unit->dwFlags |= UNITFLAG_ISCLIENTUNIT;
    set_flag(unit, UNITFLAG_TARGETABLE, txt->nSelectable[mode] != 0);
    set_flag(unit, UNITFLAG_CANBEATTACKED, txt->nIsAttackable0 != 0);
    UNITS_SetAnimStartFrame(unit);
    set_flag(unit, UNITFLAG_DRAWSHADOW, txt->nBlocksLight[mode] == 0);
    if (txt->nOrderFlag[mode] == 1) unit->dwFlags |= UNITFLAG_PREDRAW;
    return unit;
}

void d2gs_client_object_place(D2UnitStrc* unit, D2ActiveRoomStrc* room, int x, int y) {
    if (!unit) return;
    UNITS_InitializeStaticPath(unit, room, x, y);
    if (txt_of(unit)->nHasCollision[unit->dwAnimMode]) block(unit);
}

void d2gs_client_object_unplace(D2UnitStrc* unit) {
    if (unit) unit->pStaticPath->pRoom = nullptr;
}

void d2gs_client_object_set_mode(D2UnitStrc* unit, uint32_t mode, uint8_t selectable) {
    if (!unit || mode >= kModes) return;
    const D2ObjectsTxt* txt = txt_of(unit);
    if (txt->nIsDoor) {
        set_door_mode(unit, mode, selectable);
    } else {
        change_mode(unit, mode);
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
    if (txt_of(unit)->nHasCollision[unit->dwAnimMode]) unblock(unit);
    UNITS_FreeStaticPath(unit);
    unit->pStaticPath = nullptr;
    delete unit->pObjectData;
    unit->pObjectData = nullptr;
    UNITS_FreeUnit(unit);
}
