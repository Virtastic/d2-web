#pragma once

#include <cstdint>
#include <vector>

// The 1.13c client's side of a player's death, corpse and return to town in a joined game. The
// rules are the client's own; the callers in d2gs_client_state.cpp apply them to the client units.
namespace d2::realm {

constexpr uint32_t kPlayerModeDeath = 0;
constexpr uint32_t kPlayerModeNeutral = 1;
constexpr uint32_t kPlayerModeTownNeutral = 5;
constexpr uint32_t kPlayerModeDead = 17;
constexpr uint32_t kUnitFlagDead = 0x10000;

// D2Client+0xA5440: a unit is dead when its dead flag is set or, for a player, when its mode is
// DEATH or DEAD.
inline bool playerIsDead(uint32_t flags, uint32_t mode) {
    return (flags & kUnitFlagDead) != 0 || mode == kPlayerModeDeath || mode == kPlayerModeDead;
}

// D2Client+0x18B20 (flag 0x10000 on, flag 4 off) and +0xA6210 with the caller's tail in
// +0x19830 selector 7 (flag 0x10000 off, flag 0x40 off, flag 2 on).
inline uint32_t playerFlagsAfterDeath(uint32_t flags) { return (flags | kUnitFlagDead) & ~4u; }
inline uint32_t playerFlagsAfterRevive(uint32_t flags) { return ((flags & ~kUnitFlagDead) & ~0x40u) | 2u; }

enum class PlayerLifeMove : uint8_t {
    None,    // not a death/revive selector
    Stay,    // within one cell of the server's position: only the path target is dropped
    Place,   // put at the server's position at once (another player who was dead)
    Walk,    // action 1 to the server's position (the hero)
    Run,     // action 0x17 to the server's position (another player who was alive)
};

struct PlayerLifeAction {
    int mode = -1;               // the mode to set, -1 for none
    bool storeHitClass = false;  // write the record's hit class to the unit's last-hit-class slot
    bool markDead = false;
    bool clearDead = false;
    bool closePanels = false;    // the hero's own death closes its open panels
    PlayerLifeMove move = PlayerLifeMove::None;
};

// D2Client+0x19830, the player column of the 0x0D action selector (also reached by the other
// unit-action packets), selectors 7, 8 and 9:
//   8  the player dies: last-hit class stored, mode DEATH, dead flag set;
//   9  the player is already down: mode DEAD, dead flag set (a corpse entering view);
//   7  the player stands again: dead flag cleared, mode TOWNNEUTRAL in town and NEUTRAL outside,
//      then D2Client+0x2A460 with the server's position.
// dx and dy are the server's position minus the unit's. Returns false for any other selector.
inline bool playerLifeAction(uint8_t selector, bool hero, bool inTown, bool wasDead,
        int32_t dx, int32_t dy, PlayerLifeAction* out) {
    if (!out) return false;
    *out = PlayerLifeAction{};
    if (selector == 8) {
        out->mode = static_cast<int>(kPlayerModeDeath);
        out->storeHitClass = true;
        out->markDead = true;
        out->closePanels = hero;
        return true;
    }
    if (selector == 9) {
        out->mode = static_cast<int>(kPlayerModeDead);
        out->markDead = true;
        return true;
    }
    if (selector != 7) return false;
    out->mode = static_cast<int>(inTown ? kPlayerModeTownNeutral : kPlayerModeNeutral);
    out->clearDead = true;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    if (dx <= 1 && dy <= 1) out->move = PlayerLifeMove::Stay;
    else if (hero) out->move = PlayerLifeMove::Walk;
    else out->move = wasDead ? PlayerLifeMove::Place : PlayerLifeMove::Run;
    return true;
}

// The hero is never sent selector 7 for itself. It stands up when a life value arrives while it is
// DEAD: D2Client+0xAEBE0 (0x95), +0xAECA0 (0x18) and the hit-point case of +0xAEB10 (0x1D-0x1F,
// stat 6) set mode TOWNNEUTRAL and clear the dead flag when the new life is nonzero.
inline bool heroRevivesOnLife(uint32_t mode, uint32_t life) {
    return mode == kPlayerModeDead && life != 0;
}

// D2Client+0xA7E20, the 0x15 reassignment: a dead unit is left where it is.
inline bool reassignRefusedForDead(uint32_t flags, uint32_t mode) { return playerIsDead(flags, mode); }

// 0x8E, 10 bytes: assign flag at +1, the owner's GUID at +2, the corpse's GUID at +6.
// 0x74, 10 bytes: flag at +1, the owner's GUID at +2, the corpse's GUID at +6.
struct CorpsePacket {
    uint8_t flag = 0;
    uint32_t owner = 0;
    uint32_t corpse = 0;
};

inline bool decodeCorpsePacket(const uint8_t* p, int length, uint8_t opcode, CorpsePacket* out) {
    if (!p || !out || length != 10 || p[0] != opcode) return false;
    const auto u32 = [](const uint8_t* b) {
        return uint32_t(b[0]) | (uint32_t(b[1]) << 8) | (uint32_t(b[2]) << 16) | (uint32_t(b[3]) << 24);
    };
    out->flag = p[1];
    out->owner = u32(p + 2);
    out->corpse = u32(p + 6);
    return true;
}

// The corpse ids the roster keeps per player (D2Client+0x9D760 adds, +0x9D700 removes,
// +0x9D630 finds the owner of a corpse). A player can leave several corpses.
struct CorpseRoster {
    struct Entry { uint32_t owner; uint32_t corpse; };
    std::vector<Entry> entries;

    void clear() { entries.clear(); }

    // Returns true when the list changed.
    bool apply(const CorpsePacket& packet) {
        for (size_t i = 0; i < entries.size(); ++i) {
            if (entries[i].owner != packet.owner || entries[i].corpse != packet.corpse) continue;
            if (packet.flag) return false;
            entries.erase(entries.begin() + static_cast<std::ptrdiff_t>(i));
            return true;
        }
        if (!packet.flag) return false;
        entries.push_back({ packet.owner, packet.corpse });
        return true;
    }

    // The owner of a corpse unit; 0 when the unit is not a known corpse.
    uint32_t ownerOf(uint32_t corpse) const {
        for (const Entry& entry : entries) if (entry.corpse == corpse) return entry.owner;
        return 0;
    }
};

// D2Client+0x194E0, the 0x74 receiver. It acts only when the flag is set, which the server does
// for a corpse made this frame: the owner falls (mode DEATH) unless it is already dead, the
// hero's own corpse is put on the hero's cell facing the hero's way, and the owner loses every
// item worn on the body (inventory node page 3); those items now belong to the corpse.
constexpr int kInventoryNodePageBody = 3;

struct CorpseDropAction {
    bool act = false;
    bool ownerFalls = false;
    bool placeCorpseOnHero = false;
    bool stripWornItems = false;
};

inline CorpseDropAction corpseDropAction(const CorpsePacket& packet, bool ownerFound, bool ownerDead,
        bool ownerIsHero, bool corpseFound) {
    CorpseDropAction action{};
    if (!packet.flag || !ownerFound) return action;
    action.act = true;
    action.ownerFalls = !ownerDead;
    action.placeCorpseOnHero = ownerIsHero && corpseFound;
    action.stripWornItems = true;
    return action;
}

}  // namespace d2::realm
