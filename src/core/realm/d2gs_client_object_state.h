#pragma once

// A joined game's objects animate on the client: the server sends a mode (0x51 assign, 0x0E) and never the end of
// an operating sequence. Each object runs the client object stepper (object_client_clock.h) once per tick; an
// operating sequence that does not loop hands off to the opened mode on its last cel.

#include <algorithm>
#include <cstdint>
#include <vector>

#include "../monspawn/object_client_clock.h"

inline constexpr uint32_t kRealmObjectModeOpened = 2;

// One Objects.txt mode of one object class: FrameCnt, Start << 8, FrameDelta, CycleAnim, and the class's IsDoor.
struct D2RealmObjectTiming { int32_t count = 0, start = 0; int16_t speed = 0; bool cycling = false, isDoor = false; };
struct D2RealmObjectClock { uint32_t guid; d2::object_clock::State state; };

inline void d2_realm_object_clock_drop(std::vector<D2RealmObjectClock>& clocks, uint32_t guid) {
    clocks.erase(std::remove_if(clocks.begin(), clocks.end(), [guid](const D2RealmObjectClock& c) { return c.guid == guid; }), clocks.end());
}

// The object entered `mode`: its clock restarts at that mode's start cel.
inline void d2_realm_object_clock_set(std::vector<D2RealmObjectClock>& clocks, uint32_t guid, uint32_t mode, const D2RealmObjectTiming& t) {
    d2_realm_object_clock_drop(clocks, guid);
    clocks.push_back({ guid, { mode, t.start, t.count, t.speed, t.start } });
}

// One client tick for every object. `timing(guid, mode)` gives the mode's table row; `opened(guid)` is called when
// an operating sequence ends, after which the object runs its opened mode.
template <class Timing, class Opened>
void d2_realm_object_clock_tick(std::vector<D2RealmObjectClock>& clocks, Timing timing, Opened opened) {
    for (D2RealmObjectClock& c : clocks) {
        const D2RealmObjectTiming t = timing(c.guid, c.state.shown);
        if (!d2::object_clock::step(c.state, t.isDoor, t.cycling)) continue;
        opened(c.guid);
        const D2RealmObjectTiming on = timing(c.guid, kRealmObjectModeOpened);
        c.state = { kRealmObjectModeOpened, on.start, on.count, on.speed, on.start };
    }
}
