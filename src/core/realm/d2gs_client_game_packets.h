// Client state three groups of server packets build in a joined game, as the 1.13c client's own
// handlers store it. Pure state: the caller resolves units and hands in what the handler looked up.
//
//   0x01 (8 bytes)   game flags          D2Client+0xAFA90
//   0x8A (6 bytes)   NPC wants to talk   D2Client+0xADB30 -> +0x47970
//   0x28 (103 bytes) NPC interact reply  D2Client+0xAFA70 -> +0x4B8A0 (the part that clears the marks)
//   0x81 (20 bytes)  pet assign          D2Client+0xAEBA0 -> +0x21D60
//   0x7F (10 bytes)  pet life, kind 0    D2Client+0xAD820 -> +0x21950
//   0x9E..0xA2       hireling stat       D2Client+0xAEA40
#pragma once

#include <cstdint>
#include <vector>

namespace d2::realm {

inline uint32_t gamePktU16(const uint8_t* p) { return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8); }
inline uint32_t gamePktU32(const uint8_t* p) { return gamePktU16(p) | (gamePktU16(p + 2) << 16); }

// ---- 0x01: game flags -------------------------------------------------------------------------
// The handler stores the difficulty byte, the 32-bit game flags, the expansion byte and the ladder
// byte in four globals, then starts the game UI.
struct GameInfo {
    uint32_t received = 0;
    uint32_t difficulty = 0;   // packet[1]
    uint32_t gameFlags = 0;    // packet[2..5]
    uint32_t expansion = 0;    // packet[6]
    uint32_t ladder = 0;       // packet[7]
};

inline bool gameInfoApply(GameInfo& info, const uint8_t* packet, int size) {
    if (!packet || size != 8 || packet[0] != 0x01) return false;
    info.received = 1;
    info.difficulty = packet[1];
    info.gameFlags = gamePktU32(packet + 2);
    info.expansion = packet[6];
    info.ladder = packet[7];
    return true;
}

// The same facts in the character save-flag layout a local game's client reads (hardcore 0x04,
// expansion 0x20, ladder 0x40). Hardcore is bit 0x800 of the game flags.
inline uint32_t gameInfoSaveFlags(const GameInfo& info) {
    uint32_t flags = 0;
    if (info.gameFlags & 0x800u) flags |= 0x04u;
    if (info.expansion) flags |= 0x20u;
    if (info.ladder) flags |= 0x40u;
    return flags;
}

// ---- 0x8A / 0x28: the mark over an NPC that has something to say --------------------------------
struct NpcAlert { uint32_t guid, classId; };
struct NpcAlertState {
    std::vector<NpcAlert> alerts;   // units carrying the mark
    uint32_t activeNpc = 0;         // the NPC whose menu is open
    uint32_t hasActiveNpc = 0;
};

enum class NpcAlertResult { None, Marked, SoundOnly };

constexpr uint32_t kNpcAlertSoundOnlyClass = 534;

// 0x8A. `found` and `classId` are the unit lookup by packet[1] (type) and packet[2..5] (guid). A unit
// that is not there gets nothing; class 534 only speaks; the NPC whose menu is open is not marked.
inline NpcAlertResult npcAlertApply(NpcAlertState& state, const uint8_t* packet, int size, bool found, uint32_t classId) {
    if (!packet || size != 6 || packet[0] != 0x8a || !found) return NpcAlertResult::None;
    const uint32_t type = packet[1], guid = gamePktU32(packet + 2);
    if (classId == kNpcAlertSoundOnlyClass) return NpcAlertResult::SoundOnly;
    if (state.hasActiveNpc && type == 1 && guid == state.activeNpc) return NpcAlertResult::None;
    for (const NpcAlert& alert : state.alerts) if (alert.guid == guid) return NpcAlertResult::Marked;
    state.alerts.push_back({ guid, classId });
    return NpcAlertResult::Marked;
}

// 0x28. Kind 6 only carries quest bytes. For any other kind, when the unit it names is there, the mark
// comes off every monster whose MonStats row is an NPC (`isNpc`). Returns how many marks came off.
inline int npcInteractReplyApply(NpcAlertState& state, const uint8_t* packet, int size, bool found, bool (*isNpc)(uint32_t classId)) {
    if (!packet || size != 103 || packet[0] != 0x28 || packet[1] == 6 || !found || !isNpc) return 0;
    int removed = 0;
    for (size_t i = 0; i < state.alerts.size();) {
        if (!isNpc(state.alerts[i].classId)) { ++i; continue; }
        state.alerts.erase(state.alerts.begin() + static_cast<long>(i));
        ++removed;
    }
    return removed;
}

// A unit leaving the client takes its mark with it.
inline void npcAlertUnitRemoved(NpcAlertState& state, uint32_t guid) {
    for (size_t i = 0; i < state.alerts.size(); ++i)
        if (state.alerts[i].guid == guid) { state.alerts.erase(state.alerts.begin() + static_cast<long>(i)); return; }
}

