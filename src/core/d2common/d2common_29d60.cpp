// 1.13c D2Common+0x29D60 (stdcall export #10107, ret 0x18, six stack words).
// The thunk loads word 0 (the player) into EAX, pushes words 1..5 (item, difficulty, quest flags, vendor id,
// transaction type) and calls 1.13c 0x28E80 (ITEMS_CalculateTransactionCost), whose eax result it returns unchanged.
// Nothing is dropped: the returned value is the transaction cost.
#include <cstdint>
#include "../d2moo/include/D2Items.h"

int32_t D2Common_29D60_Forward(uint32_t pPlayer, uint32_t pItem, int32_t nDifficulty, uint32_t pQuestFlags,
                               int32_t nVendorId, int32_t nTransactionType)
{
    return ITEMS_CalculateTransactionCost((D2UnitStrc*)(uintptr_t)pPlayer, (D2UnitStrc*)(uintptr_t)pItem,
                                          (D2C_Difficulties)nDifficulty, (D2BitBufferStrc*)(uintptr_t)pQuestFlags,
                                          nVendorId, (D2C_TransactionTypes)nTransactionType);
}
