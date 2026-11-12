// The stat entries of a 0xA8 / 0xAA state packet, and the room friendly-unit count an
// alignment stat moves.
#include "d2gs_client_state_stats.h"

#include <D2DataTbls.h>
#include <D2StatList.h>
#include <D2Dungeon.h>
#include <Path/Path.h>
#include <Units/Units.h>

// A monster linked into a room was counted as friendly or not by the alignment it had when
// it was linked, and unlinking re-reads the alignment. So when the alignment stat changes on a monster already in a room, the
// room's count follows it: +1 on becoming good, -1 on leaving good, then the
// unit's collision footprint is rebuilt. Players and other unit types are untouched.
static void d2gs_client_alignment_changed(D2UnitStrc* unit, uint8_t previous, uint8_t value) {
    if (!unit || unit->dwUnitType != UNIT_MONSTER || previous == value) return;
    D2ActiveRoomStrc* room = UNITS_GetRoom(unit);
    if (!room) return;
    if (value == UNIT_ALIGNMENT_GOOD) {
        DUNGEON_IncreaseAlliedCountOfRoom(room);
    } else if (previous == UNIT_ALIGNMENT_GOOD) {
        DUNGEON_DecreaseAlliedCountOfRoom(room);
    }
    D2Common_10214(unit);
}

// Run once per decoded stat entry (an immediate 0x1ff terminator
// therefore allocates nothing). The first entry finds the state's list, or makes one and posts
// it to the unit; only a list that already existed yields the stat's previous value.
// Later entries reuse the list and keep the default previous value: 4 for the alignment stat
// (no alignment), 0 otherwise. The new value is written before the alignment check.
// As retail's client does it, the new list has no flags and is posted with full propagation, so
// damage stats a state carries (an aura's fire damage) reach the unit; a stat whose
// ItemStatCost row has the update-anim-rate flag (velocity, attack rate) then refreshes the
// unit's animation rate and walk speed at once.
static bool d2gs_client_stat_updates_anim_rate(int statId) {
    if (!sgptDataTables || statId < 0 || statId >= sgptDataTables->nItemStatCostTxtRecordCount) return false;
    return (sgptDataTables->pItemStatCostTxt[statId].dwItemStatFlags & ITEMSTATCOSTFLAG_UPDATEANIMRATE) != 0;
}

bool d2gs_client_apply_state_stats(D2UnitStrc* unit, uint8_t state,
    const D2RealmClientStateStat* stats, size_t count) {
    D2StatListStrc* list = nullptr;
    for (size_t i = 0; i < count; ++i) {
        const D2RealmClientStateStat& stat = stats[i];
        int32_t previous = stat.statId == STAT_ALIGNMENT ? 4 : 0;
        if (!list) {
            list = STATLIST_GetStatListFromUnitAndState(unit, state);
            if (list) {
                previous = STATLIST_GetStatValue(list, stat.statId, stat.layer);
            } else {
                list = STATLIST_AllocStatList(unit->pMemoryPool, 0, 0,
                    unit->dwUnitType, unit->dwUnitId);
                if (!list) return false;
                STATLIST_SetState(list, state);
                D2COMMON_10475_PostStatToStatList(unit, list, TRUE);
                list = STATLIST_GetStatListFromUnitAndState(unit, state);
                if (!list) return false;
            }
        }
        STATLIST_SetStat(list, stat.statId, stat.value, stat.layer);
        if (stat.statId == STAT_ALIGNMENT)
            d2gs_client_alignment_changed(unit, static_cast<uint8_t>(previous),
                static_cast<uint8_t>(stat.value));
        if (d2gs_client_stat_updates_anim_rate(stat.statId))
            UNITS_UpdateAnimRateAndVelocity(unit, __FILE__, __LINE__);
    }
    return true;
}