// ---- 0x81 / 0x7F / 0x9E..0xA2: the pet list and the hireling's stats ----------------------------
// One entry per pet, newest first, keyed by the pet unit's guid. Kind 7 is the hireling: there is one.
struct Pet {
    uint32_t classId;   // packet[2..3]
    uint32_t kind;      // packet[1]
    uint32_t guid;      // packet[8..11]
    uint32_t owner;     // packet[4..7]
    uint32_t area;      // 0x7F packet[8..9]
    uint32_t lifePct;   // 100 on creation, then 0x7F packet[2..3]
    uint32_t seed;      // packet[12..15]
    uint32_t nameId;    // packet[16..19]
};
struct PetStat { uint32_t guid; int32_t stat; int32_t value; };
struct PetState {
    std::vector<Pet> pets;
    std::vector<PetStat> stats;   // the hireling unit's stats set by 0x9E..0xA2
};

constexpr uint32_t kPetKindHireling = 7;

inline Pet* petFind(PetState& state, uint32_t guid) {
    for (Pet& pet : state.pets) if (pet.guid == guid) return &pet;
    return nullptr;
}

inline bool petAssignApply(PetState& state, const uint8_t* packet, int size) {
    if (!packet || size != 20 || packet[0] != 0x81) return false;
    const uint32_t kind = packet[1], guid = gamePktU32(packet + 8);
    if (kind == kPetKindHireling) {   // a new hireling replaces the one already listed
        for (size_t i = 0; i < state.pets.size(); ++i)
            if (state.pets[i].kind == kPetKindHireling && state.pets[i].guid != guid) { state.pets.erase(state.pets.begin() + static_cast<long>(i)); break; }
    }
    Pet* pet = petFind(state, guid);
    if (!pet) {
        state.pets.insert(state.pets.begin(), Pet{ 0, kind, guid, 0, 0, 100, 0, 0 });
        pet = &state.pets.front();
    }
    pet->classId = gamePktU16(packet + 2);
    pet->kind = kind;
    pet->owner = gamePktU32(packet + 4);
    pet->seed = gamePktU32(packet + 12);
    pet->nameId = gamePktU32(packet + 16);
    return true;
}

inline void petStatSet(PetState& state, uint32_t guid, int32_t stat, int32_t value, bool add) {
    for (PetStat& s : state.stats)
        if (s.guid == guid && s.stat == stat) { s.value = add ? s.value + value : value; return; }
    state.stats.push_back({ guid, stat, value });
}

inline bool petStatGet(const PetState& state, uint32_t guid, int32_t stat, int32_t* value) {
    for (const PetStat& s : state.stats)
        if (s.guid == guid && s.stat == stat) { *value = s.value; return true; }
    return false;
}

// 0x7F with kind 0: the pet's life percentage and area. For the hireling whose unit is there, life
// (stat 6) becomes that share of its maximum (stat 7).
inline bool petLifeApply(PetState& state, const uint8_t* packet, int size, bool unitFound) {
    if (!packet || size != 10 || packet[0] != 0x7f || packet[1] != 0) return false;
    Pet* pet = petFind(state, gamePktU32(packet + 4));
    if (!pet) return false;
    pet->area = gamePktU16(packet + 8);
    pet->lifePct = gamePktU16(packet + 2);
    int32_t max = 0;
    if (pet->kind == kPetKindHireling && unitFound && petStatGet(state, pet->guid, 7, &max))
        petStatSet(state, pet->guid, 6, static_cast<int32_t>((static_cast<int64_t>(max) * static_cast<int32_t>(pet->lifePct)) / 100), false);
    return true;
}

// 0x9E (byte), 0x9F (word), 0xA0 (dword) set a stat of the monster unit packet[2..5]; 0xA1 (byte) and
// 0xA2 (word) add to it. Nothing happens when the unit is not there.
inline bool petStatApply(PetState& state, const uint8_t* packet, int size, bool unitFound) {
    if (!packet || size < 7) return false;
    uint32_t value = 0;
    bool add = false;
    switch (packet[0]) {
        case 0x9e: if (size != 7) return false; value = packet[6]; break;
        case 0x9f: if (size != 8) return false; value = gamePktU16(packet + 6); break;
        case 0xa0: if (size != 10) return false; value = gamePktU32(packet + 6); break;
        case 0xa1: if (size != 7) return false; value = packet[6]; add = true; break;
        case 0xa2: if (size != 8) return false; value = gamePktU16(packet + 6); add = true; break;
        default: return false;
    }
    if (!unitFound) return false;
    petStatSet(state, gamePktU32(packet + 2), packet[1], static_cast<int32_t>(value), add);
    return true;
}

// The unit is gone: so is its stat list. The pet entry stays (the list is not keyed on the unit).
inline void petUnitRemoved(PetState& state, uint32_t guid) {
    for (size_t i = 0; i < state.stats.size();) {
        if (state.stats[i].guid == guid) state.stats.erase(state.stats.begin() + static_cast<long>(i)); else ++i;
    }
}

}  // namespace d2::realm
