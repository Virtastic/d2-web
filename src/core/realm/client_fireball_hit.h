#pragma once

#include <cstdint>

struct D2UnitStrc;

namespace d2::realm {

// D2Client 1.13c FUN_6FB66D40's output, kept separate from the server missile descriptor.
// The realm adapter resolves owner identity and builds the client-owned missile CUnit.
struct FireballHitChild {
    int childClass;
    int x;
    int y;
    int sourceClass; // value passed by the hit callback's Common #10240 call
    int sourceLevel; // value passed by the hit callback's Common #11029 call
    int startFrame;  // source-seed roll modulo child Missiles.txt dwRandStart
};

struct FireballHitCounts {
    uint32_t inCircle = 0;
    uint32_t gateRejected = 0;
    uint32_t outsideNearRooms = 0;
    uint32_t emitted = 0;
    uint32_t factoryRejected = 0;
    uint32_t seedRolls = 0;
};

using FireballHitChildFactory = bool (*)(D2UnitStrc* source,
        const FireballHitChild& child, void* context);

// Engine adapter declared in monspawn_check.cpp. The caller supplies the selected client player
// CUnit (the native room-membership origin) and the canonical client missile factory.
FireballHitCounts createFireballHitChildren(D2UnitStrc* sourceFireball,
        D2UnitStrc* currentClientPlayer, FireballHitChildFactory factory, void* context);

// FUN_6FAB1400 accepts the current room directly before consulting Common #10383's near-room
// list. Keep that branch explicit so an empty/absent near list cannot reject current-room points.
template<class Room>
bool isCurrentOrNearRoom(Room* candidate, Room* current, Room* const* nearRooms, int nearCount) {
    if (candidate && candidate == current) return true;
    if (!candidate || !nearRooms || nearCount <= 0) return false;
    for (int i = 0; i < nearCount; ++i) {
        if (nearRooms[i] == candidate) return true;
    }
    return false;
}

// Replays the client callback's deterministic candidate loop. `seed` is the source Fireball
// client CUnit's +0x20/+0x24 pair. `inClientNearRooms` represents FUN_6FAB1400 against the
// current client Room1 and Common #10383's ordered near-room list; it is checked after both
// applicable random rolls, as in the native routine. `emit` creates/owns the child CUnit.
template<class Seed, class NearPredicate, class Emit>
FireballHitCounts enumerateFireballHitChildren(
        Seed& seed, int centerX, int centerY, int radius, uint32_t gate,
        uint32_t childRandStart, int sourceClass, int sourceLevel,
        NearPredicate&& inClientNearRooms, Emit&& emit) {
    FireballHitCounts counts{};
    if (radius < 0) return counts;

    auto roll = [&]() -> uint32_t {
        const uint64_t next = uint64_t(seed.nLowSeed) * 0x6ac690c5ULL + seed.nHighSeed;
        seed.nLowSeed = static_cast<uint32_t>(next);
        seed.nHighSeed = static_cast<uint32_t>(next >> 32);
        ++counts.seedRolls;
        // Native callers take the low dword in EAX before unsigned division.
        return seed.nLowSeed;
    };

    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (int64_t(dx) * dx + int64_t(dy) * dy > int64_t(radius) * radius) continue;
            ++counts.inCircle;

            if (gate > 0 && roll() % gate != 0) {
                ++counts.gateRejected;
                continue;
            }

            const int startFrame = childRandStart > 0
                ? static_cast<int>(roll() % childRandStart)
                : 0;
            const int x = centerX + dx;
            const int y = centerY + dy;
            if (!inClientNearRooms(x, y)) {
                ++counts.outsideNearRooms;
                continue;
            }

            if (emit(FireballHitChild{265, x, y, sourceClass, sourceLevel, startFrame})) {
                ++counts.emitted;
            } else {
                ++counts.factoryRejected;
            }
        }
    }
    return counts;
}

} // namespace d2::realm
