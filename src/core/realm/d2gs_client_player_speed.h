#pragma once

#include <cstdint>

// The client's own speed bookkeeping for a player unit. The game server never sends a player's
// base velocity to its owner, so the client has to give its unit the same base the server gives its
// own, and has to drop the run bonus when the unit leaves the run mode, or the predicted route and
// the server's drift apart.

// The stats the client's player-unit init sets to 100 on every new player unit: attack rate
// (0x44), velocity percent (0x43) and other-anim rate (0x45). Without the velocity base the unit
// reads 0 and moves at the 25% floor.
inline constexpr int32_t kRealmClientPlayerBaseRateStats[] = { 0x44, 0x43, 0x45 };
inline constexpr int32_t kRealmClientPlayerBaseRate = 100;

template <typename Unit, typename SetStat>
inline void d2_realm_client_player_init_rates(Unit* unit, SetStat&& setStat) {
    for (const int32_t stat : kRealmClientPlayerBaseRateStats) setStat(unit, stat, kRealmClientPlayerBaseRate);
}

// A mode change on a player unit drops its temporary stat lists before the new mode's start-frame
// setup, as the common mode change does. Entering the run mode adds a temporary velocity bonus; a
// change that skips the drop keeps that bonus through the next walk and stacks one more per run.
// `needsSetup` is the caller's "mode differs or the animation was never set up" test.
template <typename Unit, typename DropTemporary, typename StartFrame>
inline void d2_realm_client_unit_enter_mode(Unit* unit, uint32_t mode, bool isPlayer, bool needsSetup,
        DropTemporary&& dropTemporary, StartFrame&& startFrame) {
    if (!needsSetup) return;
    const bool changed = static_cast<uint32_t>(unit->dwAnimMode) != mode;
    unit->dwAnimMode = mode;
    if (isPlayer && changed) dropTemporary(unit);
    startFrame(unit);
}
