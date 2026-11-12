// 1.13c D2Game+0x2D9B0, the per-client arm of the frame's client update for a client whose player
// has spawned (state 3) or is changing act (state 5): once the client's room is ready the game sends
// the one-byte "in game" marker (0x04), moves the client to the in-game state and sends what its
// player holds. The state 5 arm is the end of an act change (LEVEL_ChangeAct leaves the client in
// state 5 after 0x05/0x03/0x07): it needs only the room, not the player, before the marker. Retail has
// no separate function for the marker: the send is inline in the update, after the client's unit
// sync of the same frame. Returns 1 on the frame the client entered.
//
// The state 3 arm then runs the roster sync (0x2C120), the living-player walk (0xDFC10) and sends the
// 0x5A "joined the game" line to every in-game client (0xCBDB0).
#include <cstdint>
#include <cstring>
#include <D2Dungeon.h>
#include <D2Constants.h>
#include <D2PacketDef.h>
#include <Storm.h>
#include <GAME/Clients.h>
#include <GAME/Game.h>
#include <GAME/SCmd.h>
#include <ITEMS/ItemMode.h>
#include <PLAYER/PlayerList.h>
#include <PLAYER/PlrMsg.h>

extern "C" int32_t __fastcall D2GAME_UpdateSpawnedClient(D2GameStrc* pGame, D2ClientStrc* pClient)
{ // 1.13c D2Game+0x2D9B0 (state 3 and state 5 arms)
    if (!pClient) return 0;
    const bool bChangingAct = pClient->dwClientState == CLIENTSTATE_CHANGING_ACT;
    if (bChangingAct) {
        if (!D2Common_10073(pClient->pRoom)) return 0;
    } else if (pClient->dwClientState != CLIENTSTATE_PLAYER_SPAWNED ||
               !D2Common_10073(pClient->pRoom) || !CLIENTS_GetPlayerFromClient(pClient, 0)) {
        return 0;
    }
    D2GAME_PACKETS_SendHeaderOnlyPacket(pClient, 4u);
    CLIENTS_SetClientState(pClient, CLIENTSTATE_INGAME);
    if (D2UnitStrc* pPlayer = CLIENTS_GetPlayerFromClient(pClient, 0))
        D2GAME_ITEMS_UpdateInventoryItems_6FC45050(pGame, pPlayer, 0, 1);
    if (bChangingAct) return 1;
    // State 3 only: the roster (0x5B each way, then the arena scores 0x65), the living-player walk
    // (player lists and 0x8D), then the "joined" line (0x5A) to every in-game client. The realm's
    // enter-game callback is not set in a local game.
    sub_6FC369C0(pGame, pClient);
    D2UnitStrc* pNew = CLIENTS_GetPlayerFromClient(pClient, 1);
    if (!pNew) return 1;
    sub_6FCBAB20(pGame, pNew);
    D2GSPacketSrv5A p = {};
    p.nHeader = 0x5A;
    p.nType = EVENTTYPE_PLAYERJOIN;
    p.nColor = STRCOLOR_DARK_GOLD;
    char szAccount[0x32] = {};
    SStrCopy(szAccount, pClient->szAccount, sizeof szAccount);
    if (szAccount[0]) { SStrCopy(&p.szText[16], szAccount, 16u); p.szText[31] = '\0'; }
    SStrCopy(p.szText, pClient->szName, 16u);
    if (std::memchr(p.szText, 0, 16))
        for (D2ClientStrc* c = pGame->pClientList; c; c = c->pNext)
            if (c->dwClientState == CLIENTSTATE_INGAME) D2GAME_PACKETS_SendPacket0x5A_6FC3DEC0(c, &p);
    return 1;
}
