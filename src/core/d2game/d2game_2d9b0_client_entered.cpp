// 1.13c D2Game+0x2D9B0, the per-client arm of the frame's client update for a client whose player
// has spawned (state 3) or is changing act (state 5): once the client's room is ready the game sends
// the one-byte "in game" marker (0x04), moves the client to the in-game state and sends what its
// player holds. The state 5 arm is the end of an act change (LEVEL_ChangeAct leaves the client in
// state 5 after 0x05/0x03/0x07): it needs only the room, not the player, before the marker. Retail has
// no separate function for the marker: the send is inline in the update, after the client's unit
// sync of the same frame. Returns 1 on the frame the client entered.
//
// Not carried here, each waiting for its own port: the per-client quest setup (0x2C120), the
// party roster walk, the realm's enter-game callback and the 0x5A "joined the game" line sent to
// every client.
#include <cstdint>
#include <D2Dungeon.h>
#include <GAME/Clients.h>
#include <GAME/SCmd.h>
#include <ITEMS/ItemMode.h>

extern "C" int32_t __fastcall D2GAME_UpdateSpawnedClient(D2GameStrc* pGame, D2ClientStrc* pClient)
{ // 1.13c D2Game+0x2D9B0 (state 3 and state 5 arms)
    if (!pClient) return 0;
    if (pClient->dwClientState == CLIENTSTATE_CHANGING_ACT) {
        if (!D2Common_10073(pClient->pRoom)) return 0;
    } else if (pClient->dwClientState != CLIENTSTATE_PLAYER_SPAWNED ||
               !D2Common_10073(pClient->pRoom) || !CLIENTS_GetPlayerFromClient(pClient, 0)) {
        return 0;
    }
    D2GAME_PACKETS_SendHeaderOnlyPacket(pClient, 4u);
    CLIENTS_SetClientState(pClient, CLIENTSTATE_INGAME);
    if (D2UnitStrc* pPlayer = CLIENTS_GetPlayerFromClient(pClient, 0))
        D2GAME_ITEMS_UpdateInventoryItems_6FC45050(pGame, pPlayer, 0, 1);
    return 1;
}
