#include "GAME/Clients.h"
#include "D2Dungeon.h"
#include <test_seam.h>
#include <cstring>

//D2Game.0x6FC33640
// 1.13c D2Game+0x4AD80 (EBX = client, stack = max count). Builds the distance-sorted node array from every unit on
// the client's adjacent rooms and flattens it into client+0x1CC.
void __fastcall CLIENTS_RefreshUnitsUpdateList(D2ClientStrc* pClient, uint32_t nUpdateSize)
{
    D2_ASSERT(pClient->pRoom);

    D2ActiveRoomStrc** ppRoomList = nullptr;
    int32_t nNumRooms = 0;
    D2_CALL(DUNGEON_GetAdjacentRoomsListFromRoom, pClient->pRoom, &ppRoomList, &nNumRooms);
    D2_ASSERT(ppRoomList);

    D2UnitStrc* pPlayer = D2_CALL(CLIENTS_GetPlayerFromClient, pClient, 0);
    if (!pPlayer)
    {
        return;
    }

    D2ClientUnitUpdateSortStrc aNodes[0x39];
    memset(aNodes, 0, sizeof(aNodes));

    const int32_t nX = D2_CALL(CLIENTS_GetUnitX, pPlayer);
    const int32_t nY = D2_CALL(CLIENTS_GetUnitY, pPlayer);
    uint32_t nBest = 0;
    pClient->nUnitUpdateIndex = 0;

    for (int32_t i = 0; i < nNumRooms; ++i)
    {
        for (D2UnitStrc* pUnit = ppRoomList[i]->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
        {
            D2_CALL(D2GAME_ClientsUpdateInsert_6FC6A460, pUnit, nY, &nBest, pClient, nUpdateSize, aNodes, nX);
        }
    }

    D2_CALL(CLIENTS_SetUnitsUpdateList, pClient, aNodes);
}
