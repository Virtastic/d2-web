#pragma once

#include <cstdint>

// Mirror the terminal clock policies selected by D2Client's monster CUnit pass
// for DT (mode 0) and DD (mode 12). The caller supplies Common #10426's result.
template <typename Unit, typename IsAtEnd>
inline bool d2_realm_client_apply_monster_death_terminal_clock(Unit& unit, IsAtEnd&& isAtEnd) {
    if (unit.dwUnitType != 1 || (unit.dwAnimMode != 0 && unit.dwAnimMode != 12)) return false;
    if (!isAtEnd(&unit)) return false;

    if (unit.dwAnimMode == 0) {
        unit.nSeqCurrentFramePrecise = static_cast<int32_t>(unit.dwFrameCountPrecise - 0x100u);
        unit.nActionFrame = 0;
        unit.dwFlags |= 0x20u;
    }
    // DD policy 1 holds the current frame. DT policy 2 clamps to the final cel.
    return true;
}
