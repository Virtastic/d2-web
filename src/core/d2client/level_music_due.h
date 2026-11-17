// 1.13c D2Client+0x45660: when a different music selection is adopted. The wanted id replaces the active one once the
// active one has stood for 0x4b ticks, or nothing is playing (ok false) or no selection is active yet (id 0).
#pragma once
#include <cstdint>
inline bool d2client_music_selection_due(uint32_t active, uint32_t wanted, bool ok, uint32_t age) {
    return active != wanted && (active == 0 || !ok || age >= 0x4b);
}
