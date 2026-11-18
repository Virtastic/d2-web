#include "GAME/Game113c.h"
#include "GAME/SCmd.h"
#include "GAME/Clients.h"
#include <Fog.h>
#include <test_seam.h>

// 1.13c D2Game+0x2AC60
// ESI = client. Sends the 0x01 game-flags packet (the 0x8C420 body, header 1), the header-only packet 0, then marks the client CLIENTSTATE_GAME_INIT_SENT; a null client is the 0x95F assert.
void __fastcall GAME_SendGameInitToClient(D2ClientStrc* pClient)
{
    D2_CALL(D2GAME_PACKETS_SendPacket0x01_6FC3C7C0, pClient, 1, pClient ? pClient->pGame : nullptr);
    D2_CALL(D2GAME_PACKETS_SendHeaderOnlyPacket, pClient, 0);
    D2_ASSERT(pClient);
    pClient->dwClientState = CLIENTSTATE_GAME_INIT_SENT;
}
