#pragma once

// The client's own skill bookkeeping for a player unit.
//
// The client's player-unit init gives every new player unit a skill list before any packet names
// a skill: Attack, the class's base skills, and Attack in each hand that holds nothing. The
// server's skill list packet only adds the skills with points in them and its hand packet only
// changes a hand, so a join that carries neither still leaves a unit whose hands can be read.
// A unit made without the list has no hands at all, and reading one is fatal.
//
// `ops` is the engine's skill interface: hasList(unit), allocList(unit) -> bool,
// initList(unit) (the common init: Attack, base skills, empty hands to Attack), left(unit) and
// right(unit) (the hand's skill; only valid on a unit with a list).

template <typename Unit, typename Ops>
inline bool d2_realm_client_player_init_skills(Unit* unit, const Ops& ops) {
    if (!unit) return false;
    if (!ops.hasList(unit) && !ops.allocList(unit)) return false;
    ops.initList(unit);
    return true;
}

// The left (hand 0) or right (hand 1) skill, or null for a unit with no list: a click then starts
// nothing instead of reading a list that is not there.
template <typename Unit, typename Ops>
inline auto d2_realm_client_hand_skill(Unit* unit, int hand, const Ops& ops) -> decltype(ops.right(unit)) {
    if (!unit || !ops.hasList(unit)) return nullptr;
    return hand ? ops.right(unit) : ops.left(unit);
}
