#pragma once

#include <cstdint>

namespace d2::object_clock {

// Retail's D2Client object stepper (D2Client+0x25660, policy rows 0x6FBA2268), per object mode: a cycling mode
// wraps; a non-cycling mode clamps and never wraps; a non-cycling OPERATING (1) hands off to OPENED (2) once its
// last cel is reached. A non-cycling single-cel mode (a shrine's opened mode: one cel shown at Start2) holds its
// start cel -- the wrap used to count it down a cel per tick, replaying the use in reverse.
struct State { uint32_t shown = ~0u; int32_t clock = 0, count = 0; int16_t speed = 0; };

// Advances one client tick. Returns true when the caller must switch to mode 2 (and reload its record); the
// clock is then unchanged.
inline bool step(State& c, bool isDoor, bool cycling) {
    if (!isDoor && !cycling && c.count > 256) {
        if (c.shown == 1 && c.clock >= c.count - 256) return true;
        c.clock += c.speed;
        if (c.clock >= c.count) c.clock = c.count - 256;
        return false;
    }
    if (!isDoor && !cycling) return false;
    c.clock += c.speed;
    if (c.count > 0 && c.clock >= c.count) c.clock -= c.count;
    return false;
}

}  // namespace d2::object_clock
