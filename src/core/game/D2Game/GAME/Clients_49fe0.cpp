#include "GAME/Clients.h"
#include "GAME/Game.h"
#include <test_seam.h>
#include <Fog.h>

// 1.13c D2Game+0x49FE0
// Client lookup by id. The decompile hides both arguments: EAX = pGame (null is the 0xE9 assert), ECX = nClientId. The list at
// pGame+0x88 is walked through pNext (+0x4A8) and the first client whose id (+0x00) matches is returned, else null.
// Delta from 1.10f (GAME/Clients.cpp): none in the walk.
D2ClientStrc* __fastcall CLIENTS_GetClientFromClientId(D2GameStrc* pGame, int32_t nClientId)
{
	if (D2Seam<&CLIENTS_GetClientFromClientId>::hook) return D2Seam<&CLIENTS_GetClientFromClientId>::hook(pGame, nClientId);
    D2_ASSERT(pGame);

    for (D2ClientStrc* pClient = pGame->pClientList; pClient; pClient = pClient->pNext)
    {
        if (pClient->dwClientId == (uint32_t)nClientId)
        {
            return pClient;
        }
    }

    return nullptr;
}
