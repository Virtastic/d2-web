// The party and relation state of one D2GS game, as the game server keeps it per pair of players.
// Shared by the 0x5d / 0x5e handlers, the party command table and the 0x5c / 0x8b / 0x8c / 0x8d /
// 0x5a composers.  Every player has its own output queue: a notice for player B waits in B's queue
// until B's connection takes it.
#pragma once
#include <cstdint>

namespace d2gs_party {

constexpr int kMax = 8;                 // players in a game
constexpr uint16_t kNoParty = 0xFFFF;
constexpr uint32_t kHostileWaitMs = 60000;

// Relation flags one player holds toward another (the 0x8c flags word).
enum : uint16_t { kLoot = 1, kHear = 2, kSquelch = 4, kHostile = 8 };

// Sub-kinds of a 0x5a type-7 (party / hostility / corpse-loot) notice, byte 7 of the packet.
enum Notice : uint8_t {
    kAllowMaleLoot = 1, kAllowFemaleLoot = 2, kHostileNotice = 3, kNoLongerHostile = 4,
    kReceivedInvite = 5, kInviteCancelled = 6, kJoinedParty = 7, kNowAlliedWith = 8,
    kLeftParty = 9, kDisallowMaleLoot = 10, kDisallowFemaleLoot = 11,
};

struct Player {
    bool used;
    uint32_t id;
    uint8_t cls;
    uint16_t level;
    uint16_t party;
    bool inTown;
};
struct Pair {            // what player a holds toward player b
    uint16_t flags;
    uint8_t state;       // 0 none, 1 in a party together, 2 invited by b, 4 has invited b
    uint32_t hostileUntil;
};
struct State {
    Player p[kMax];
    Pair pair[kMax][kMax];
    uint32_t self;       // the first client's unit id (what the single-client entry points act for)
    bool hardcore;
    uint32_t nowMs;
    uint16_t nextParty;
    uint8_t out[kMax][512];   // raw packets queued per player slot
    int outLen[kMax];
};
extern State g;

int find(uint32_t id);                          // slot of a player, -1 when absent
void emit(int recipient, const uint8_t* bytes, int n);
void send_state(int recipient, int other);      // 0x8b: recipient's party state toward other
void send_relation(int a, int b);               // 0x8c to everyone: a's flags toward b
void send_party(int p);                         // 0x8d to everyone: p's party id
void send_notice(int recipient, int about, uint8_t notice);   // 0x5a type 7
void send_wait_hostile(int recipient);          // 0x5a type 16
void set_flag(int a, int b, uint16_t flag, bool on);
int command(int issuer, uint8_t cmd, uint32_t target, uint8_t param);   // 0 ok, 3 refused
int party_of_count(uint16_t party);

}  // namespace d2gs_party

extern "C" {
int d2_realm_d2gs_player_left(uint32_t id, uint8_t* out);
int d2_realm_d2gs_party_state(uint32_t id, uint8_t state, uint8_t* out);
int d2_realm_d2gs_relation(uint32_t from, uint32_t to, uint16_t flags, uint8_t* out);
int d2_realm_d2gs_party_id(uint32_t id, uint16_t party, uint8_t* out);
int d2_realm_d2gs_pvp_status(uint32_t id, uint8_t notice, uint8_t* out);
int d2_realm_d2gs_event_wait_hostile(uint8_t* out);

void d2_realm_d2gs_party_reset(uint32_t self, uint8_t cls, uint16_t level, int hardcore);
int d2_realm_d2gs_party_add(uint32_t id, uint8_t cls, uint16_t level);
void d2_realm_d2gs_party_remove(uint32_t id);
void d2_realm_d2gs_party_clock(uint32_t nowMs);
int d2_realm_d2gs_party_command(uint32_t issuerId, uint8_t cmd, uint32_t target, uint8_t param);
void d2_realm_d2gs_party_set_town(uint32_t id, int inTown);
int d2_realm_d2gs_party_take(uint8_t* out, int cap);
int d2_realm_d2gs_party_take_for(uint32_t id, uint8_t* out, int cap);
int d2_realm_d2gs_recv_5d_from(uint32_t issuerId, const uint8_t* packet, int size);
int d2_realm_d2gs_recv_5e_from(uint32_t issuerId, const uint8_t* packet, int size);
int d2_realm_d2gs_recv_5d(const uint8_t* packet, int size);
int d2_realm_d2gs_recv_5e(const uint8_t* packet, int size);
}
