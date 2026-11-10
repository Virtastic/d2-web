#pragma once

#include <cstdint>
#include <cstring>

// The quest state a joined game's client holds, built only from the server's packets (1.13c D2Client):
//   0x28  the player's own quest record (96 bytes at +7); written by the caller, which applies the
//         "type 6, or the named unit is known" rule of the NPC packet handler
//   0x29  the game's quest record (96 bytes at +1)
//   0x52  the quest log's state table (41 bytes at +1, one per log entry)
//   0x5D  one quest's state change
// Nothing here draws or opens a panel: the handlers' screen events are left to the shell.
struct D2RealmClientQuests {
    uint8_t player[96];
    uint8_t game[96];
    uint8_t states[41];
    uint8_t have;            // bit 0: a 0x28 arrived, bit 1: a 0x29, bit 2: a 0x52
    uint8_t button;          // 0 until a state change arrives with the log's button idle, then 1
    uint8_t logOpen;         // the shell's quest log is open
    uint8_t selected[10];    // per log tab: the entry the last state change points at
    int32_t questOneCount;   // quest 1's counter from a 0x5D with flag 0x20 (0..5, or 0x29A when out of range)
    int32_t quest32Count;    // quest 0x20's counter from a 0x5D with flag 0x20
};
constexpr int kRealmQuestViewBytes = 4 + 96 + 96 + 41 + 3 + 8 + 12;
static_assert(kRealmQuestViewBytes == 260);

// The log's entry table: the quest id each of the 41 state slots belongs to (0x25 = no quest), its row in
// the tab and the tab it sits on.
struct D2RealmQuestLogEntry { uint8_t quest, row, tab; };
inline constexpr D2RealmQuestLogEntry kRealmQuestLogEntries[41] = {
    {0x25, 0, 0}, {0x01, 0, 0}, {0x02, 1, 0}, {0x03, 4, 0}, {0x04, 2, 0}, {0x05, 3, 0}, {0x06, 5, 0}, {0x25, 0, 0},
    {0x25, 0, 1}, {0x08, 0, 1}, {0x09, 1, 1}, {0x0A, 2, 1}, {0x0B, 3, 1}, {0x0C, 4, 1}, {0x0D, 5, 1}, {0x25, 0, 1},
    {0x25, 0, 2}, {0x0F, 3, 2}, {0x10, 2, 2}, {0x11, 1, 2}, {0x12, 0, 2}, {0x13, 4, 2}, {0x14, 5, 2}, {0x25, 0, 2},
    {0x25, 0, 3}, {0x16, 0, 3}, {0x17, 2, 3}, {0x18, 1, 3}, {0x25, 0, 3}, {0x25, 0, 9}, {0x25, 0, 9}, {0x25, 0, 9},
    {0x25, 0, 9}, {0x25, 0, 9}, {0x1E, 0, 4}, {0x1F, 0, 4}, {0x20, 1, 4}, {0x21, 2, 4}, {0x22, 3, 4}, {0x23, 4, 4},
    {0x24, 5, 4},
};

// The state slot of a quest id: the first entry carrying it, or -1.
inline int d2_realm_quest_log_slot(int quest) {
    for (int i = 0; i < 41; ++i) if (kRealmQuestLogEntries[i].quest == quest) return i;
    return -1;
}

inline void d2_realm_client_quests_reset(D2RealmClientQuests& q) { q = {}; }

inline void d2_realm_client_quests_set_player(D2RealmClientQuests& q, const uint8_t* bytes) {
    std::memcpy(q.player, bytes, 96);
    q.have |= 1;
}

// 0x29, 97 bytes.
inline int d2_realm_client_quests_on_game_record(D2RealmClientQuests& q, const uint8_t* packet, int size) {
    if (!packet || size != 97 || packet[0] != 0x29) return 0;
    std::memcpy(q.game, packet + 1, 96);
    q.have |= 2;
    return 1;
}

// 0x52, 42 bytes.
inline int d2_realm_client_quests_on_log_states(D2RealmClientQuests& q, const uint8_t* packet, int size) {
    if (!packet || size != 42 || packet[0] != 0x52) return 0;
    std::memcpy(q.states, packet + 1, 41);
    q.have |= 4;
    return 1;
}

// 0x5D, 6 bytes: [quest][flags][state][int16 value].  Flags 1 and 2 and the exact value 0x10 only raise
// screen events.  Flag 0x20 carries a counter: quest 0x20's is stored and the change goes on; any other
// quest's belongs to quest 1 and is dropped once the player's record has quest 1's bit 0 or bit 1.
// A change that goes on lights the idle log button and points the tab at the quest; with the button
// already lit it writes the state slot, and only while the log is open.
inline int d2_realm_client_quests_on_state(D2RealmClientQuests& q, const uint8_t* packet, int size) {
    if (!packet || size != 6 || packet[0] != 0x5D) return 0;
    const uint8_t quest = packet[1], flags = packet[2], state = packet[3];
    const int32_t value = static_cast<int16_t>(static_cast<uint16_t>(packet[4] | (packet[5] << 8)));
    int32_t count = q.questOneCount;
    if ((flags & 1) || (flags & 2) || flags == 0x10) return 1;
    if (flags & 0x20) {
        if (quest == 0x20) q.quest32Count = value;
        else {
            if (quest == 0x21) return 1;
            if ((q.player[2] & 1) || (q.player[2] & 2)) return 1;
            q.questOneCount = value;
            if (value >= 0 && value < 6) {
                if (value == 5 && quest == 0x17) return 1;
                if (value == 5) count = 5;
            } else q.questOneCount = 0x29A;
            if (value != 5) {
                q.states[d2_realm_quest_log_slot(1)] = state;
                return 1;
            }
        }
    }
    q.questOneCount = count;
    if (q.button == 0) {
        const int slot = d2_realm_quest_log_slot(quest);
        if (slot != -1) q.selected[kRealmQuestLogEntries[slot].tab] = kRealmQuestLogEntries[slot].row;
        q.button = 1;
    } else if (q.logOpen) {
        const int slot = d2_realm_quest_log_slot(quest);
        if (slot != -1) q.states[slot] = state;
    }
    return 1;
}

// The view the browser reads: [have][button][logOpen][0] player[96] game[96] states[41] pad[3]
// questOneCount quest32Count (int32 LE) selected[10] pad[2].
inline int d2_realm_client_quests_view(const D2RealmClientQuests& q, uint8_t* out, int cap) {
    if (!out || cap < kRealmQuestViewBytes) return 0;
    std::memset(out, 0, kRealmQuestViewBytes);
    out[0] = q.have; out[1] = q.button; out[2] = q.logOpen;
    std::memcpy(out + 4, q.player, 96);
    std::memcpy(out + 100, q.game, 96);
    std::memcpy(out + 196, q.states, 41);
    for (int i = 0; i < 4; ++i) {
        out[240 + i] = static_cast<uint8_t>(static_cast<uint32_t>(q.questOneCount) >> (8 * i));
        out[244 + i] = static_cast<uint8_t>(static_cast<uint32_t>(q.quest32Count) >> (8 * i));
    }
    std::memcpy(out + 248, q.selected, 10);
    return kRealmQuestViewBytes;
}
