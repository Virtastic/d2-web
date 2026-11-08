#pragma once
// The snapshot the browser reads each frame, in the single-player session's exact 68-byte layout
// (monspawn_check.cpp D2SessionSnapshot), filled from the Battle.net game-server client state.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace d2::realm {

#pragma pack(push, 1)
struct ClientSnapshot {
    uint32_t version, size, seed, frame;
    uint16_t level;
    uint8_t difficulty, act;
    uint32_t playerClass, playerId;
    int32_t x, y, targetX, targetY, hp, maxHp;
    uint32_t mode, flags, monsters, hash;
};
#pragma pack(pop)
static_assert(sizeof(ClientSnapshot) == 68);
static_assert(offsetof(ClientSnapshot, x) == 28 && offsetof(ClientSnapshot, y) == 32);

struct ClientSnapshotInput {
    bool playerActive;
    uint32_t seed, frame, level, difficulty, act, classId, guid;
    int32_t x, y, hp, maxHp;
    uint32_t mode, flags, monsters;
};

inline ClientSnapshot build_client_snapshot(const ClientSnapshotInput& in) {
    ClientSnapshot s;
    std::memset(&s, 0, sizeof s);
    s.version = 1;
    s.size = sizeof s;
    if (!in.playerActive) return s;   // empty exactly where the single-player snapshot is empty
    s.seed = in.seed; s.frame = in.frame;
    s.level = static_cast<uint16_t>(in.level);
    s.difficulty = static_cast<uint8_t>(in.difficulty);
    s.act = static_cast<uint8_t>(in.act);
    s.playerClass = in.classId; s.playerId = in.guid;
    s.x = s.targetX = in.x; s.y = s.targetY = in.y;
    s.hp = in.hp; s.maxHp = in.maxHp;
    s.mode = in.mode; s.flags = in.flags; s.monsters = in.monsters;
    uint32_t h = 2166136261u;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&s);
    for (size_t i = 0; i < offsetof(ClientSnapshot, hash); ++i) { h ^= bytes[i]; h *= 16777619u; }
    s.hash = h;
    return s;
}

}  // namespace d2::realm
