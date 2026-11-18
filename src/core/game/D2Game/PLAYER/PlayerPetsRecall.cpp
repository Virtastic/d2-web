#include "PLAYER/PlayerPets.h"
#include <Units/Player.h>
#include "UNIT/SUnit.h"
#include <test_seam.h>

// 1.13c D2Game+0x972C0
// EAX = the pet list, EBX = the game, ECX = the room, two stack words. The list's pet data is walked and each live entry (flag bit 0 clear) looks up its monster unit by GUID in the game's unit hash; a hit is moved to the room through sub_6FC7E550 (1.13c 0x971F0). A null game with a live entry is a fatal assert (line 0x2D3).
void __fastcall PLAYERPETS_RecallPetList(D2GameStrc* pGame, D2PetListStrc* pPetList, D2ActiveRoomStrc* pRoom, int32_t a4, int32_t a5)
{
    for (D2PetDataStrc* pPetData = pPetList->pPetData; pPetData; pPetData = pPetData->pNext)
    {
        if (!(pPetData->nFlags & 1))
        {
            D2_ASSERT(pGame);
            D2UnitStrc* pPet = D2_CALL(SUNIT_GetServerUnit, pGame, UNIT_MONSTER, pPetData->nUnitGUID);
            if (pPet)
            {
                D2_CALL(sub_6FC7E550, pGame, pPet, pPetData, pRoom, a4, a5);
            }
        }
    }
}
