#include "UNIT/Party.h"
#include "GAME/Game113c.h"
#include <Fog.h>

// 1.13c D2Game+0x9BD00
// EAX = game, ECX = party id (the low word is compared). Walks the game's party list and returns the party with that id, or null; a null party control is the 0xD2 assert.
D2PartyStrc* __fastcall PARTY_GetPartyById(D2GameStrc* pGame, int16_t nPartyId)
{
    D2_ASSERT(pGame->pPartyControl);

    for (D2PartyStrc* pParty = pGame->pPartyControl->pParties; pParty; pParty = pParty->pNext)
    {
        if (pParty->nPartyId == nPartyId)
        {
            return pParty;
        }
    }
    return nullptr;
}
