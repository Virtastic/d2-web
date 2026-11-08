#include "d2gs_party.h"

// The party command table (1.13c D2Game 0xF0060-0xF0FF0 and the PARTYSCREEN handlers D2MOO names):
// 1 loot, 2 hear, 3 squelch, 4 hostile, 6 invite, 7 cancel the invite, 8 accept, 9 leave.  0 and 5
// have no handler.  `issuer` is the player who sent the packet, `target` the unit id it names.

namespace d2gs_party {
namespace {

bool female(const Player& p) { return p.cls == 0 || p.cls == 1 || p.cls == 6; }   // amazon, sorceress, assassin

// Open or drop the mutual hostility between two players.
void set_hostile(int a, int b, bool on) {
    set_flag(a, b, kHostile, on);
    set_flag(b, a, kHostile, on);
}

// Put a player into a party; they stop being hostile to the party's members.
void join_party(int p, uint16_t party) {
    g.p[p].party = party;
    send_party(p);
    for (int m = 0; m < kMax; m++) {
        if (m == p || !g.p[m].used || g.p[m].party != party) continue;
        set_hostile(p, m, false);
    }
}

// A party member's own 0x8b/0x8d refresh after membership changed.
void after_leave(int p) {
    for (int l = 0; l < kMax; l++) {
        if (!g.p[l].used || l == p) continue;
        g.pair[l][p].state = 0;
        g.pair[p][l].state = g.p[l].party != kNoParty;
        send_state(l, p);
        send_state(p, l);
    }
}

void leave_party(int p) {
    const uint16_t party = g.p[p].party;
    if (party == kNoParty) return;
    g.p[p].party = kNoParty;
    after_leave(p);
    send_party(p);
    if (party_of_count(party) > 1) return;
    for (int m = 0; m < kMax; m++) {          // a party of one is dissolved
        if (!g.p[m].used || g.p[m].party != party) continue;
        g.p[m].party = kNoParty;
        after_leave(m);
        send_party(m);
    }
}

void player_left_party(int p) {
    for (int m = 0; m < kMax; m++)
        if (g.p[m].used && m != p && g.p[m].party != kNoParty && g.p[m].party == g.p[p].party)
            send_notice(m, p, kLeftParty);
    leave_party(p);
}

uint16_t new_party() {
    uint16_t id = g.nextParty < 3 ? 3 : g.nextParty;
    for (bool taken = true; taken;) {
        taken = false;
        for (int i = 0; i < kMax; i++) if (g.p[i].used && g.p[i].party == id) taken = true;
        if (taken) id = (uint16_t)(id + 1 < 3 ? 3 : id + 1);
    }
    g.nextParty = (uint16_t)(id + 1);
    return id;
}

// What one player records toward a newly allied pair.
void sync_pair(int l, int x) {
    if (l == x) return;
    if (g.p[l].party != kNoParty) {
        g.pair[x][l].state = 1;
        g.pair[l][x].state = 1;
    } else if (g.pair[x][l].state == 4) {
        g.pair[l][x].state = 2;
    } else {
        g.pair[x][l].state = 0;
        g.pair[l][x].state = 1;
    }
}

// Accepting players' pending invitations (state 2) resolve to in-party or none.
void settle_invites(int p) {
    for (int x = 0; x < kMax; x++) {
        if (!g.p[x].used || x == p || g.pair[p][x].state != 2) continue;
        g.pair[p][x].state = g.p[x].party != kNoParty;
        send_state(p, x);
    }
}

void toggle_loot(int a, int b, uint8_t on) {
    if (!g.hardcore) return;
    set_flag(a, b, kLoot, on != 0);
    uint8_t kind;
    if (female(g.p[a])) kind = on ? kAllowFemaleLoot : kDisallowFemaleLoot;
    else kind = on ? kAllowMaleLoot : kDisallowMaleLoot;
    send_notice(b, a, kind);
}

int toggle_hostile(int a, int b, uint8_t on) {
    if (!g.p[a].inTown || g.p[a].level < 9 || g.p[b].level < 9) return 3;
    const bool now = (g.pair[a][b].flags & kHostile) != 0;
    if (now == (on != 0)) { send_relation(a, b); send_relation(b, a); return 0; }
    if (!on) {
        set_hostile(a, b, false);
        send_notice(b, a, kNoLongerHostile);
        return 0;
    }
    if (g.pair[a][b].hostileUntil > g.nowMs) {
        send_wait_hostile(a);
        send_relation(a, b);
        send_relation(b, a);
        return 0;
    }
    g.pair[a][b].hostileUntil = g.nowMs + kHostileWaitMs;
    if (g.p[b].party == kNoParty) {
        set_hostile(a, b, true);
        send_notice(b, a, kHostileNotice);
    } else if (g.p[a].level >= 9) {
        for (int m = 0; m < kMax; m++) {      // the whole party of the target turns hostile
            if (!g.p[m].used || m == a || g.p[m].party != g.p[b].party || g.p[m].level < 9) continue;
            set_hostile(a, m, true);
            send_notice(m, a, kHostileNotice);
        }
    }
    if (g.p[a].party != kNoParty && g.p[a].party == g.p[b].party) player_left_party(a);
    return 0;
}

void invite(int a, int b) {
    if (g.pair[a][b].state == 0) {
        g.pair[a][b].state = 4;
        g.pair[b][a].state = 2;
        send_notice(b, a, kReceivedInvite);
    }
    send_state(a, b);
    send_state(b, a);
}

void cancel_invite(int a, int b) {
    if (g.pair[a][b].state == 4) {
        g.pair[a][b].state = g.p[b].party != kNoParty;
        g.pair[b][a].state = g.p[a].party != kNoParty;
        send_notice(b, a, kInviteCancelled);
    }
    send_state(a, b);
    send_state(b, a);
}

void accept(int a, int b) {            // a accepts b's invitation
    if (g.pair[a][b].state != 2) {
        send_state(b, a);
        send_state(a, b);
        return;
    }
    uint16_t party = g.p[b].party;
    if (party == kNoParty) {
        party = new_party();
        join_party(b, party);
        join_party(a, party);
        settle_invites(b);
    } else {
        join_party(a, party);
    }
    settle_invites(a);
    send_notice(a, b, kNowAlliedWith);
    for (int l = 0; l < kMax; l++) {       // everyone refreshes what they hold toward both
        if (!g.p[l].used) continue;
        sync_pair(l, b);
        sync_pair(l, a);
        if (g.p[l].party == party && l != a) send_notice(l, a, kJoinedParty);
        send_state(l, b);
        send_state(l, a);
        send_party(l);
    }
}

}  // namespace

int command(int issuer, uint8_t cmd, uint32_t target, uint8_t param) {
    const int t = find(target);
    if (t < 0) return 0;                    // a target not in the game is ignored
    switch (cmd) {
    case 1: toggle_loot(issuer, t, param); return 0;
    case 2: set_flag(issuer, t, kHear, param != 0); return 0;
    case 3: set_flag(issuer, t, kSquelch, param != 0); return 0;
    case 4: return toggle_hostile(issuer, t, param);
    case 6: invite(issuer, t); return 0;
    case 7: cancel_invite(issuer, t); return 0;
    case 8: accept(issuer, t); return 0;
    case 9: player_left_party(issuer); return 0;
    default: return 0;                      // 0 and 5 have no handler
    }
}

}  // namespace d2gs_party
