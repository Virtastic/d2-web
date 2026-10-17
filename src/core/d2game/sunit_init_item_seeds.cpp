#include "core/d2game/sunit_init_item_seeds.h"

#include <D2Items.h>
#include <D2Seed.h>
#include <D2Game/ITEMS/Items.h>

// 1.13c D2Game+0xE00C0: give a freshly allocated item its seed pair. The item seed starts at {1, 666}, one
// game-seed roll is taken and stored as the item's start seed, and the item seed's low word becomes that
// start seed (high stays 666). Return the item seed pointer, which is what retail leaves in EAX.
D2SeedStrc* SUNIT_InitItemSeeds(D2UnitStrc* pUnit, D2SeedStrc* pGameSeed)
{
    ITEMS_InitItemSeed(pUnit);
    ITEMS_SetItemStartSeed(pUnit, static_cast<int32_t>(ITEMS_RollRandomNumber(pGameSeed)));

    const int32_t nStartSeed = ITEMS_GetItemStartSeed(pUnit);
    D2SeedStrc* pItemSeed = ITEMS_GetItemSeed(pUnit);
    SEED_InitLowSeed(pItemSeed, nStartSeed);
    return pItemSeed;
}
