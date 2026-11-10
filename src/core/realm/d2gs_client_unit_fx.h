#pragma once
// The one queue of what a unit's skill start, hit or server record makes the page sound and draw.
// kind 1, 3, 4: a sound row due `delay` ticks after `tick` (a skill's start sound or a record's
//               own sound, the weapon's swing, the class's start sound);
// kind 2: a one-shot overlay row on the unit;
// kind 5: the unit was struck (row = its hit class);
// kind 6: a unit sound event (row = the event, skill = the unit's class id).
// `skill` is the skill id that caused the effect, kSkillHit for a hit and kSkillRecord for an
// effect a server record named directly.
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace d2::realm {

inline constexpr uint32_t kUnitFxSound = 1, kUnitFxOverlay = 2, kUnitFxSwing = 3, kUnitFxClassSound = 4,
    kUnitFxHit = 5, kUnitFxUnitSound = 6;
inline constexpr uint32_t kUnitFxSkillHit = 0xffffffffu, kUnitFxSkillRecord = 0xfffffffeu;

struct UnitFxEvent { uint32_t version, tick, kind, unitType, unitGuid, row, delay, skill; };
static_assert(sizeof(UnitFxEvent) == 32);

struct UnitFxQueue {
    static constexpr size_t kCapacity = 256;
    std::vector<UnitFxEvent> events;

    // A full queue drops its oldest event.
    void push(const UnitFxEvent& e) {
        if (events.size() == kCapacity) events.erase(events.begin());
        events.push_back(e);
    }
    // Hands out up to `cap` events once, oldest first, eight words each.
    int take(int32_t* out, int cap) {
        if (!out || cap <= 0) return 0;
        const int count = std::min(cap, static_cast<int>(events.size()));
        for (int i = 0; i < count; ++i) std::memcpy(out + i * 8, &events[static_cast<size_t>(i)], 32);
        events.erase(events.begin(), events.begin() + count);
        return count;
    }
    size_t size() const { return events.size(); }
    void clear() { events.clear(); }
};

} // namespace d2::realm
