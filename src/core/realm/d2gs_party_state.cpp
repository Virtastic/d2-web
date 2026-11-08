#include "d2gs_party.h"

#include <emscripten.h>

namespace d2gs_party {

State g;

int find(uint32_t id) {
    for (int i = 0; i < kMax; i++) if (g.p[i].used && g.p[i].id == id) return i;
    return -1;
}

void emit(int recipient, const uint8_t* bytes, int n) {
    if (recipient < 0 || recipient >= kMax || !g.p[recipient].used) return;
    if (g.outLen[recipient] + n > (int)sizeof g.out[recipient]) return;
    for (int i = 0; i < n; i++) g.out[recipient][g.outLen[recipient]++] = bytes[i];
}

void send_state(int recipient, int other) {
    if (recipient == other) return;
    uint8_t b[6];
    emit(recipient, b, d2_realm_d2gs_party_state(g.p[other].id, g.pair[recipient][other].state, b));
}

void send_relation(int a, int b) {
    uint8_t pk[11];
    const int n = d2_realm_d2gs_relation(g.p[a].id, g.p[b].id, g.pair[a][b].flags, pk);
    for (int r = 0; r < kMax; r++) emit(r, pk, n);
}

void send_party(int p) {
    uint8_t pk[7];
    const int n = d2_realm_d2gs_party_id(g.p[p].id, g.p[p].party, pk);
    for (int r = 0; r < kMax; r++) emit(r, pk, n);
}

void send_notice(int recipient, int about, uint8_t notice) {
    uint8_t pk[40];
    emit(recipient, pk, d2_realm_d2gs_pvp_status(g.p[about].id, notice, pk));
}

void send_wait_hostile(int recipient) {
    uint8_t pk[40];
    emit(recipient, pk, d2_realm_d2gs_event_wait_hostile(pk));
}

// Set or clear one relation flag; the 0x8c goes out only when the flag actually changed.
void set_flag(int a, int b, uint16_t flag, bool on) {
    if (a == b) return;
    const uint16_t had = g.pair[a][b].flags;
    const uint16_t now = on ? (uint16_t)(had | flag) : (uint16_t)(had & ~flag);
    if (now == had) return;
    g.pair[a][b].flags = now;
    send_relation(a, b);
}

int party_of_count(uint16_t party) {
    int n = 0;
    for (int i = 0; i < kMax; i++) if (g.p[i].used && g.p[i].party == party) n++;
    return n;
}

}  // namespace d2gs_party

using namespace d2gs_party;

extern "C" {

EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_party_reset(uint32_t self, uint8_t cls, uint16_t level, int hardcore) {
    g = State();
    g.self = self;
    g.hardcore = hardcore != 0;
    g.nextParty = 3;
    g.p[0] = Player{ true, self, cls, level, kNoParty, true };
}

// Another player entering the game.  Returns the slot, -1 when the game is full or the id is taken.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_party_add(uint32_t id, uint8_t cls, uint16_t level) {
    if (find(id) >= 0) return -1;
    for (int i = 0; i < kMax; i++) {
        if (g.p[i].used) continue;
        g.p[i] = Player{ true, id, cls, level, kNoParty, true };
        g.outLen[i] = 0;
        for (int j = 0; j < kMax; j++) { g.pair[i][j] = Pair(); g.pair[j][i] = Pair(); }
        return i;
    }
    return -1;
}

// A player leaving the game: they leave their party first (the party's other members are told),
// then every other client gets 0x5c and drops the player's relations.
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_party_remove(uint32_t id) {
    const int i = find(id);
    if (i < 0) return;
    if (g.p[i].party != kNoParty) command(i, 9, id, 0);
    uint8_t b[5];
    const int n = d2_realm_d2gs_player_left(id, b);
    g.p[i].used = false;
    for (int r = 0; r < kMax; r++) emit(r, b, n);
}

// A party command from any player in the game (the connected client's own packets arrive through
// the 0x5d / 0x5e handlers).  Returns the command's result, 0 or 3.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_party_command(uint32_t issuerId, uint8_t cmd, uint32_t target, uint8_t param) {
    const int i = find(issuerId);
    return i < 0 ? 0 : command(i, cmd, target, param);
}

EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_party_clock(uint32_t nowMs) { g.nowMs = nowMs; }

EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_party_set_town(uint32_t id, int inTown) {
    const int i = find(id);
    if (i >= 0) g.p[i].inTown = inTown != 0;
}

// Move the raw packets queued for one player out (the session compresses and frames them).
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_party_take_for(uint32_t id, uint8_t* out, int cap) {
    const int s = find(id);
    if (s < 0) return 0;
    if (g.outLen[s] > cap) return -1;
    const int n = g.outLen[s];
    for (int i = 0; i < n; i++) out[i] = g.out[s][i];
    g.outLen[s] = 0;
    return n;
}

// The first client's queue (the single-client entry point).
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_party_take(uint8_t* out, int cap) {
    return d2_realm_d2gs_party_take_for(g.self, out, cap);
}

}  // extern "C"
