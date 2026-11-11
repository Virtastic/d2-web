// Host test: g++ -std=c++17 -Isrc/core/d2moo/{compat,include,include/D2Game,squall} (+ an emscripten.h defining EMSCRIPTEN_KEEPALIVE empty)
// The in-game marker (0x04) goes out only once the client's room is ready: for a spawned client with
// its player (a join) and for a client changing act (no player needed), never for another state, and
// it moves the client to the in-game state and sends its items.
#include "d2game_2d9b0_client_entered.cpp"
#include <cassert>

static int g_roomReady, g_hasPlayer, g_sent, g_lastOpcode, g_items;
static D2UnitStrc g_player;
BOOL __stdcall D2Common_10073(D2ActiveRoomStrc*) { return g_roomReady; }
D2UnitStrc* __fastcall CLIENTS_GetPlayerFromClient(D2ClientStrc*, int) { return g_hasPlayer ? &g_player : nullptr; }
void __fastcall D2GAME_PACKETS_SendHeaderOnlyPacket(D2ClientStrc*, uint8_t op) { ++g_sent; g_lastOpcode = op; }
void CLIENTS_SetClientState(D2ClientStrc* c, D2ClientState s) { c->dwClientState = s; }
void __fastcall D2GAME_ITEMS_UpdateInventoryItems_6FC45050(D2GameStrc*, D2UnitStrc*, int, int) { ++g_items; }

static int run(int state, int ready, int player, int* sent = nullptr, int* items = nullptr, int* after = nullptr) {
    D2ClientStrc c{}; c.dwClientState = static_cast<D2ClientState>(state);
    g_roomReady = ready; g_hasPlayer = player; g_sent = 0; g_items = 0; g_lastOpcode = -1;
    const int r = D2GAME_UpdateSpawnedClient(nullptr, &c);
    if (sent) *sent = g_sent;
    if (items) *items = g_items;
    if (after) *after = c.dwClientState;
    return r;
}

int main() {
    int sent, items, after;
    // A join (state 3): needs the room and the player.
    assert(run(CLIENTSTATE_PLAYER_SPAWNED, 1, 1, &sent, &items, &after) == 1);
    assert(sent == 1 && g_lastOpcode == 4 && items == 1 && after == CLIENTSTATE_INGAME);
    assert(run(CLIENTSTATE_PLAYER_SPAWNED, 0, 1, &sent) == 0 && sent == 0);
    assert(run(CLIENTSTATE_PLAYER_SPAWNED, 1, 0, &sent) == 0 && sent == 0);
    // An act change (state 5): the room alone arms it; before the room is ready nothing is sent and the state stays.
    assert(run(CLIENTSTATE_CHANGING_ACT, 0, 1, &sent, &items, &after) == 0);
    assert(sent == 0 && items == 0 && after == CLIENTSTATE_CHANGING_ACT);
    assert(run(CLIENTSTATE_CHANGING_ACT, 1, 1, &sent, &items, &after) == 1);
    assert(sent == 1 && g_lastOpcode == 4 && items == 1 && after == CLIENTSTATE_INGAME);
    assert(run(CLIENTSTATE_CHANGING_ACT, 1, 0, &sent, &items, &after) == 1 && sent == 1 && items == 0);
    // A client already in game, or not yet spawned, is left alone; so is a missing client.
    assert(run(CLIENTSTATE_INGAME, 1, 1, &sent) == 0 && sent == 0);
    assert(run(CLIENTSTATE_ACT_INIT_SENT, 1, 1, &sent) == 0 && sent == 0);
    assert(D2GAME_UpdateSpawnedClient(nullptr, nullptr) == 0);
    return 0;
}
